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
#include <stratosphere.hpp>

namespace edz::serv {

    class EdzService final : public ams::sf::IServiceObject {
        private:
            enum class CommandId {
                GetLogs                     = 65000,
                SetOverlayKeyCombo          = 65001,

                AddNote                     = 65010,
                ClearNotes                  = 65011,

                AttachVirtualController     = 65020,
                DetachVirtualController     = 65021
            };

        public:
            ams::Result GetLogs(ams::sf::OutArray<int> outLogs, ams::sf::Out<u64> outCount);
            ams::Result SetOverlayKeyCombo(u64 modifierKeys, u64 activationKey);
            ams::Result AddNote(ams::sf::InArray<char>, size_t size, u64 gameTitleId);
            ams::Result ClearNotes();

            ams::Result AttachVirtualController(u64 bodyColor, u64 buttonColor, u64 leftGripColor, u64 rightGripColor);
            ams::Result DetachVirtualController();
        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(GetLogs),
                MAKE_SERVICE_COMMAND_META(SetOverlayKeyCombo),
                MAKE_SERVICE_COMMAND_META(AddNote),
                MAKE_SERVICE_COMMAND_META(ClearNotes),
                MAKE_SERVICE_COMMAND_META(AttachVirtualController),
                MAKE_SERVICE_COMMAND_META(DetachVirtualController)
            };
    };

}
