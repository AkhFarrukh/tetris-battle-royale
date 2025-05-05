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
        void setLogin(int user_id);
        int  getUserId(const char* username);
        bool usernameChanged(int userId, const char* newUsername);
        void changePassword(int userId, const char* newPassword);
        void setSendingFileDescriptor(int userId, int fileDescriptor);
        void setReceivingFileDescriptor(const string& username, int fileDescriptor);
        void logOut(int userId);
        void deleteAccount(int userId);

        bool isUniqueUsername(const char* username);
        bool isValidUsername(const char* username);

        void addScore(int userId, int score);
        std::vector<std::pair<std::string, int>> getTopScores(int limit);

        void addSession(int userId, int client_fd);
        void removeSession(int userId);

        vector<string> friendsList(const string& username);
        void deleteFriend(const string& user, const string& friendDelete);
        void addFriend(const string& user, const string& newFriend);
        int verifyRequestFriend(const string& user, const string& newFriend);
        int requestFriend(const string& user, const string& newFriend);
        void deleteRequestFriend(const string& user, const string& newFriend);
        int getNumberFriendRequestsReceived(const string& user);
        vector<string> requestReceivedList(const string& username);
        vector<string> requestSentList(const string& username);
        void conversationOpen(int user_id, int friend_id);
        void conversationClose(int user_id, int friend_id);
        int getInConversation(int user_id, int friend_id);
        void addMessage(int sender_id, int receiver_id, const std::string& message);
        void deleteMessages(int sender_id, int receiver_id);
        std::vector<std::string> getMessages(int sender_id, int receiver_id);
        int getNumberMessageNoRead(int user_id, int friend_id);
        void setNotInConversation(int user_id);
        bool isUserInSession(int user_id);
        void deleteSession(int user_id);
        std::vector<int> getUserIdSession();
        int getSockUser(int user_id);
};

#endif