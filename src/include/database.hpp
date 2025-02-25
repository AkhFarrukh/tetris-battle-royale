#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h> // MUST be installed first via "sudo apt install libsqlite3-dev"
#include <stdio.h>
#include <string.h>


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
        
        // Methods for SQL statement preparing
        void prepareStatement(const char* statement);
        void bindText(const char* value, int index);
        void execute();
        void destructStatement();

        // Methods for retrieving values from a specific column
        int  getIntValueFromColumn(int columnIndex);
        const unsigned char* getTextValueFromColumn(int columnIndex);      
};

#endif