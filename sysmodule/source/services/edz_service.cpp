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

#include "services/edz_service.hpp"

namespace edz::serv {

    ams::Result EdzService::GetLogs(ams::sf::OutArray<int> outLogs, ams::sf::Out<u64> outCount) {

    }

    ams::Result EdzService::SetOverlayKeyCombo(u64 modifierKeys, u64 activationKey) {

    }

    ams::Result EdzService::AddNote(ams::sf::InArray<char>, size_t size, u64 gameTitleId) {

    }

    ams::Result EdzService::ClearNotes() {

    }


    ams::Result EdzService::AttachVirtualController(u64 bodyColor, u64 buttonColor, u64 leftGripColor, u64 rightGripColor) {

    }

    ams::Result EdzService::DetachVirtualController() {

    }

}