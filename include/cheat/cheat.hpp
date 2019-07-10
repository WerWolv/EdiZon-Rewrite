#pragma once

#include "edizon.hpp"
#include "cheat/dmntcht.hpp"

#include <string>
#include <vector>

namespace edz::cheat {

    class Cheat {
    public:
        Cheat(edz::dmntcht::DmntCheatEntry cheatEntry);

        std::string getName();
        u32 getID();

        bool toggle();
        bool isEnabled();

    private:
        std::string m_name;
        u32 m_id;
    };

    class FrozenAddress {
    public:
        FrozenAddress(edz::dmntcht::DmntFrozenAddressEntry);
        FrozenAddress(u64 address, u8 width, u64 value);
        FrozenAddress(u64 address, u8 width);

        u64 getAddress();
        u8 getWidth();
        u64 getValue();
        u64 setValue(u64 value, u8 width);

        bool toggle();
        bool isFrozen();

    private:
        edz::dmntcht::DmntFrozenAddressEntry m_frozenAddressEntry;
        bool m_frozen;
    };


    class CheatManager {
    public:
        static CheatManager& getInstance() {
            static CheatManager instance;
            
            return instance;
        }

        CheatManager(CheatManager const&) = delete;
        void operator=(CheatManager const&) = delete;

        bool forceAttach();
        bool hasCheatProcess();

        u64 getTitleID();
        u64 getProcessID();
        u64 getBuildID();

        std::vector<Cheat*>& getCheats();
        std::vector<FrozenAddress*>& getFrozenAddresses();

        MemoryInfo queryMemory(u64 address);
        std::vector<MemoryInfo> getMemoryRegions();

        EResult readMemory(u64 address, u8 *buffer, size_t bufferSize);
        EResult writeMemory(u64 address, const u8 *buffer, size_t bufferSize);

    private:
        CheatManager();
        ~CheatManager();

        std::vector<Cheat*> m_cheats;
        std::vector<FrozenAddress*> m_frozenAddresses;

        edz::dmntcht::DmntCheatProcessMetadata m_processMetadata;
    };

} 