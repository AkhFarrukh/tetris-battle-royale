#include "database.hpp"


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
 * @brief prepare a statement (the resultant statement is found in "&stmt".)
 * to prevent "SQL Injection"
 * @param statement: a statement to prepare
 */
void Database::prepareStatement(const char* statement){
    int returnCode = sqlite3_prepare_v2(db, statement, strlen(statement), &stmt, nullptr);
    if (returnCode != SQLITE_OK)  fprintf(stderr, "ERROR PREPARING STATEMENT!");
    
}


/**
 * @brief bind a TEXT value to a statement (&stmt)
 * @param value: a value to be binded
 * @param index: a column index (starting from 1 (not zero!) from the most left)
 */
void Database::bindText(const char* value, int index){
    int returnCode = sqlite3_bind_text(stmt, index, value, strlen(value), SQLITE_STATIC);
    if (returnCode != SQLITE_OK)  fprintf(stderr, "ERROR BINDING VALUE!");
}


/**
 * @brief bind an INT value to a statement (&stmt)
 * @param value: a value to be binded
 * @param index: a column index (starting from 1 (not zero!) from the most left)
 */
void Database::bindInt(int value, int index){
    int returnCode = sqlite3_bind_int(stmt, index, value);
    if (returnCode != SQLITE_OK)  fprintf(stderr, "ERROR BINDING VALUE!");
}


/**
 * @brief destruct the "&stmt" variable (opposite of preparing statement)
 */
void Database::destructStatement(){
    sqlite3_finalize(stmt);
}


/**
 * @brief execute the statement: needed to be called at least once after preparing
 * the statement. For querying, it stops row-by-row until the end of result.
 */
void Database::execute(){
    returnCode = sqlite3_step(stmt);
}


/**
 * @brief get INT result in the given column
 * @param columnIndex: straightforward (index = 0 on the most left column)
 */
int Database::getIntValueFromColumn(int columnIndex){
    return sqlite3_column_int(stmt, columnIndex);
}


/**
 * @brief get TEXT result in the given column
 * @param columnIndex: straightforward (index = 0 on the most left column)
 */
const unsigned char* Database::getTextValueFromColumn(int columnIndex){
    return sqlite3_column_text(stmt, columnIndex);
}


/**
 * @brief set returnCode to a new value
 * @param newReturnCode as a new return code value to be set
 */
void Database::setReturnCode(int newReturnCode){
    returnCode = newReturnCode;
}


/**
 * @brief get returnCode 
 * @return a value of return code
 */
int Database::getReturnCode(){
    return returnCode;
}