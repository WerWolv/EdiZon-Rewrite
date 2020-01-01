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

#include "helpers/config_manager.hpp"

#endif

#include "helpers/file.hpp"
#include "helpers/folder.hpp"

#define MHz *1E6

namespace edz::hlp {

    static HidsysNotificationLedPattern patternOn, patternOff;
    static u64 uniquePadIds[0x10];
    static s32 uniquePadCnt;

#ifndef __SYSMODULE__

    bool isPctlEnabled() {
        bool enabled = false;

        if (EResult(pctlIsRestrictionEnabled(&enabled)).failed())
            return false;

        return enabled;
    }

    bool openPctlPrompt(std::function<void()> f, bool askAgain) {
        static bool alreadyAnswered = false;

        if (askAgain)
            alreadyAnswered = false;

        if (isPctlEnabled() && !alreadyAnswered && GET_CONFIG(Settings.pctlChecksEnabled))
            if (EResult(pctlauthShow(false)).failed())
                return false;
        
        f();

        alreadyAnswered = true;
        
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

    bool openPlayerSelect(std::function<void(std::unique_ptr<save::Account>&)> f) {

        // If there's only one Account present on the system, just use it without asking
        if (save::SaveFileSystem::getAllAccounts().size() == 1) {
            f(save::SaveFileSystem::getAllAccounts().begin()->second);
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

        if (outdata.result == 0) {
            f(save::SaveFileSystem::getAllAccounts()[outdata.userID]);
            return true;
        }

        return false;
    }

#endif


    bool isTitleRunning() {
        u64 runningTitlePid = 0, edizonPid = 0;

        if (EResult(pmdmntGetApplicationProcessId(&runningTitlePid)).failed())
            return false;

        if (EResult(svcGetProcessId(&edizonPid, CUR_PROCESS_HANDLE)).failed())
            return false;

        return runningTitlePid > 0 && runningTitlePid != edizonPid;
    }

#ifndef __SYSMODULE__

    bool isInApplicationMode() {
        u64 runningTitlePid = 0, edizonPid = 0;

        if (EResult(pmdmntGetApplicationProcessId(&runningTitlePid)).failed())
            return false;

        if (EResult(svcGetProcessId(&edizonPid, CUR_PROCESS_HANDLE)).failed())
            return false;

        return runningTitlePid == edizonPid;
    }

    bool isInAppletMode() {
        return !isInApplicationMode();
    }

#endif

    bool isProgramRunning(titleid_t titleID) {
        u64 pid = 0;
        if (EResult(pmdmntGetProcessId(&pid, titleID)).failed())
            return false;
        
        return pid != 0;
    }

    bool isServiceRunning(const char *serviceName) {
        Handle handle;
        bool running = EResult(smRegisterService(&handle, smEncodeName(serviceName), false, 1));

        svcCloseHandle(handle);

        if (!running)
            smUnregisterService(smEncodeName(serviceName));

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


    std::string getLFSContentsPath() {
        if (isOnAMS())
            return "sdmc:/atmosphere/contents";
        
        if (isOnRNX())
            return "sdmc:/ReiNX/contents";

        if (isOnSXOS())
            return "sdmc:/sxos/contents";

        return "";
    }

    std::string getLFSCheatsPath(titleid_t titleID) {
        return getLFSContentsPath() + "/" + hlp::toHexString(titleID) + "/cheats";
    }
    

    std::string getCurrentDateTimeString() {
        char buffer[0x20];

        time_t unixTime = time(nullptr);
        struct tm* time = localtime((const time_t*)&unixTime);

        sprintf(buffer, "%0d%0d%0d_%02d%02d%02d", time->tm_year % 100, time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

        return std::string(buffer);
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
            hidsysSetNotificationLedPatternWithTimeout(state ? &patternOn : &patternOff, uniquePadIds[i], 10E9);
    }


#ifndef __SYSMODULE__

    void enableAutostartOfBackgroundService() {
        // Create boot2.flag file to let the sysmodule get started on boot
        File file(getLFSContentsPath() + "/01000000000ED150/flags/boot2.flag"); 
        file.createDirectories();
        file.write(nullptr, 0);
    }

    void disableAutostartOfBackgroundService() {
        File(getLFSContentsPath() + "/01000000000ED150/flags/boot2.flag").remove();
    }

    EResult startBackgroundService() {
        EResult res = ResultSuccess;
        u64 pid = 0;

        NcmProgramLocation edizonLocation = { EDIZON_SYSMODULE_TITLEID, NcmStorageId_None };

        pmdmntGetProcessId(&pid, EDIZON_SYSMODULE_TITLEID);
        if (pid != 0) return ResultEdzSysmoduleAlreadyRunning;
        
        pid = 0;
        if (EResult(pmshellLaunchProgram(0, &edizonLocation, &pid)).failed())
            res = ResultEdzSysmoduleLaunchFailed;

        return res;
    }

    EResult stopBackgroundService() {
        u64 pid = 0;

        pmdmntGetProcessId(&pid, EDIZON_SYSMODULE_TITLEID);
        if (pid == 0) return ResultEdzSysmoduleNotRunning;

        if (EResult(pmshellTerminateProcess(pid)).failed())
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

    void unwindStack(u64 *outStackTrace, s32 *outStackTraceSize, size_t maxStackTraceSize, u64 currFp) {
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

    std::string removeInvalidCharacters(std::string in) {
        std::string out;

        std::copy_if(in.begin(), in.end(), std::back_inserter(out), [](char c) {
            if (c < ' ' || c > '~')
                return false;
            else if (c == '/' || c == '\\' || c == ':')
                return false;
            else return true;
        });

        return out;
    }

    userid_t accountUidToUserID(AccountUid accountUID) {
        return *reinterpret_cast<userid_t*>(accountUID.uid);
    }

    AccountUid userIDToAccountUid(userid_t userID) {
        AccountUid ret = { 0 };

        std::memcpy(ret.uid, &userID, sizeof(userid_t));
        return ret;
    }

    Version getAtmosphereVersion() {
        u64 config = 0x00;

        // Since Gateway copies the entirety of Exosphere in SXOS, check to make sure the
        // version we get actually says something about the features the CFW supports
        if (!isOnAMS())
            return { 0, 0, 0 };

        // 65000 is an unofficial config item from exosphere containing the current Atmosphere and exosphere version and the mkey revision
        if (EResult(splGetConfig(static_cast<SplConfigItem>(65000), &config)).failed())
            return { 0, 0, 0 };

        return { static_cast<u8>((config >> 0x20) & 0xFF), static_cast<u8>((config >> 0x18) & 0xFF), static_cast<u8>((config >> 0x10) & 0xFF) };
    }

}