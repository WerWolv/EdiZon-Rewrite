#pragma once

#include <switch.h>
#include <stratosphere.hpp>

#include "edz_vc_types.hpp"

namespace edz::vc {

    class EdzVCService final : public IServiceObject {
        private:
            enum class CommandId {
                isVCConnected           = 65000,
                connectVC               = 65001,
                disconnectVC            = 65002,

                addVCButton             = 65100,
                removeVCButton          = 65101,

                getVCFrameCount         = 65200,
                getVCFrames             = 65201,
                addVCFrame              = 65202,
                clearVCFrames           = 65203

            };
        private:
            void isVCConnected(Out<bool> out);
            Result connectVC(u64 bodyColor, u64 buttonColor);
            Result disconnectVC();

            Result addVCButton(Out<VirtualControllerButtonID> vcButtonID, u64 button);
            Result removeVCButton(VirtualControllerButtonID vcButtonID);
            
            Result getVCFrameCount(Out<u64> count, VirtualControllerButtonID vcButtonID);
            Result getVCFrames(OutBuffer<VirtualControllerFrame> frames, Out<u64> count, size_t bufferSize, VirtualControllerButtonID vcButtonID);
            Result addVCFrame(VirtualControllerButtonID vcButtonID, VirtualControllerFrame frame);
            Result clearVCFrames(VirtualControllerButtonID vcButtonID);

        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(EdzVCService, isVCConnected),
                MAKE_SERVICE_COMMAND_META(EdzVCService, connectVC),
                MAKE_SERVICE_COMMAND_META(EdzVCService, disconnectVC),

                MAKE_SERVICE_COMMAND_META(EdzVCService, addVCButton),
                MAKE_SERVICE_COMMAND_META(EdzVCService, removeVCButton),

                MAKE_SERVICE_COMMAND_META(EdzVCService, getVCFrameCount),
                MAKE_SERVICE_COMMAND_META(EdzVCService, getVCFrames),
                MAKE_SERVICE_COMMAND_META(EdzVCService, addVCFrame),
                MAKE_SERVICE_COMMAND_META(EdzVCService, clearVCFrames),
            };
    };

}
