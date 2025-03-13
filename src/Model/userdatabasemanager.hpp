#ifndef USERDATABASEMANAGER_HPP
#define USERDATABASEMANAGER_HPP

#include "database.hpp"
#include "bcrypt.h"

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <tuple>

/**
 * @brief A class responsible for operations regarding users with the database
 */

class UserDatabaseManager {
    
    private:
        Database database{"tetris.db"};

    public:

        bool newAccountCreated(const char* username, const char* password);
        bool isCorrectLogin(const char* username, const char* password);
        bool isLoggedIn(const char* username);
        int  getUserId(const char* username);
        bool usernameChanged(int userId, const char* newUsername);
        void changePassword(int userId, const char* newPassword);
        void setSendingFileDescriptor(int userId, int fileDescriptor);
        void setReceivingFileDescriptor(const string& username, int fileDescriptor);
        void logOut(int userId);
        void deleteAccount(int userId);

        bool isUniqueUsername(const char* username);
        bool isValidUsername(const char* username);

        // bool isInHighestScoreTable(int userId);
        // void setHighestScore(int userId, int highestScore);
        // void deleteHighestScore(int userId);
        void updatePlayerStats(int userId, int newScore, bool won, int energyUsed, std::string gameMode);
        std::vector<std::tuple<int, std::string, int, int, int, int>> getRanking(std::string mode, int limit);
        // std::vector<std::tuple<int, std::string, int>> getRanking(int limit = 0);

        void addSession(int userId, int client_fd);
        void removeSession(int userId);
};

#endif