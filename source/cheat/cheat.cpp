#include "cheat/cheat.hpp"

#include <cstring>

namespace edz::cheat {

    /////// Cheat Type ///////

    Cheat::Cheat(dmntcht::DmntCheatEntry cheatEntry) : m_name(cheatEntry.definition.readable_name), m_id(cheatEntry.cheat_id) {

    }

    std::string Cheat::getName() {
        return m_name;
    }

    u32 Cheat::getID() {
        return m_id;
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

    FrozenAddress::FrozenAddress(u64 address, u8 width, u64 value) : m_frozen(false) {
        m_frozenAddressEntry.address        = address;
        m_frozenAddressEntry.value.width    = width;
        m_frozenAddressEntry.value.value    = value;
    }

    FrozenAddress::FrozenAddress(u64 address, u8 width) : m_frozen(false) {
        m_frozenAddressEntry.address        = address;
        m_frozenAddressEntry.value.width    = width;
    }


    u64 FrozenAddress::getAddress() {
        return m_frozenAddressEntry.address;
    }

    u8 FrozenAddress::getWidth() {
        return m_frozenAddressEntry.value.width;
    }

    u64 FrozenAddress::getValue() {
        return m_frozenAddressEntry.value.value;
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
            m_frozenAddressEntry.value.value = newValue;
            m_frozenAddressEntry.value.width = width;
                    
            return newValue;
        }

        return -1;
    }


    bool FrozenAddress::toggle() {
        if (isFrozen()) {
            if (dmntcht::disableFrozenAddress(getAddress()).succeeded())
                m_frozen = false;
        }
        else {
            if (dmntcht::enableFrozenAddress(getAddress(), getWidth(), &m_frozenAddressEntry.value.value).succeeded())
                m_frozen = true;
        }

        return isFrozen();
    }

    bool FrozenAddress::isFrozen() {
        return m_frozen;
    }


    /////// FrozenAddress Type ///////

    CheatManager::CheatManager() {
        dmntcht::initialize();

        // Get process metadata
        if (dmntcht::getCheatProcessMetadata(&m_processMetadata).failed())
            return;


        // Get all loaded cheats
        u64 cheatCnt = 0;
        if (dmntcht::getCheatCount(&cheatCnt).failed())
            return;

        dmntcht::DmntCheatEntry cheatEntries[cheatCnt];

        if (dmntcht::getCheats(cheatEntries, cheatCnt, 0, &cheatCnt).failed())
            return;
        
        m_cheats.reserve(cheatCnt);
        for (auto &cheatEntry : cheatEntries)
            m_cheats.push_back(new Cheat(cheatEntry));


        // Get all frozen addresses
        u64 frozenAddressCnt = 0;
        if (dmntcht::getFrozenAddressCount(&frozenAddressCnt).failed())
            return;

        dmntcht::DmntFrozenAddressEntry frozenAddressEntries[frozenAddressCnt];
        
        if (dmntcht::getFrozenAddresses(frozenAddressEntries, frozenAddressCnt, 0, &frozenAddressCnt).failed())
            return;

        m_frozenAddresses.reserve(frozenAddressCnt);
        for (auto &frozenAddressEntry : frozenAddressEntries)
            m_frozenAddresses.push_back(new FrozenAddress(frozenAddressEntry));

    }   

    CheatManager::~CheatManager() {
        for (auto &cheat : m_cheats)
            delete cheat;
        m_cheats.clear();

        for (auto &frozenAddress : m_frozenAddresses)
            delete frozenAddress;
        m_frozenAddresses.clear();

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


    u64 CheatManager::getTitleID() {
        return m_processMetadata.title_id;
    }

    u64 CheatManager::getProcessID() {
        return m_processMetadata.process_id;
    }

    u64 CheatManager::getBuildID() {
        return *reinterpret_cast<u64*>(m_processMetadata.main_nso_build_id);
    }


    std::vector<Cheat*>& CheatManager::getCheats() {
        return m_cheats;
    }

    std::vector<FrozenAddress*>& CheatManager::getFrozenAddresses() {
        return m_frozenAddresses;
    }


    MemoryInfo queryMemory(u64 address) {
        MemoryInfo memInfo = { 0 };

        dmntcht::queryCheatProcessMemory(&memInfo, address);

        return memInfo;
    }

    std::vector<MemoryInfo> getMemoryRegions() {
        MemoryInfo memInfo = { 0 };
        std::vector<MemoryInfo> memInfos;

        u64 lastAddress = 0;

        do {
            lastAddress = memInfo.addr;

            memInfo = queryMemory(memInfo.addr + memInfo.size);
            memInfos.push_back(memInfo);
        } while (lastAddress < (memInfo.addr + memInfo.size));

        return memInfos;
    }


    EResult CheatManager::readMemory(u64 address, u8 *buffer, size_t bufferSize) {
        return dmntcht::readCheatProcessMemory(address, buffer, bufferSize);
    }

    EResult CheatManager::writeMemory(u64 address, const u8 *buffer, size_t bufferSize) {
        return dmntcht::writeCheatProcessMemory(address, buffer, bufferSize);
    }

}