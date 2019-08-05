#include "edz_vc_service.hpp"

#include "vc_manager.hpp"
#include "results.hpp"

namespace edz::vc {

    void EdzVCService::isVCConnected(Out<bool> out) {

    }
    
    Result EdzVCService::connectVC(u64 bodyColor, u64 buttonColor)  {
        return VirtualControllerManager::getInstance().connectVC(bodyColor, buttonColor);
    }
    
    Result EdzVCService::disconnectVC() {
        return VirtualControllerManager::getInstance().disconnectVC();
    }   
    

    Result EdzVCService::addVCButton(Out<VirtualControllerButtonID> vcButtonID, u64 button) {
        return VirtualControllerManager::getInstance().addVCButton(vcButtonID.GetPointer(), button);
    }
    
    Result EdzVCService::removeVCButton(VirtualControllerButtonID vcButtonID) {
        return VirtualControllerManager::getInstance().removeVCButton(vcButtonID);
    }
    
    
    Result EdzVCService::getVCFrameCount(Out<u64> count, VirtualControllerButtonID vcButtonID) {
        return VirtualControllerManager::getInstance().getVCFrameCount(count.GetPointer(), vcButtonID);
    }
    
    Result EdzVCService::getVCFrames(OutBuffer<VirtualControllerFrame> frames, Out<u64> count, size_t bufferSize, VirtualControllerButtonID vcButtonID) {
        size_t frameCount = 0;
        VirtualControllerManager::getInstance().getVCFrameCount(&frameCount, vcButtonID);

        if (frameCount * sizeof(VirtualControllerFrame) > bufferSize)
            return ResultEdzInvalidBuffer;

        return VirtualControllerManager::getInstance().getVCFrames(frames.buffer, count.GetPointer(), bufferSize, vcButtonID);
    }
    
    Result EdzVCService::addVCFrame(VirtualControllerButtonID vcButtonID, VirtualControllerFrame frame) {
        return VirtualControllerManager::getInstance().addVCFrame(vcButtonID, frame);
    }
    
    Result EdzVCService::clearVCFrames(VirtualControllerButtonID vcButtonID) {
        return VirtualControllerManager::getInstance().clearVCFrames(vcButtonID);
    }
    

}