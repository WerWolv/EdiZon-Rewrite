#include "edz_vc_manager.hpp"

#include "edz_results.hpp"

namespace edz::vc {

        EdzVCManager::EdzVCManager() {
            mutexInit(&m_btnMutex);
        }
        
        EdzVCManager::~EdzVCManager() {
            
        }


        void EdzVCManager::process() {
            hidScanInput();

            u64 buttons = hidKeysDown(CONTROLLER_HANDHELD);

            mutexLock(&this->m_btnMutex);

            if (this->m_buttonIDs[buttons] != 0)
                ; // Let virtual controller do its work here

            mutexUnlock(&this->m_btnMutex);
        }

        static u64 getUID() {
            static u64 nextUID = 1;

            return nextUID++;
        }


        EResult EdzVCManager::addVCButton(VirtualControllerButtonID *vcButtonID, u64 button) {
            if (this->m_buttonIDs.find(button) != this->m_buttonIDs.end())
                return ResultEdzAlreadyRegistered;

            mutexLock(&this->m_btnMutex);

            *vcButtonID = getUID();
            this->m_buttonIDs[button] = *vcButtonID;

            mutexUnlock(&this->m_btnMutex);

            return ResultSuccess;
        }

        EResult EdzVCManager::removeVCButton(VirtualControllerButtonID vcButtonID) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            mutexLock(&this->m_btnMutex);

            m_vcPlaybacks.erase(vcButtonID);

            mutexUnlock(&this->m_btnMutex);

            return ResultSuccess;
        }
        

        EResult EdzVCManager::getVCFrameCount(u64 *count, VirtualControllerButtonID vcButtonID) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            *count = m_vcPlaybacks[vcButtonID].size();

            return ResultSuccess;
        }
        
        EResult EdzVCManager::getVCFrames(VirtualControllerFrame *frames, u64 *count, size_t bufferSize, VirtualControllerButtonID vcButtonID) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            for (u32 i = 0; i < (bufferSize / sizeof(VirtualControllerFrame)); i++) {
                if (i >= this->m_vcPlaybacks[vcButtonID].size()) break;

                frames[i] = this->m_vcPlaybacks[vcButtonID][i];
            }
        }
        
        EResult EdzVCManager::addVCFrame(VirtualControllerButtonID vcButtonID, VirtualControllerFrame frame) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            mutexLock(&this->m_btnMutex);

            this->m_vcPlaybacks[vcButtonID].push_back(frame);

            mutexUnlock(&this->m_btnMutex);
        }
        
        EResult EdzVCManager::clearVCFrames(VirtualControllerButtonID vcButtonID) {
            if (this->m_vcPlaybacks.find(vcButtonID) == this->m_vcPlaybacks.end())
                return ResultEdzUnknownButtonID;

            mutexLock(&this->m_btnMutex);

            this->m_vcPlaybacks[vcButtonID].clear();

            mutexUnlock(&this->m_btnMutex);
        }       

}