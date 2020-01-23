/**
 * Copyright (C) 2020 WerWolv
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

#include <edizon.hpp>

#include <string>
#include <functional>
#include <sstream>
#include <iomanip>
#include <cstring>

#include "helpers/folder.hpp"
#include "helpers/version.hpp"

namespace edz::save {
    class Title;
    class Account;
}

namespace edz::hlp {

#ifndef __SYSMODULE__
    /* Has Parential Control been enabled */
    bool isPctlEnabled();

    /* Opens the Parential Control applet and calls the callback function when the PIN was entered correctly */ 
    bool openPctlPrompt(std::function<void()> f, bool askAgain = false);

    /* Opens the Software Keyboard for text input */
    bool openSwkbdForText(std::function<void(std::string)> f, std::string headerText = "", std::string subText = "", u8 maxStringLength = 32, std::string initialText = "");
    
    /* Opens the Software Keyboard for password input */
    bool openSwkbdForPassword(std::function<void(std::string)> f, std::string headerText = "", std::string subText = "", u8 maxStringLength = 32, std::string initialText = "");
    
    /* Opens the Software Keyboard for number input */
    bool openSwkbdForNumber(std::function<void(std::string)> f, std::string headerText = "", std::string subText = "", std::string leftButton = "", std::string rightButton = "", u8 maxStringLength = 32, std::string initialText = "");
    
    /* Opens the Player Select applet to select a user */
    bool openPlayerSelect(std::function<void(std::unique_ptr<save::Account>&)> f);

    /* Returns if EdiZon was ran through Applet override */
    bool isInAppletMode();

    /* Returns if EdiZon was ran through Application override */
    bool isInApplicationMode();

#endif

    /* Returns if any application running */
    bool isTitleRunning();

    /* Returns if a sysmodule running that has a specific service registered */
    bool isServiceRunning(const char *serviceName);

    /* Returns if a process with a specific titleid is running */
    bool isProgramRunning(titleid_t titleID);

    /* Returns if the user is running Atmosphere */
    bool isOnAMS();

    /* Returns if the user is running ReiNX */
    bool isOnRNX();

    /* Returns if the user is running SXOS :ew: */
    bool isOnSXOS();


    /* Returns the /<CFW>/contents path based on what CFW the user is running*/
    std::string getLFSContentsPath();
    
    /* Returns the /<CFW>/contents/<titleid>/cheats path based on what CFW the user is running and the title provided */
    std::string getLFSCheatsPath(titleid_t titleID);
    
    /* C++ style sprintf */
    template <typename ...Args>
    std::string formatString(const std::string& format, Args && ...args) {
        auto size = std::snprintf(nullptr, 0, format.c_str(), args...);
        char output[size + 1];
        std::memset(output, ' ', sizeof(output));

        std::sprintf(output, format.c_str(), args...);

        return output;
    }

    /* Returns the hex string representation of a provided number */
    template<typename T>
    std::string toHexString(T value, size_t width = sizeof(T) * 2) {
        std::stringstream sstream;
        sstream << std::uppercase << std::setfill('0') << std::setw(width) << std::hex << value;

        return sstream.str();
    }

    /* Returns the truncated string that was passed in. Truncated strings end with ... */
    std::string limitStringLength(std::string string, size_t maxLength);

    /* Removes all illegal characters from a path */
    std::string removeIllegalPathCharacters(std::string path);

    /* Returns the current date time as a string suitable for file names */
    std::string getCurrentDateTimeString();

    /* Returns the current time as a string */
    std::string getCurrentTimeString();

    /* Returns the current battery percentage as a string*/
    u8 getCurrentBatteryPercentage();


    /* Overclocks the CPU and Memory to the maximum */
    void overclockSystem(bool enable);


    /* Sets up the controller LED patterns */
    EResult controllerLEDInitialize();

    /* Turns the controller LED on or off*/
    void setLedState(bool state);
    
    /* Creates a temporary folder in /switch/EdiZon/tmp and returns it */
    Folder createTmpFolder();

    /* Clears the /switch/EdiZon/tmp folder */
    void clearTmpFolder();


    /* Returns the base address hbloader mapped this homebrew to */
    u64 getHomebrewBaseAddress();

    /* Parses the Stack starting at the current Frame Pointer and returns a stack trace */
    void unwindStack(u64 *outStackTrace, s32 *outStackTraceSize, size_t maxStackTraceSize, u64 currFp);

    /* Erases invalid file name characters from string */
    std::string removeInvalidCharacters(std::string in);

    /* Converts a AccountUid to a userid_t */
    userid_t accountUidToUserID(AccountUid accountUID);

    /* Converts a userid_t to a AccountUid */
    AccountUid userIDToAccountUid(userid_t userID);

    /* Get the current Atmosphere version (or 0.0.0 if we're not on Atmosphere) */
    Version getAtmosphereVersion();

    /* Requests focus to the Tesla overlay */
    EResult focusOverlay(bool focus);

}