#include "helpers/result.hpp"
#include "helpers/utils.hpp"

namespace edz {

    EResult::EResult() : m_module(0), m_desc(0) {}

    EResult::EResult(u32 module, u32 desc) : m_module(module), m_desc(desc) {}

    EResult::EResult(Result result) : m_module(static_cast<u32>(R_MODULE(result))), m_desc(static_cast<u32>(R_DESCRIPTION(result))) {}

    u32 EResult::getModule() {
        return this->m_module;
    }

    u32 EResult::getDescription() {
        return this->m_desc;
    }

    std::string EResult::getString() {
        return edz::helper::formatString("2%03d-%04d (0x%x)", this->getModule(), this->getDescription(), static_cast<u32>(*this));
    }

    bool EResult::operator==(EResult &other) {
        return this->getDescription() == other.getDescription();
    }

    bool EResult::operator!=(EResult &other) {
        return this->getDescription() != other.getDescription();
    }

    bool EResult::operator==(Result &other) {
        return this->getDescription() == EResult(other).getDescription() && this->getDescription() == EResult(other).getModule();
    }

    bool EResult::operator!=(Result &other) {
        return this->getDescription() != EResult(other).getDescription() || this->getDescription() != EResult(other).getModule();
    }

    bool EResult::succeeded() {
        return this->m_desc == 0;
    }

    bool EResult::failed() {
        return this->m_desc != 0;
    }

    EResult EResult::operator=(u32 &other) {
        return Result(other);
    }

    EResult EResult::operator=(EResult &other) {
        return edz::EResult(other.getDescription());
    }

    EResult EResult::operator=(Result other) {
        return EResult(static_cast<u32>(other));
    }

    EResult::operator u32() const { 
        return MAKERESULT(this->m_module, this->m_desc);
    }

}