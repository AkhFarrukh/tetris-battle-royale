#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h> // MUST be installed first via "sudo apt install libsqlite3-dev"
#include <string>


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

        // Method
        void query(const char* sql, int (*callback)(void*, int, char**, char**), void* data);
};

int printCallback(void *data, int numberOfColumns, char **values, char **columnsNames);

#endif