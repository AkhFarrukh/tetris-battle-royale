#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h> // MUST be installed first via "sudo apt install libsqlite3-dev"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <unordered_map>
#include <fstream>

/**
 * @brief A class for the entire operation of a database including methods for
 * connecting to database, querying, and disconnecting from the database after
 * the end of the program
 * 
 * Attention! This class manages the database at the lowest level such as 
 * preparing statements, bind arguments, execute, get values, etc. 
 * Any method that wants to manipulate the database SHALL NOT be in this class. 
 * Instead, it SHALL be in the "UserDatabaseManager" class which has this class 
 * as an attribute and will use its methods from there.
 */
class Database{
    private:
        sqlite3 *db;        // A database handle
        int returnCode;
        char* errorMessage;

        // All SQL statements that will be executed in SQLite shall be stored
        // in this attribute in a form of "unordered_map" (similar to Python dict)
        // which stores data "key/value" way i.e. we get a value from a key.
        std::unordered_map<std::string, std::string> statements;

    public:
        Database(const char* databaseFileName);
        ~Database();
        sqlite3_stmt* stmt; // A prepared statement

        void setReturnCode(int newReturnCode);
        int  getReturnCode();

        void loadStatements(const std::string statementsFileName);
        std::string getStatement(const std::string key);

        void prepareStatement(const char* statement);
        void bindText(const char* value, int index);
        void bindInt(int value, int index);
        void execute();
        void destructStatement();

        int  getIntValueFromColumn(int columnIndex);
        const unsigned char* getTextValueFromColumn(int columnIndex);
};

#endif