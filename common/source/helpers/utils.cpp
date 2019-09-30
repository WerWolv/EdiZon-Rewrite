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

#ifndef __SYSMODULE__

#include "save/title.hpp"
#include "save/account.hpp"
#include "save/save_data.hpp"

#endif

#include "helpers/file.hpp"
#include "helpers/folder.hpp"

#define MHz *1E6

namespace edz::hlp {

    static HidsysNotificationLedPattern patternOn, patternOff;
    static u64 uniquePadIds[0x10];
    static size_t uniquePadCnt;

#ifndef __SYSMODULE__

    bool openPctlPrompt(std::function<void()> f) {
        bool restricted = false;
        pctlIsRestrictionEnabled(&restricted);

        if (restricted)
            if (EResult(pctlauthShow(false)).failed())
                return false;
        
        f();

        return true;
    }

    bool openSwkbdForText(std::function<void(std::string)> f, std::string headerText, std::string subText, u8 maxStringLength, std::string initialText) {
        SwkbdConfig config;

        swkbdCreate(&config, 0);

        swkbdConfigMakePresetDefault(&config);
        swkbdConfigSetHeaderText(&config, headerText.c_str());
        swkbdConfigSetSubText(&config, subText.c_str());
        swkbdConfigSetInitialText(&config, initialText.c_str());
        swkbdConfigSetBlurBackground(&config, true);
        swkbdConfigSetType(&config, SwkbdType_QWERTY);
        swkbdConfigSetStringLenMax(&config, maxStringLength);
        swkbdConfigSetStringLenMaxExt(&config, 1);
        swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

        char buffer[0x100] = { 0 };

        if (EResult(swkbdShow(&config, buffer, 0x100)).succeeded() && std::strcmp(buffer, "") != 0) {
            swkbdClose(&config);
            f(std::string(buffer));

            return true;
        }

        swkbdClose(&config);

        return false;
    }

    bool openSwkbdForPassword(std::function<void(std::string)> f, std::string headerText, std::string subText, u8 maxStringLength, std::string initialText) {
        SwkbdConfig config;

        swkbdCreate(&config, 0);

        swkbdConfigMakePresetPassword(&config);
        swkbdConfigSetHeaderText(&config, headerText.c_str());
        swkbdConfigSetSubText(&config, subText.c_str());
        swkbdConfigSetInitialText(&config, initialText.c_str());
        swkbdConfigSetBlurBackground(&config, true);
        swkbdConfigSetType(&config, SwkbdType_Normal);
        swkbdConfigSetStringLenMax(&config, maxStringLength);
        swkbdConfigSetStringLenMaxExt(&config, 1);

        char buffer[0x100];

        if (EResult(swkbdShow(&config, buffer, 0x100)).succeeded() && std::strcmp(buffer, "") != 0) {
            swkbdClose(&config);
            f(buffer);

            return true;
        }

        swkbdClose(&config);

        return false;
    }

    bool openSwkbdForNumber(std::function<void(std::string)> f, std::string headerText, std::string subText, std::string leftButton, std::string rightButton, u8 maxStringLength, std::string initialText) {
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
            swkbdClose(&config);
            f(buffer);

            return true;
        }

        swkbdClose(&config);

        return false;
    }

    bool openPlayerSelect(std::function<void(save::Account*)> f) {

        // If there's only one Account present on the system, just use it without asking
        if (save::SaveFileSystem::getAllAccounts().size() == 1) {
            f(save::SaveFileSystem::getAllAccounts().begin()->second.get());
            return true;
        }

        struct UserReturnData{
            u64 result;
            u128 userID;
        } PACKED;

        struct UserReturnData outdata;

        AppletHolder aph;
        AppletStorage ast;
        AppletStorage hast1;
        LibAppletArgs args;

        u8 indata[0xA0] = { 0 };
        indata[0x96] = 1;

        appletCreateLibraryApplet(&aph, AppletId_playerSelect, LibAppletMode_AllForeground);
        libappletArgsCreate(&args, 0);
        libappletArgsPush(&args, &aph);

        appletCreateStorage(&hast1, 0xA0);

        appletStorageWrite(&hast1, 0, indata, 0xA0);
        appletHolderPushInData(&aph, &hast1);
        appletHolderStart(&aph);

        while(appletHolderWaitInteractiveOut(&aph));        

        appletHolderJoin(&aph);
        appletHolderPopOutData(&aph, &ast);
        appletStorageRead(&ast, 0, &outdata, 24);

        appletHolderClose(&aph);
        appletStorageClose(&ast);
        appletStorageClose(&hast1);

        if (outdata.userID != 0) {
            f(save::SaveFileSystem::getAllAccounts()[outdata.userID].get());
            return true;
        }

        return false;
    }

#endif


    bool isTitleRunning() {
        u64 runningTitlePid = 0, edizonPid = 0;

        if (EResult(pmdmntGetApplicationPid(&runningTitlePid)).failed())
            return false;

        if (EResult(svcGetProcessId(&edizonPid, CUR_PROCESS_HANDLE)).failed())
            return false;

        return runningTitlePid > 0 && runningTitlePid != edizonPid;
    }

#ifndef __SYSMODULE__

    bool isInApplicationMode() {
        u64 runningTitlePid = 0, edizonPid = 0;

        if (EResult(pmdmntGetApplicationPid(&runningTitlePid)).failed())
            return false;

        if (EResult(svcGetProcessId(&edizonPid, CUR_PROCESS_HANDLE)).failed())
            return false;

        return runningTitlePid == edizonPid;
    }

    bool isInAppletMode() {
        return !isInApplicationMode();
    }

#endif

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
            return "sdmc:/atmosphere/titles";
        
        if (isOnRNX())
            return "sdmc:/ReiNX/titles";

        if (isOnSXOS())
            return "sdmc:/sxos/titles";

        return "";
    }

    std::string getLFSCheatsPath(titleid_t titleID) {
        return getLFSTitlesPath() + "/" + hlp::toHexString(titleID) + "/cheats";
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

    std::string limitStringLength(std::string string, size_t maxLength) {
        if (string.length() <= maxLength)
            return string;

        char modString[string.length() + 1];
        std::strcpy(modString, string.c_str());

        modString[maxLength - 3] = '.'; 
        modString[maxLength - 2] = '.';
        modString[maxLength - 1] = '.';
        modString[maxLength - 0] = '\0';

        return std::string(modString);
    }

    std::string removeIllegalPathCharacters(std::string path) {
        const static char illegalCharacters[] = ".,!\\/:?*\"<>|";

        for (u16 i = 0; i < strlen(illegalCharacters); i++)
            path.erase(std::remove(path.begin(), path.end(), illegalCharacters[i]), path.end());

        return path;
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
        ER_TRY(hidsysGetUniquePadsFromNpad(hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1, uniquePadIds, 2, &uniquePadCnt));

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


#ifndef __SYSMODULE__

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
            File(getLFSTitlesPath() + "/0100000000ED1204/exefs.nsp").remove();
            return ResultEdzSysmoduleLaunchFailed;
        }

        if (!startOnBoot)
            File(getLFSTitlesPath() + "/0100000000ED1204/exefs.nsp").remove();
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

            sysmoduleFile.remove();
            flagFile.remove();
        }

        pmdmntGetTitlePid(&pid, _edizonBackgroundServiceTitleId);
        if (pid == 0) return ResultEdzSysmoduleNotRunning;

        if (EResult(pmshellTerminateProcessByProcessId(pid)).failed())
            return ResultEdzSysmoduleTerminationFailed;
        
        return ResultSuccess;
    }

#endif

    Folder createTmpFolder() {
        static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

        while (true) {
            char randomName[17];

            for (u8 i = 0; i < sizeof(randomName); i++)
                randomName[i] = charset[rand() % (sizeof(charset) - 1)];
            randomName[sizeof(randomName) - 1] = '\0';

            Folder tmpFolder(EDIZON_BASE_DIR "/tmp/" + std::string(randomName));

            if (!tmpFolder.exists()) {
                tmpFolder.createDirectories();
                return tmpFolder;
            }
        }

        Folder tmpFolder(EDIZON_BASE_DIR "/tmp/0");
        tmpFolder.createDirectories();

        return tmpFolder;
    }

    void clearTmpFolder() {
        Folder tmpFolder(EDIZON_BASE_DIR "/tmp");

        tmpFolder.remove();
        tmpFolder.createDirectories();
    }

    u64 getHomebrewBaseAddress() {
        u32 p;
        MemoryInfo info;

        // Find the memory region in which the function getHomebrewBaseAddress() is stored.
        // The start of it will be the base address the homebrew was mapped to.
        svcQueryMemory(&info, &p, (u64) &getHomebrewBaseAddress);

        return info.addr;
    }

    void unwindStack(u64 *outStackTrace, size_t *outStackTraceSize, size_t maxStackTraceSize, u64 currFp) {
        struct StackFrame{
            u64 fp;     // Frame Pointer (Pointer to previous stack frame)
            u64 lr;     // Link Register (Return address)
        };

        for (size_t i = 0; i < maxStackTraceSize; i++) {
            if (currFp == 0 || currFp % sizeof(u64) != 0)
                break;
            
            auto currTrace = reinterpret_cast<StackFrame*>(currFp); 
            outStackTrace[(*outStackTraceSize)++] = currTrace->lr;
            currFp = currTrace->fp;
        }
    }

}