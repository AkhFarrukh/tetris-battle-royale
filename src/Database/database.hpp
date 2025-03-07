#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h> // MUST be installed first via "sudo apt install libsqlite3-dev"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <string.h>

using namespace std;

/**
 * @brief A class for the entire operation of a database including methods for
 * connecting to database, querying, and disconnecting from the database after
 * the end of the program
 */
class Database{
    private:
        sqlite3 *db;        // A database handle
        int returnCode;
        char* errorMessage;

    public:
        Database(const char* databaseFileName);
        ~Database();
        sqlite3_stmt* stmt; // A prepared statement

        void setReturnCode(int newReturnCode);
        int  getReturnCode();

        void prepareStatement(const char* statement);
        void bindText(const char* value, int index);
        void bindInt(int value, int index);
        void execute();
        void destructStatement();

        int getUserId(const char* username);
        int  getIntValueFromColumn(int columnIndex);
        const unsigned char* getTextValueFromColumn(int columnIndex);
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
};

#endif