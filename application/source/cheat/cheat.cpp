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
        dmntcht::toggleCheat(getID());

        return isEnabled();
    }

    bool Cheat::isEnabled() {
        dmntcht::CheatEntry cheatEntry;

        if (dmntcht::getCheatById(&cheatEntry, getID()).failed())
            return false;

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

    CheatManager::CheatManager() {
        dmntcht::initialize();

        CheatManager::reload();
    }   

    CheatManager::~CheatManager() {
        for (auto &cheat : this->m_cheats)
            delete cheat;
        this->m_cheats.clear();

        for (auto &frozenAddress : this->m_frozenAddresses)
            delete frozenAddress;
        this->m_frozenAddresses.clear();

        dmntcht::exit();
    }


    bool CheatManager::isCheatServiceAvailable() {
        return hlp::isServiceRunning("dmnt:cht");
    }

    bool CheatManager::forceAttach() {
        return dmntcht::forceOpenCheatProcess().succeeded();
    }

    bool CheatManager::hasCheatProcess() {
        bool hasCheatProcess;

        dmntcht::hasCheatProcess(&hasCheatProcess);

        return hasCheatProcess;
    }


    titleid_t CheatManager::getTitleID() {
        return this->m_processMetadata.title_id;
    }

    processid_t CheatManager::getProcessID() {
        return this->m_processMetadata.process_id;
    }

    buildid_t CheatManager::getBuildID() {
        buildid_t buildid = 0;

        std::memcpy(&buildid, this->m_processMetadata.main_nso_build_id, sizeof(buildid_t));
        return buildid;
    }


    std::pair<EResult, u32> CheatManager::addCheat(dmntcht::CheatDefinition cheatDefinition, bool enabled) {
        u32 cheatID = 0;
        EResult res;
        
        if ((res = dmntcht::addCheat(&cheatDefinition, enabled, &cheatID)).failed())
            return { res, 0 };

        if ((res = CheatManager::reload()).failed())
            return { res, 0 };

        return { res, cheatID };
    }

    EResult CheatManager::removeCheat(u32 cheatID) {
        EResult res;

        if ((res = dmntcht::removeCheat(cheatID)).failed())
            return res;

        return CheatManager::reload();
    }


    std::vector<Cheat*>& CheatManager::getCheats() {
        return this->m_cheats;
    }

    std::vector<FrozenAddress*>& CheatManager::getFrozenAddresses() {
        return this->m_frozenAddresses;
    }


    MemoryInfo CheatManager::queryMemory(addr_t address) {
        MemoryInfo memInfo = { 0 };

        dmntcht::queryCheatProcessMemory(&memInfo, address);

        return memInfo;
    }

    std::vector<MemoryInfo> CheatManager::getMemoryRegions() {
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


    EResult CheatManager::readMemory(addr_t address, u8 *buffer, size_t bufferSize) {
        return dmntcht::readCheatProcessMemory(address, buffer, bufferSize);
    }

    EResult CheatManager::writeMemory(addr_t address, const u8 *buffer, size_t bufferSize) {
        return dmntcht::writeCheatProcessMemory(address, buffer, bufferSize);
    }


    EResult CheatManager::reload() {
        EResult res;

        // Delete local cheats copy if there are any
        for (auto &cheat : this->m_cheats)
            delete cheat;
        this->m_cheats.clear();

        // Delete local frozen addresses copy if there are any
        for (auto &frozenAddress : this->m_frozenAddresses)
            delete frozenAddress;
        this->m_frozenAddresses.clear();

        // Get process metadata
        if ((res = dmntcht::getCheatProcessMetadata(&this->m_processMetadata)).failed())
            return res;


        // Get all loaded cheats
        u64 cheatCnt = 0;
        if ((res = dmntcht::getCheatCount(&cheatCnt)).failed())
            return res;

        dmntcht::CheatEntry cheatEntries[cheatCnt];

        if ((res = dmntcht::getCheats(cheatEntries, cheatCnt, 0, &cheatCnt)).failed())
            return res;
        
        this->m_cheats.reserve(cheatCnt);
        for (auto &cheatEntry : cheatEntries)
            this->m_cheats.push_back(new Cheat(cheatEntry));


        // Get all frozen addresses
        u64 frozenAddressCnt = 0;
        if ((res = dmntcht::getFrozenAddressCount(&frozenAddressCnt)).failed())
            return res;

        dmntcht::FrozenAddressEntry frozenAddressEntries[frozenAddressCnt];
        
        if ((res = dmntcht::getFrozenAddresses(frozenAddressEntries, frozenAddressCnt, 0, &frozenAddressCnt)).failed())
            return res;

        this->m_frozenAddresses.reserve(frozenAddressCnt);
        for (auto &frozenAddressEntry : frozenAddressEntries)
            this->m_frozenAddresses.push_back(new FrozenAddress(frozenAddressEntry));

        return res;
    }

}