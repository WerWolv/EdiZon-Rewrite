#include "cheat/cheat.hpp"

#include <cstring>

namespace edz::cheat {

    /////// Cheat Type ///////

    Cheat::Cheat(dmntcht::DmntCheatEntry cheatEntry) : m_name(cheatEntry.definition.readable_name), m_id(cheatEntry.cheat_id) {

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
        dmntcht::DmntCheatEntry cheatEntry;

        if (dmntcht::getCheatById(&cheatEntry, getID()).failed())
            return false;

        return cheatEntry.enabled;
    }


    /////// FrozenAddress Type ///////

    FrozenAddress::FrozenAddress(dmntcht::DmntFrozenAddressEntry frozenAddressEntry) : m_frozenAddressEntry(frozenAddressEntry), m_frozen(true) {

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

        // Get process metadata
        if (dmntcht::getCheatProcessMetadata(&this->m_processMetadata).failed())
            return;


        // Get all loaded cheats
        u64 cheatCnt = 0;
        if (dmntcht::getCheatCount(&cheatCnt).failed())
            return;

        dmntcht::DmntCheatEntry cheatEntries[cheatCnt];

        if (dmntcht::getCheats(cheatEntries, cheatCnt, 0, &cheatCnt).failed())
            return;
        
        this->m_cheats.reserve(cheatCnt);
        for (auto &cheatEntry : cheatEntries)
            this->m_cheats.push_back(new Cheat(cheatEntry));


        // Get all frozen addresses
        u64 frozenAddressCnt = 0;
        if (dmntcht::getFrozenAddressCount(&frozenAddressCnt).failed())
            return;

        dmntcht::DmntFrozenAddressEntry frozenAddressEntries[frozenAddressCnt];
        
        if (dmntcht::getFrozenAddresses(frozenAddressEntries, frozenAddressCnt, 0, &frozenAddressCnt).failed())
            return;

        this->m_frozenAddresses.reserve(frozenAddressCnt);
        for (auto &frozenAddressEntry : frozenAddressEntries)
            this->m_frozenAddresses.push_back(new FrozenAddress(frozenAddressEntry));

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
        return *reinterpret_cast<buildid_t*>(this->m_processMetadata.main_nso_build_id);
    }


    std::vector<Cheat*>& CheatManager::getCheats() {
        return this->m_cheats;
    }

    std::vector<FrozenAddress*>& CheatManager::getFrozenAddresses() {
        return this->m_frozenAddresses;
    }


    MemoryInfo queryMemory(addr_t address) {
        MemoryInfo memInfo = { 0 };

        dmntcht::queryCheatProcessMemory(&memInfo, address);

        return memInfo;
    }

    std::vector<MemoryInfo> getMemoryRegions() {
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

}