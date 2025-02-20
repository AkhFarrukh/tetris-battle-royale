#ifndef LOGININTERFACE_HPP
#define LOGININTERFACE_HPP

#include "database.hpp"
#include "bcrypt.h"

#include <string.h>
#include <string>
#include <iostream>

/**
 * @brief A class responsible for operations on account such as sign up and login
 */

class LoginInterface {
    
    private:
        Database database{"tetris.db"};

    public:
        bool newAccountCreated(const char* username, const char* password);
        bool isSuccessfulLogin(const char* username, const char* password);
        bool isUniqueUsername(const char* username);
};

#endif