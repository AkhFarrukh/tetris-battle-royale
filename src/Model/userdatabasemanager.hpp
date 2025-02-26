#ifndef USERDATABASEMANAGER_HPP
#define USERDATABASEMANAGER_HPP

#include "database.hpp"
#include "bcrypt.h"

#include <string.h>
#include <string>
#include <iostream>

/**
 * @brief A class responsible for operations regarding users with the database
 */

class UserDatabaseManager {
    
    private:
        Database database{"tetris.db"};

    public:
        // "Create" and "Read" methods for login data
        bool newAccountCreated(const char* username, const char* password);
        bool isCorrectLogin(const char* username, const char* password);

        // Methods for validations
        bool isUniqueUsername(const char* username);
        bool isValidUsername(const char* username);
};

#endif