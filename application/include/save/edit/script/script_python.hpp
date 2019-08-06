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

#pragma once

#include <edizon.hpp>

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