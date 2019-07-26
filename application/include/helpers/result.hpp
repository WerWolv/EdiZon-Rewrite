#pragma once

#include <switch.h>

#define MODULE_EDIZON 555

namespace edz {
    class EResult {
    public:
        EResult();

        EResult(u32 module, u32 desc);

        EResult(Result result);

        constexpr u32 getModule();

        constexpr u32 getDescription();

        bool operator==(EResult &other);
        bool operator==(Result &other);
        bool operator!=(EResult &other);
        bool operator!=(Result &other);

        EResult operator=(u32 &other);
        EResult operator=(EResult &other);
        EResult operator=(Result other);

        operator u32() const;

        bool succeeded();
        bool failed();

    private:
        const u32 m_module, m_desc;
    };
}