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

#include <switch.h>
#include "edz_vc_types.hpp"
#include "result.hpp"

#include <vector>
#include <map>
#include <queue>

namespace edz::vc {

    class VirtualControllerManager {
    public:  
        static VirtualControllerManager& getInstance() {
            static VirtualControllerManager instance;
            
            return instance;
        }

        VirtualControllerManager(VirtualControllerManager const&) = delete;
        void operator=(VirtualControllerManager const&) = delete;
        
        void process(u64 kDown);
        
        bool isVCConnected();
        EResult connectVC(u64 bodyColor, u64 buttonColor);
        EResult disconnectVC();

        EResult addVCButton(VirtualControllerButtonID *vcButtonID, u64 button);
        EResult removeVCButton(VirtualControllerButtonID vcButtonID);
        
        EResult getVCFrameCount(u64 *count, VirtualControllerButtonID vcButtonID);
        EResult getVCFrames(VirtualControllerFrame *frames, u64 *count, size_t bufferSize, VirtualControllerButtonID vcButtonID);
        EResult addVCFrame(VirtualControllerButtonID vcButtonID, VirtualControllerFrame frame);
        EResult clearVCFrames(VirtualControllerButtonID vcButtonID);

    private:
        VirtualControllerManager();
        ~VirtualControllerManager();

        std::map<u64, VirtualControllerButtonID> m_buttonIDs;
        std::map<VirtualControllerButtonID, std::vector<VirtualControllerFrame>> m_vcPlaybacks;

        Mutex m_btnMutex;
        u64 m_hldsHandle;
    };
}