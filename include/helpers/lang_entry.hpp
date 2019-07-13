#pragma once

#include "edizon.hpp"

#include <string>
#include <map>

namespace edz {

    class LangEntry {
    public:
        LangEntry(std::string unlocalizedString);

        std::string get();

    private:
        std::string m_unlocalizedString;

        static inline std::map<std::string, std::string> m_localizedStrings;
    };

}
