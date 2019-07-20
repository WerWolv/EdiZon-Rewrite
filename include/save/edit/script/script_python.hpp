#pragma once

#include "edizon.hpp"
#include <string>
#include <vector>
#include <map>

#include "save/edit/script/script.hpp"
#include "save/edit/widgets/widget.hpp"

#include <python/Python.h>

namespace edz::save::edit {

    class ScriptPython : public Script {
    public:
        ScriptPython(std::string scriptName);
        ~ScriptPython();


        std::shared_ptr<widget::Arg> getValue();
        void setValue(std::shared_ptr<widget::Arg> value);
        std::vector<u8> getModifiedSaveFileData();

    private:
        PyObject *m_ctx;

        // Script callable functions
        PyObject* getArgument(PyObject *self, PyObject *args);
        PyObject* getDataAsBuffer(PyObject *self, PyObject *args);
        PyObject* getDataAsString(PyObject *self, PyObject *args);
    };

}