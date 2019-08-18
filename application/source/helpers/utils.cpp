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

#include "helpers/utils.hpp"

#include <string>
#include <ctime>
#include <cstring>

#include "save/title.hpp"
#include "helpers/file.hpp"

#define MHz *1E6

namespace edz::hlp {

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

    bool askSwkbdText(std::function<void(std::string)> f, std::string headerText, std::string subText, u8 maxStringLength, std::string initialText) {
        SwkbdConfig config;

        swkbdCreate(&config, 0);

        swkbdConfigMakePresetDefault(&config);
        swkbdConfigSetHeaderText(&config, headerText.c_str());
        swkbdConfigSetSubText(&config, subText.c_str());
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

    bool askSwkbdNumber(std::function<void(std::string)> f, std::string headerText, std::string subText, std::string leftButton, std::string rightButton, u8 maxStringLength, std::string initialText) {
        SwkbdConfig config;

        swkbdCreate(&config, 0);

        swkbdConfigMakePresetDefault(&config);
        swkbdConfigSetHeaderText(&config, headerText.c_str());
        swkbdConfigSetSubText(&config, subText.c_str());
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


    std::string getLFSTitlesPath() {
        if (isOnAMS())
            return "/atmosphere/titles";
        
        if (isOnRNX())
            return "/ReiNX/titles";

        if (isOnSXOS())
            return "/sxos/titles";
    }

    std::string getLFSCheatsPath(save::Title *title) {
        return getLFSTitlesPath() + "/" + title->getIDString() + "/cheats";
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

    EResult controllerLEDInitialize() {
        TRY(hidsysGetUniquePadsFromNpad(hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1, uniquePadIds, 2, &uniquePadCnt));

        std::memset(&patternOn, 0x00, sizeof(HidsysNotificationLedPattern));
        std::memset(&patternOff, 0x00, sizeof(HidsysNotificationLedPattern));

        patternOn.baseMiniCycleDuration = 0x0F;
        patternOn.startIntensity = 0x0F;
        patternOn.miniCycles[0].ledIntensity = 0x0F;
        patternOn.miniCycles[0].transitionSteps = 0x0F;
        patternOn.miniCycles[0].finalStepDuration = 0x0F;

        return ResultSuccess;
    }

    void setLedState(bool state) {
        for(u8 i = 0; i < uniquePadCnt; i++)
            hidsysSetNotificationLedPattern(state ? &patternOn : &patternOff, uniquePadIds[i]);
    }


    static constexpr titleid_t _edizonBackgroundServiceTitleId = 0x0100000000ED1204;

    EResult startBackgroundService(bool startOnBoot) {
        u64 pid = 0;

        pmdmntGetTitlePid(&pid, _edizonBackgroundServiceTitleId);
        if (pid != 0) return ResultEdzSysmoduleAlreadyRunning;

        {
            File romfsSysmoduleFile("romfs:/sysmodule/exefs.nsp");
            romfsSysmoduleFile.copyTo(getLFSTitlesPath() + "/0100000000ED1204/exefs.nsp");
        }

        if (EResult(pmshellLaunchProcess(0, _edizonBackgroundServiceTitleId, FsStorageId_None, &pid)).failed()) {
            File(getLFSTitlesPath() + "/0100000000ED1204/exefs.nsp").removeFile();
            return ResultEdzSysmoduleLaunchFailed;
        }

        if (!startOnBoot)
            File(getLFSTitlesPath() + "/0100000000ED1204/exefs.nsp").removeFile();
        else {
            // Create boot2.flag file to let the sysmodule get started on boot
            File file(getLFSTitlesPath() + "/0100000000ED1204/flags/boot2.flag"); 
            file.createDirectories();
            file.write(nullptr, 0);
        }

        return ResultSuccess;
    }

    EResult stopBackgroundService(bool startOnBoot) {
        u64 pid = 0;

        if (!startOnBoot) {
            File sysmoduleFile(getLFSTitlesPath() + "/0100000000ED1204/exefs.nsp");
            File flagFile(getLFSTitlesPath() + "/0100000000ED1204/flags/boot2.flag");

            sysmoduleFile.removeFile();
            flagFile.removeFile();
        }

        pmdmntGetTitlePid(&pid, _edizonBackgroundServiceTitleId);
        if (pid == 0) return ResultEdzSysmoduleNotRunning;

        if (EResult(pmshellTerminateProcessByProcessId(pid)).failed())
            return ResultEdzSysmoduleTerminationFailed;
        
        return ResultSuccess;
    }

}