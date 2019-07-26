#include "save/edit/script/script_python.hpp"

namespace edz::save::edit {

    typedef PyObject *(ScriptPython::*mem_func)(PyObject *, PyObject *);
    template <mem_func func>
    PyObject *dispatch(PyObject *self, PyObject *args) {
        ScriptPython *ptr = *reinterpret_cast<ScriptPython**>(&python_extraspace);
        return ((*ptr).*func)(self, args);
    }

    ScriptPython::ScriptPython(std::string scriptName) : Script(scriptName) {
        Py_Initialize();

        PyObject *sysPath = PySys_GetObject("path");
        PyObject *stdlibPath = PyUnicode_FromString("romfs:/libs/python");
        PyObject *scriptPath = PyUnicode_FromString(EDIZON_BASE_DIR"/editor/scripts");
        PyObject *libsPath = PyUnicode_FromString(EDIZON_BASE_DIR"/editor/scripts/libs");

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

        FILE *scriptFile = fopen(std::string(EDIZON_BASE_DIR"/editor/scripts" + scriptName).c_str(), "r");

        if (scriptFile == nullptr) {
            error("Failed to load Python script file");
            return;
        }

        if(PyRun_SimpleFileEx(scriptFile, std::string(EDIZON_BASE_DIR"/editor/scripts" + scriptName).c_str(), true) != 0) {
            error("Failed to execute Python script's global scope:");
            PyErr_Print();
            printf("\n");
            fclose(scriptFile);

            return;
        }

        fclose(scriptFile);

        info("Python script successfully loaded!");
    }

    ScriptPython::~ScriptPython() {
        Py_Finalize();
    }


    std::shared_ptr<widget::Arg> ScriptPython::getValue() {
        std::shared_ptr<widget::Arg> out;

        PyObject *func = PyObject_GetAttrString(this->m_ctx, "getValue");
        PyObject *result = PyObject_CallObject(func, nullptr);

        if (result == nullptr) 
            return nullptr; // Return empty argument

        if (PyErr_Occurred() != nullptr) {
            error("Python script's getValue function failed:");
            PyErr_Print();
            printf("\n");

            return;
        }

        if (PyLong_Check(result))
            out = widget::Arg::create("value", PyLong_AsLong(result));
        else if (PyFloat_Check(result))
            out = widget::Arg::create("value", PyFloat_AsDouble(result));
        else if (PyBool_Check(result))
            out = widget::Arg::create("value", PyObject_IsTrue(result));
        else if (PyUnicode_Check(result))
            out = widget::Arg::create("value", PyUnicode_AsUnicode(result));
        else error("Invalid value returned from Python script's getValue!");
        
        return out;
    }

    void ScriptPython::setValue(std::shared_ptr<widget::Arg> value) {
        PyObject *func = PyObject_GetAttrString(this->m_ctx, "setValue");


        switch(value->type) {
            case widget::Arg::ArgumentType::INTEGER:
                PyObject_CallObject(func, PyTuple_Pack(1, PyLong_FromLong(value->intArg)));
                break;
            case widget::Arg::ArgumentType::FLOAT:
                PyObject_CallObject(func, PyTuple_Pack(1, PyFloat_FromDouble(value->floatArg)));
                break;
            case widget::Arg::ArgumentType::BOOLEAN:
                PyObject_CallObject(func, PyTuple_Pack(1, PyBool_FromLong(value->boolArg)));
                break;            
            case widget::Arg::ArgumentType::STRING:
                PyObject_CallObject(func, PyTuple_Pack(1, PyUnicode_FromString(value->stringArg.c_str())));
                break;
        }

        if (PyErr_Occurred() != nullptr) {
            error("Python script's setValue function failed:");
            PyErr_Print();
            printf("\n");
        }
    }

    std::vector<u8> ScriptPython::getModifiedSaveFileData() {
        PyObject *func = PyObject_GetAttrString(this->m_ctx, "getModifiedSaveFileData");
        PyObject *result = PyObject_CallObject(func, nullptr);

        if (result == nullptr) 
            return std::vector<u8>(); // Return empty buffer

        if (PyErr_Occurred() != nullptr) {
            error("Python script's getModifiedSaveFileData function failed:");
            PyErr_Print();
            printf("\n");

            return std::vector<u8>(); // Return empty buffer
        }

        size_t size = PyByteArray_Size(result);
        char *data = PyByteArray_AsString(result);

        return std::vector<u8>(data, data + size);
    }
    

    PyObject* ScriptPython::getArgument(PyObject *self, PyObject *args) {
        char *argName;
        PyArg_ParseTuple(args, "s", &argName);

        std::shared_ptr<widget::Arg> arg = this->m_arguments.at(argName);

        if (arg == nullptr) {
            return nullptr;
        }

        switch(arg->type) {
            case widget::Arg::ArgumentType::INTEGER:
                return PyLong_FromLong(arg->intArg);
            case widget::Arg::ArgumentType::FLOAT:
                return PyFloat_FromDouble(arg->floatArg);
            case widget::Arg::ArgumentType::BOOLEAN:
                return PyBool_FromLong(arg->boolArg);
            case widget::Arg::ArgumentType::STRING:
                return PyUnicode_FromString(arg->stringArg.c_str());
        }
    }

    PyObject* ScriptPython::getDataAsBuffer(PyObject *self, PyObject *args) {
        return PyByteArray_FromStringAndSize(reinterpret_cast<char*>(this->m_saveFileData), this->m_saveFileSize);
    }

    PyObject* ScriptPython::getDataAsString(PyObject *self, PyObject *args) {
        std::string string(reinterpret_cast<char*>(this->m_saveFileData), this->m_saveFileSize);

        return PyUnicode_FromStringAndSize(string.c_str(), string.length());
    }

}
