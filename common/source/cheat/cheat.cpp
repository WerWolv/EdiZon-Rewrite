/**
 * Copyright (C) 2019 - 2020 WerWolv
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

#include "cheat/cheat.hpp"
#include "helpers/utils.hpp"

#include <cstring>

namespace edz::cheat {

    /////// Cheat Type ///////

    Cheat::Cheat(dmntcht::CheatEntry cheatEntry) : m_name(cheatEntry.definition.readable_name), m_id(cheatEntry.cheat_id) {

    }

    std::string Cheat::getName() {
        return this->m_name;
    }

    u32 Cheat::getID() {
        return this->m_id;
    }

    bool Cheat::toggle() {
        dmntcht::toggleCheat(getID()).getString().c_str();

        return isEnabled();
    }

    bool Cheat::setState(bool state) {
        bool ret = state;
        if (isEnabled() != state)
            ret = this->toggle();
        
        return ret;
    }

    bool Cheat::isEnabled() {
        dmntcht::CheatEntry cheatEntry;

        if (EResult res = dmntcht::getCheatById(&cheatEntry, getID()); res.failed()) {
            return false;
        }


        return cheatEntry.enabled;
    }


    /////// FrozenAddress Type ///////

    FrozenAddress::FrozenAddress(dmntcht::FrozenAddressEntry frozenAddressEntry) : m_frozenAddressEntry(frozenAddressEntry), m_frozen(true) {

    }

    FrozenAddress::FrozenAddress(addr_t address, u8 width, u64 value) : m_frozen(false) {
        this->m_frozenAddressEntry.address        = address;
        this->m_frozenAddressEntry.value.width    = width;
        this->m_frozenAddressEntry.value.value    = value;
    }

    FrozenAddress::FrozenAddress(addr_t address, u8 width) : m_frozen(false) {
        this->m_frozenAddressEntry.address        = address;
        this->m_frozenAddressEntry.value.width    = width;
    }


    addr_t FrozenAddress::getAddress() {
        return this->m_frozenAddressEntry.address;
    }

    u8 FrozenAddress::getWidth() {
        return this->m_frozenAddressEntry.value.width;
    }

    u64 FrozenAddress::getValue() {
        return this->m_frozenAddressEntry.value.value;
    }

    u64 FrozenAddress::setValue(u64 value, u8 width) {
        bool wasFrozen = isFrozen();
        u64 newValue = 0;

        dmntcht::disableFrozenAddress(getAddress());

        dmntcht::writeCheatProcessMemory(getAddress(), &value, width);

        if (wasFrozen) {
            if (dmntcht::enableFrozenAddress(getAddress(), getWidth(), &newValue).succeeded())
                return -1;
        }

        // Check if the value was set correctly
        if (std::memcmp(&value, &newValue, width) == 0) {
            this->m_frozenAddressEntry.value.value = newValue;
            this->m_frozenAddressEntry.value.width = width;
                    
            return newValue;
        }

        return -1;
    }


    bool FrozenAddress::toggle() {
        if (isFrozen()) {
            if (dmntcht::disableFrozenAddress(getAddress()).succeeded())
                this->m_frozen = false;
        }
        else {
            if (dmntcht::enableFrozenAddress(getAddress(), getWidth(), &m_frozenAddressEntry.value.value).succeeded())
                this->m_frozen = true;
        }

        return isFrozen();
    }

    bool FrozenAddress::isFrozen() {
        return this->m_frozen;
    }


    /////// FrozenAddress Type ///////

    EResult CheatManager::initialize() {
        return CheatManager::reload();
    }   

    void CheatManager::exit() {
        for (auto &cheat : CheatManager::s_cheats)
            delete cheat;
        CheatManager::s_cheats.clear();

        for (auto &frozenAddress : CheatManager::s_frozenAddresses)
            delete frozenAddress;
        CheatManager::s_frozenAddresses.clear();
    }


    bool CheatManager::isCheatServiceAvailable() {
        static s8 running = -1;
        if (running == -1) 
            running = hlp::isServiceRunning("dmnt:cht");

        return !!running; 
    }

    bool CheatManager::forceAttach() {
        if (!CheatManager::isCheatServiceAvailable())
            return ResultEdzCheatServiceNotAvailable;

        return dmntcht::forceOpenCheatProcess().succeeded();
    }

    bool CheatManager::hasCheatProcess() {
        if (!CheatManager::isCheatServiceAvailable())
            return ResultEdzCheatServiceNotAvailable;

        bool hasCheatProcess;

        dmntcht::hasCheatProcess(&hasCheatProcess);

        return hasCheatProcess;
    }


    titleid_t CheatManager::getTitleID() {
        return CheatManager::s_processMetadata.title_id;
    }

    processid_t CheatManager::getProcessID() {
        return CheatManager::s_processMetadata.process_id;
    }

    buildid_t CheatManager::getBuildID() {
        buildid_t buildid = 0;

        std::memcpy(&buildid, CheatManager::s_processMetadata.main_nso_build_id, sizeof(buildid_t));
        
        return __builtin_bswap64(buildid);;
    }


    types::Region CheatManager::getBaseRegion() {
        if (!CheatManager::isCheatServiceAvailable())
            return { };

        return types::Region{ CheatManager::s_processMetadata.address_space_extents.base, CheatManager::s_processMetadata.address_space_extents.size };
    }

    types::Region CheatManager::getHeapRegion() {
        if (!CheatManager::isCheatServiceAvailable())
            return { };

        return types::Region{ CheatManager::s_processMetadata.heap_extents.base, CheatManager::s_processMetadata.heap_extents.size };
    }

    types::Region CheatManager::getMainRegion() {
        if (!CheatManager::isCheatServiceAvailable())
            return { };

        return types::Region{ CheatManager::s_processMetadata.main_nso_extents.base, CheatManager::s_processMetadata.main_nso_extents.size };
    }

    types::Region CheatManager::getAliasRegion() {
        if (!CheatManager::isCheatServiceAvailable())
            return { };

        return types::Region{ CheatManager::s_processMetadata.alias_extents.base, CheatManager::s_processMetadata.alias_extents.size };
    }


    EResultVal<std::string> CheatManager::getCheatFile() {
        if (!CheatManager::isCheatServiceAvailable())
            return { ResultEdzCheatServiceNotAvailable, "" };
        
        std::string expectedFileName = hlp::toHexString(CheatManager::getBuildID()) + ".txt";

        for (auto &[fileName, file] : hlp::Folder(EDIZON_CHEATS_DIR).getFiles()) {
            if (strcasecmp(expectedFileName.c_str(), fileName.c_str()) == 0)
                return { ResultSuccess, fileName };
        }
            
        return { ResultEdzNotFound, "" }; 
    }


    EResultVal<u32> CheatManager::addCheat(dmntcht::CheatDefinition cheatDefinition, bool enabled) {
        if (!CheatManager::isCheatServiceAvailable())
            return { ResultEdzCheatServiceNotAvailable, 0 };

        u32 cheatID = 0;
        EResult res;
        
        if ((res = dmntcht::addCheat(&cheatDefinition, enabled, &cheatID)).failed())
            return { res, 0 };

        if ((res = CheatManager::reload()).failed())
            return { res, 0 };

        return { res, cheatID };
    }

    EResult CheatManager::removeCheat(u32 cheatID) {
        if (!CheatManager::isCheatServiceAvailable())
            return ResultEdzCheatServiceNotAvailable;

        EResult res;

        if ((res = dmntcht::removeCheat(cheatID)).failed())
            return res;

        return CheatManager::reload();
    }


    std::vector<Cheat*>& CheatManager::getCheats() {
        return CheatManager::s_cheats;
    }

    std::vector<FrozenAddress*>& CheatManager::getFrozenAddresses() {
        return CheatManager::s_frozenAddresses;
    }


    MemoryInfo CheatManager::queryMemory(addr_t address) {
        if (!CheatManager::isCheatServiceAvailable())
            return { 0 };

        MemoryInfo memInfo = { 0 };

        dmntcht::queryCheatProcessMemory(&memInfo, address);

        return memInfo;
    }

    std::vector<MemoryInfo> CheatManager::getMemoryRegions() {
        if (!CheatManager::isCheatServiceAvailable())
            return EMPTY_RESPONSE;

        MemoryInfo memInfo = { 0 };
        std::vector<MemoryInfo> memInfos;

        addr_t lastAddress = 0;

        do {
            lastAddress = memInfo.addr;

            memInfo = queryMemory(memInfo.addr + memInfo.size);
            memInfos.push_back(memInfo);
        } while (lastAddress < (memInfo.addr + memInfo.size));

        return memInfos;
    }


    EResult CheatManager::readMemory(addr_t address, void *buffer, size_t bufferSize) {
        if (!CheatManager::isCheatServiceAvailable())
            return ResultEdzCheatServiceNotAvailable;

        return dmntcht::readCheatProcessMemory(address, buffer, bufferSize);
    }

    EResult CheatManager::writeMemory(addr_t address, const void *buffer, size_t bufferSize) {
        if (!CheatManager::isCheatServiceAvailable())
            return ResultEdzCheatServiceNotAvailable;

        return dmntcht::writeCheatProcessMemory(address, buffer, bufferSize);
    }


    EResult CheatManager::reload() {
        if (!CheatManager::isCheatServiceAvailable())
            return ResultEdzCheatServiceNotAvailable;

        EResult res;

        CheatManager::forceAttach();

        // Delete local cheats copy if there are any
        for (auto &cheat : CheatManager::s_cheats)
            delete cheat;
        CheatManager::s_cheats.clear();

        // Delete local frozen addresses copy if there are any
        for (auto &frozenAddress : CheatManager::s_frozenAddresses)
            delete frozenAddress;
        CheatManager::s_frozenAddresses.clear();

        // Get process metadata
        if ((res = dmntcht::getCheatProcessMetadata(&CheatManager::s_processMetadata)).failed())
            return res;


        // Get all loaded cheats
        u64 cheatCnt = 0;
        if ((res = dmntcht::getCheatCount(&cheatCnt)).failed())
            return res;

        dmntcht::CheatEntry *cheatEntries = new dmntcht::CheatEntry[cheatCnt];

        if ((res = dmntcht::getCheats(cheatEntries, cheatCnt, 0, &cheatCnt)).failed())
            return res;
        
        CheatManager::s_cheats.reserve(cheatCnt);
        for (u32 i = 0; i < cheatCnt; i++)
            CheatManager::s_cheats.push_back(new Cheat(cheatEntries[i]));


        // Get all frozen addresses
        u64 frozenAddressCnt = 0;
        if ((res = dmntcht::getFrozenAddressCount(&frozenAddressCnt)).failed())
            return res;

        dmntcht::FrozenAddressEntry frozenAddressEntries[frozenAddressCnt];
        
        if ((res = dmntcht::getFrozenAddresses(frozenAddressEntries, frozenAddressCnt, 0, &frozenAddressCnt)).failed())
            return res;

        CheatManager::s_frozenAddresses.reserve(frozenAddressCnt);
        for (auto &frozenAddressEntry : frozenAddressEntries)
            CheatManager::s_frozenAddresses.push_back(new FrozenAddress(frozenAddressEntry));

        return res;
    }

}