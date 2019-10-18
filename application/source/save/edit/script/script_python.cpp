/**
 * Copyright (C) 2019 WerWolv
 * 
 * This file is part of EdiZon.
 * 
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "save/edit/script/script_python.hpp"

namespace edz::save::edit {

    static std::map<PyObject*,ScriptPython*> pythonExtraSpace;

    using mem_func = PyObject* (ScriptPython::*)(PyObject *, PyObject *);
    template <mem_func func>
    PyObject *dispatch(PyObject *self, PyObject *args) {
        ScriptPython *ptr = *reinterpret_cast<ScriptPython**>(&pythonExtraSpace[self]);
        return ((*ptr).*func)(self, args);
    }

    ScriptPython::ScriptPython(std::string path) : Script(path) {
        Py_Initialize();

        PyObject *sysPath = PySys_GetObject("path");
        PyObject *stdlibPath = PyUnicode_FromString("romfs:/libraries/python");
        PyObject *scriptPath = PyUnicode_FromString(EDIZON_BASE_DIR "/scripts");
        PyObject *libsPath = PyUnicode_FromString(EDIZON_BASE_DIR "/scripts/libs");

        PyList_Insert(sysPath, 0, stdlibPath);
        PyList_Insert(sysPath, 0, scriptPath);
        PyList_Insert(sysPath, 0, libsPath);

        PyImport_AppendInittab("edizon", []() -> PyObject* {
            PyMethodDef edizonMethods[4];
            PyModuleDef edizonModule;

            edizonMethods[0] = { "getDataAsBuffer", &dispatch<&ScriptPython::getDataAsBuffer>, METH_NOARGS, "getDataAsBuffer" };
            edizonMethods[1] = { "getDataAsString", &dispatch<&ScriptPython::getDataAsString>, METH_NOARGS, "getDataAsString" };
            edizonMethods[2] = { "getArgument", &dispatch<&ScriptPython::getArgument>, METH_NOARGS, "getArgument" };
            edizonMethods[3] = { nullptr, nullptr, 0, nullptr };
            edizonModule = { PyModuleDef_HEAD_INIT, "edizon", "EdiZon API", 0, edizonMethods, nullptr, nullptr, nullptr, nullptr };

            PyObject *m = PyModule_Create(&edizonModule);

            if (m == nullptr)
                return nullptr;

            PyObject *error = PyErr_NewException("edz.error", nullptr, nullptr);

            Py_INCREF(error);

            PyModule_AddObject(m, "error", error);
            return m;
        });

        this->m_ctx = PyImport_Import(PyUnicode_FromString("__main__"));

        pythonExtraSpace.insert({ this->m_ctx, this });

        FILE *scriptFile = fopen(std::string(path).c_str(), "r");

        if (scriptFile == nullptr) {
            Logger::error("Failed to load Python script file");
            return;
        }

        if(PyRun_SimpleFileEx(scriptFile, std::string(path).c_str(), true) != 0) {
            Logger::error("Failed to execute Python script's global scope:");
            PyErr_Print();
            printf("\n");
            fclose(scriptFile);

            return;
        }

        fclose(scriptFile);

        Logger::info("Python script successfully loaded!");
    }

    ScriptPython::~ScriptPython() {
        Py_Finalize();
    }


    std::tuple<EResult, Argument> ScriptPython::getValue() {
        Argument out;

        PyObject *func = PyObject_GetAttrString(this->m_ctx, "getValue");
        PyObject *result = PyObject_CallObject(func, nullptr);

        if (result == nullptr) 
            return { ResultEdzScriptRuntimeError, nullptr };

        if (PyErr_Occurred() != nullptr) {
            Logger::error("Python script's getValue function failed:");
            PyErr_Print();
            printf("\n");

            return { ResultEdzScriptRuntimeError, nullptr };
        }

        if (PyLong_Check(result))
            out = static_cast<s128>(PyLong_AsLong(result));
        else if (PyFloat_Check(result))
            out = static_cast<double>(PyFloat_AsDouble(result));
        else if (PyBool_Check(result))
            out = static_cast<bool>(PyObject_IsTrue(result));
        else if (PyUnicode_Check(result))
            out = std::string(PyUnicode_AsUTF8(result));
        else {
            Logger::error("Invalid value returned from Python script's getValue!");
            return { ResultEdzScriptRuntimeError, nullptr };
        }
        
        return { ResultSuccess, out };
    }

    EResult ScriptPython::setValue(Argument value) {
        PyObject *func = PyObject_GetAttrString(this->m_ctx, "setValue");


        if (s128 *v = std::get_if<s128>(&value))
            PyObject_CallObject(func, PyTuple_Pack(1, PyLong_FromLong(*v)));
        else if (double *v = std::get_if<double>(&value))
            PyObject_CallObject(func, PyTuple_Pack(1, PyFloat_FromDouble(*v)));
        else if (bool *v = std::get_if<bool>(&value))
            PyObject_CallObject(func, PyTuple_Pack(1, PyBool_FromLong(*v)));
        else if (std::string *v = std::get_if<std::string>(&value))
            PyObject_CallObject(func, PyTuple_Pack(1, PyUnicode_FromStringAndSize(v->c_str(), v->length())));
        else {
            Logger::error("Invalid Argument type");
            return ResultEdzScriptInvalidArgument;
        }

        if (PyErr_Occurred() != nullptr) {
            Logger::error("Python script's setValue function failed:");
            PyErr_Print();
            printf("\n");

            return ResultEdzScriptRuntimeError;
        }

        return ResultSuccess;
    }

    std::tuple<EResult, std::optional<std::vector<u8>>> ScriptPython::getModifiedSaveData() {
        PyObject *func = PyObject_GetAttrString(this->m_ctx, "getModifiedSaveData");
        PyObject *result = PyObject_CallObject(func, nullptr);

        if (result == nullptr) 
            return { ResultEdzScriptRuntimeError, EMPTY_RESPONSE };

        if (PyErr_Occurred() != nullptr) {
            Logger::error("Python script's getModifiedSaveData function failed:");
            PyErr_Print();
            printf("\n");

            return { ResultEdzScriptRuntimeError, EMPTY_RESPONSE };
        }

        size_t size = PyByteArray_Size(result);
        char *data = PyByteArray_AsString(result);

        return { ResultSuccess, std::vector<u8>(data, data + size) };
    }
    

    PyObject* ScriptPython::getArgument(PyObject *self, PyObject *args) {
        char *argName;
        PyArg_ParseTuple(args, "s", &argName);

        Argument& arg = this->m_arguments.at(argName);

        if (s128 *v = std::get_if<s128>(&arg))
            return PyLong_FromLong(*v);
        else if (double *v = std::get_if<double>(&arg))
            return PyFloat_FromDouble(*v);
        else if (bool *v = std::get_if<bool>(&arg))
            return PyBool_FromLong(*v);
        else if (std::string *v = std::get_if<std::string>(&arg))
            return PyUnicode_FromStringAndSize(v->c_str(), v->length());

        return nullptr;
    }

    PyObject* ScriptPython::getDataAsBuffer(PyObject *self, PyObject *args) {
        return PyByteArray_FromStringAndSize(reinterpret_cast<char*>(this->m_saveData), this->m_saveSize);
    }

    PyObject* ScriptPython::getDataAsString(PyObject *self, PyObject *args) {
        std::string string(reinterpret_cast<char*>(this->m_saveData), this->m_saveSize);

        return PyUnicode_FromStringAndSize(string.c_str(), string.length());
    }

}
