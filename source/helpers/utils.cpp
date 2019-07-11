#include "helpers/utils.hpp"

#include <string>
#include <ctime>
#include <cstring>

#define MHz *1E6

namespace edz::helper {

    static HidsysNotificationLedPattern patternOn, patternOff;
    static u64 uniquePadIds[0x10];
    static size_t uniquePadCnt;


    bool askPctl(std::function<void()> f) {
        bool restricted = false;
        pctlIsRestrictionEnabled(&restricted);

        if (restricted)
            if (EResult(pctlauthShow(false)).failed())
                return false;
        
        f();

        return true;
    }

    bool askSwkbdText(std::function<void(std::string)> f, std::string headerText, std::string guideText, u8 maxStringLength, std::string initialText) {
        SwkbdConfig config;

        swkbdConfigMakePresetDefault(&config);
        swkbdConfigSetHeaderText(&config, headerText.c_str());
        swkbdConfigSetGuideText(&config, guideText.c_str());
        swkbdConfigSetInitialText(&config, initialText.c_str());
        swkbdConfigSetBlurBackground(&config, true);
        swkbdConfigSetType(&config, SwkbdType_Normal);
        swkbdConfigSetStringLenMax(&config, maxStringLength);
        swkbdConfigSetStringLenMaxExt(&config, 1);
        swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_At | SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

        char buffer[0x100];

        if (EResult(swkbdShow(&config, buffer, 0x100)).succeeded() && std::strcmp(buffer, "") != 0) {
            f(buffer);

            swkbdClose(&config);
            return true;
        }

        swkbdClose(&config);

        return false;
    }

    bool askSwkbdNumber(std::function<void(std::string)> f, std::string headerText, std::string guideText, std::string leftButton, std::string rightButton, u8 maxStringLength, std::string initialText) {
        SwkbdConfig config;

        swkbdConfigMakePresetDefault(&config);
        swkbdConfigSetHeaderText(&config, headerText.c_str());
        swkbdConfigSetGuideText(&config, guideText.c_str());
        swkbdConfigSetInitialText(&config, initialText.c_str());
        swkbdConfigSetBlurBackground(&config, true);
        swkbdConfigSetType(&config, SwkbdType_NumPad);
        swkbdConfigSetStringLenMax(&config, maxStringLength);
        swkbdConfigSetLeftOptionalSymbolKey(&config, leftButton.c_str());
        swkbdConfigSetRightOptionalSymbolKey(&config, rightButton.c_str());
        swkbdConfigSetStringLenMaxExt(&config, 1);
        swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_At | SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

        char buffer[0x100];

        if (EResult(swkbdShow(&config, buffer, 0x100)).succeeded() && std::strcmp(buffer, "") != 0) {
            f(buffer);

            swkbdClose(&config);
            return true;
        }

        swkbdClose(&config);

        return false;
    }

    bool isServiceRunning(const char *serviceName) {
        Handle handle;
        bool running = R_FAILED(smRegisterService(&handle, serviceName, false, 1));

        svcCloseHandle(handle);

        if (!running)
            smUnregisterService(serviceName);

        return running;
    }

    bool isOnRNX() {
        return isServiceRunning("tx") && isServiceRunning("rnx");
    }

    bool isOnSXOS() {
        return isServiceRunning("tx") && !isServiceRunning("rnx");
    }

    bool isOnAMS() {
        return !isOnRNX() && !isOnSXOS();
    }

    std::string getCurrentTimeString() {
        char buffer[0x10];

        time_t unixTime = time(nullptr);
        struct tm* time = localtime((const time_t*)&unixTime);

        sprintf(buffer, "%02d:%02d", time->tm_hour, time->tm_min);

        return std::string(buffer);
    }

    u8 getCurrentBatteryPercentage() {
        u32 percentage = 0;
        psmGetBatteryChargePercentage(&percentage);

        return percentage;
    }

    void overclockSystem(bool enable) {
        if (hosversionBefore(8, 0, 0)) {
            pcvSetClockRate(PcvModule_CpuBus, enable ? 1785 MHz : 1020 MHz);  // Set CPU clock
            pcvSetClockRate(PcvModule_EMC, enable ? 1600 MHz : 1331 MHz);     // Set memory clock
        } else {
            ClkrstSession clkrstSession;
            clkrstOpenSession(&clkrstSession, PcvModuleId_CpuBus, 3);
            clkrstSetClockRate(&clkrstSession, enable ? 1785 MHz : 1020 MHz); // Set CPU clock
            clkrstCloseSession(&clkrstSession);

            clkrstOpenSession(&clkrstSession, PcvModuleId_EMC, 3);
            clkrstSetClockRate(&clkrstSession, enable ? 1600 MHz : 1331 MHz); // Set memory clock
            clkrstCloseSession(&clkrstSession);
        }
    }

    void controllerLEDInitialize() {
        hidsysGetUniquePadsFromNpad(hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1, uniquePadIds, 2, &uniquePadCnt);

        std::memset(&patternOn, 0x00, sizeof(HidsysNotificationLedPattern));
        std::memset(&patternOff, 0x00, sizeof(HidsysNotificationLedPattern));

        patternOn.baseMiniCycleDuration = 0x0F;
        patternOn.startIntensity = 0x0F;
        patternOn.miniCycles[0].ledIntensity = 0x0F;
        patternOn.miniCycles[0].transitionSteps = 0x0F;
        patternOn.miniCycles[0].finalStepDuration = 0x0F;
    }

    void setLedState(bool state) {
        for(u8 i = 0; i < uniquePadCnt; i++)
            hidsysSetNotificationLedPattern(state ? &patternOn : &patternOff, uniquePadIds[i]);
    }

}