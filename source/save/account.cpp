#include "save/account.hpp"

namespace edz::save {

        Account::Account(u128 userID) : m_userID(userID) {
            
        }

        u128 Account::getUserID() {
            return m_userID;
        }

}