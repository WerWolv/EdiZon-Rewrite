#pragma once

#include <switch.h>
#include "edz_vc_types.hpp"
#include "result.hpp"

#include <vector>
#include <map>
#include <queue>

namespace edz::vc {

    class EdzVCManager {
    public:  
        static EdzVCManager& getInstance() {
            static EdzVCManager instance;
            
            return instance;
        }

        EdzVCManager(EdzVCManager const&) = delete;
        void operator=(EdzVCManager const&) = delete;
        
        void process();
        
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
        EdzVCManager();
        ~EdzVCManager();

        std::map<u64, VirtualControllerButtonID> m_buttonIDs;
        std::map<VirtualControllerButtonID, std::vector<VirtualControllerFrame>> m_vcPlaybacks;

        Mutex m_btnMutex;
    };
}