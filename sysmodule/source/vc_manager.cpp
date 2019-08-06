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

#include "vc_manager.hpp"

#include "results.hpp"

namespace edz::vc {

        VirtualControllerManager::VirtualControllerManager() {
            mutexInit(&m_btnMutex);
            
            this->m_hldsHandle = 0;
        }
        
        VirtualControllerManager::~VirtualControllerManager() {
            
        }


        void VirtualControllerManager::process(u64 kDown) {
            HiddbgHdlsState state = { 0 };

            mutexLock(&this->m_btnMutex);

            if (this->m_buttonIDs[kDown] != 0) {
                if (this->m_hldsHandle != 0) {
                    state.buttons = (kDown << 1);
                }
            } else {
                state.buttons = 0;
            }
            
            state.buttons |= (state.buttons & KEY_SL) ? 0x00 : KEY_SL; // Spam click the inexistant SL button to keep the virtual controller in slot one
            state.batteryCharge = 4;

            hiddbgSetHdlsState(this->m_hldsHandle, &state);

            mutexUnlock(&this->m_btnMutex);
        }

        bool VirtualControllerManager::isVCConnected() {
            return true;
        }

        EResult VirtualControllerManager::connectVC(u64 bodyColor, u64 buttonColor) {
            HiddbgHdlsDeviceInfo deviceInfo = {0};
            HiddbgHdlsState state = { 0 };
            
            deviceInfo.singleColorBody = bodyColor;
            deviceInfo.singleColorButtons = buttonColor;
            deviceInfo.type = BIT(0);
            deviceInfo.type2 = 0x00;

            if (EResult(hiddbgAttachHdlsWorkBuffer()).failed())
                return ResultEdzAttachFailed;

            if (EResult(hiddbgAttachHdlsVirtualDevice(&this->m_hldsHandle, &deviceInfo)).failed())
                return ResultEdzAttachFailed;

            state.batteryCharge = 4;
            state.powerConnected = true;

            hiddbgSetHdlsState(this->m_hldsHandle, &state);

            return ResultSuccess;
        }

        EResult VirtualControllerManager::disconnectVC() {
            if (EResult(hiddbgDetachHdlsVirtualDevice(this->m_hldsHandle)).failed())
                return ResultEdzDetachFailed;
            
            hiddbgReleaseHdlsWorkBuffer();

            this->m_hldsHandle = 0;

            return ResultSuccess;
        }

        static u64 getUID() {
            static u64 nextUID = 1;

            return nextUID++;
        }


        EResult VirtualControllerManager::addVCButton(VirtualControllerButtonID *vcButtonID, u64 button) {
            if (this->m_buttonIDs.find(button) != this->m_buttonIDs.end())
                return ResultEdzAlreadyRegistered;

            mutexLock(&this->m_btnMutex);

            *vcButtonID = getUID();
            this->m_buttonIDs[button] = *vcButtonID;

            mutexUnlock(&this->m_btnMutex);

            return ResultSuccess;
        }

        EResult VirtualControllerManager::removeVCButton(VirtualControllerButtonID vcButtonID) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            mutexLock(&this->m_btnMutex);

            m_vcPlaybacks.erase(vcButtonID);

            mutexUnlock(&this->m_btnMutex);

            return ResultSuccess;
        }
        

        EResult VirtualControllerManager::getVCFrameCount(u64 *count, VirtualControllerButtonID vcButtonID) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            *count = m_vcPlaybacks[vcButtonID].size();

            return ResultSuccess;
        }
        
        EResult VirtualControllerManager::getVCFrames(VirtualControllerFrame *frames, u64 *count, size_t bufferSize, VirtualControllerButtonID vcButtonID) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            for (u32 i = 0; i < (bufferSize / sizeof(VirtualControllerFrame)); i++) {
                if (i >= this->m_vcPlaybacks[vcButtonID].size()) break;

                frames[i] = this->m_vcPlaybacks[vcButtonID][i];
            }

            return ResultSuccess;
        }
        
        EResult VirtualControllerManager::addVCFrame(VirtualControllerButtonID vcButtonID, VirtualControllerFrame frame) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            mutexLock(&this->m_btnMutex);

            this->m_vcPlaybacks[vcButtonID].push_back(frame);

            mutexUnlock(&this->m_btnMutex);

            return ResultSuccess;
        }
        
        EResult VirtualControllerManager::clearVCFrames(VirtualControllerButtonID vcButtonID) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            mutexLock(&this->m_btnMutex);

            this->m_vcPlaybacks[vcButtonID].clear();

            mutexUnlock(&this->m_btnMutex);

            return ResultSuccess;
        }       

}