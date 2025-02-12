#include "database.hpp"
#include <stdio.h>


/**
 * @brief a callback function which prints the entire output data
 * This function is passed into sqlite3_exec() if we want to get an output.
 * (Can't be part of a class i.e. must always be a "function")
 */
int printCallback(void *data, int numberOfColumns, char **values, char **columnsNames){
    
    (void)data; // Stop the "unused parameter" warning
    for(int column = 0; column < numberOfColumns; ++column){
        printf("%s = %s\n", columnsNames[column], values[column]);
    }
    return 0;
}


/**
 * @brief A constructor: connect to the database file (in UTF-8 format!)
 * If the file already exists, it is then used. Else the new file will be created.
 * @param databaseFileName the name of the database file to connect to
 */
Database::Database(const char* databaseFileName){
    
    // "returnCode" returns 0 (SQLITE_OK) on successful file opening
    returnCode = sqlite3_open(databaseFileName, &db); 
    if (returnCode == SQLITE_OK){
        fprintf(stderr, "Database connection established\n");
    }
    else{
        fprintf(stderr, "Database connection failed\n");
    }
}


/**
 * @brief A destructor: close the database connection
 */
Database::~Database(){
    sqlite3_close(db);
}


/**
 * Query method is of type void as they give no "DIRECT" return value.
 * @param sql a statement in SQL
 * @param callback a callback function that respects the signature described below
 *        (can be NULL if no output is needed.)
 * @param data the argument that can be accessed and used in a callback function
 * 
 * Any query giving output needs a "callback" function which will deal with the
 * output as the output is not directly returned back to the method. 
 * 
 * The callback function is called every time for each record output 
 * by the query (e.g. being called 10 times if there are 10 output records). 
 * 
 * It is the job of the method user to "build" his own callback FUNCTION WITH
 * A PARTICULAR SQLITE SIGNATURE in order to get and use (e.g. print or compare) 
 * the output data. If callback is NULL, the result is ignored.
 * 
 * The signature: int callback(void *data, int numberOfColumns, char **values, char **columnsNames)
 * !!! The parameter names can be changed but the types must always remain the same. !!!
 * - data: the 4th argument passed to sqlite3_exec()
 * - numberOfColumns: the number of columns in each row
 * - values: an array of string, each string is a value for each column (in a row).
 * - columnNames: an array of string, each string is the names of columns.
 * 
 * @see{https://www.sqlite.org/c3ref/exec.html}
 * @see{https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm}
 * 
*/
void Database::query(const char* sql, int (*callback)(void*, int, char**, char**), void* data){
    
    // Execute SQL query and check the return code
    returnCode = sqlite3_exec(db, sql, callback, data, &errorMessage);
    if (returnCode == SQLITE_OK) {
        fprintf(stdout, "Query done successfully\n");
    } 
    else {
        fprintf(stderr, "SQL error: %s\n", errorMessage);
    }
}

/**
 * Example for INSERT method:
 *  const char* myQuery = "INSERT INTO login (username, password) VALUES ('joe', 5678)";
 *  query(myQuery, NULL, 0); // Put zero as "data" for the sake of example
 * 
 * Example for SELECT method: 
 *  const char* myQuery = "SELECT * FROM login";
 *  query(myQuery, printCallback, 0);
 */