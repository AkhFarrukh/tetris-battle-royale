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
    else {
        fprintf(stderr, "Database connection failed\n");
    }

    // Load statements which create tables and execute them one by one
    // Because we want to directly execute every statement so execute "pair.second"
    // without getting a key then get a value (a statement) to execute.
    loadStatements("createtables.sql");
    for (const auto& pair : statements){
        prepareStatement((pair.second).c_str());
        execute();
        destructStatement();
    }

    // Load all statements (Data Manipulation Language: DML) 
    // which will be used in the code
    loadStatements("statements.sql");
}

/**
 * @brief A destructor: close the database connection
 */
Database::~Database(){
    sqlite3_close(db);
}

/**
 * @brief Read a file from "statementsFileName" parameter which is a file that 
 * stores ALL statements in a structured way (key/value system): a key always 
 * starts with a double dash ("--") and serves as a name of a statement then a 
 * statement (whether single or multiple line(s)) is under a line containing the 
 * particular key. All data are stored in an attribute "statements" which has a 
 * similar structure to a Python dictionary.
 * 
 * The file is then read line by line. If it finds a line with a double dash, we 
 * strip it then we get a key. We then read the next lines which contain a statement
 * and that statement (single or multiple line) shall be assigned to that key.
 * 
 * E.g. If a query "SELECT username FROM user", as a value, has a key "getUsername".
 * 
 * FYI: In an iteration e.g. for/while loop, the ".first" is used in getting the key
 * and ".second" is used in getting a value of that key. 
 * We can also directly access it if we know a particular key: 
 * e.g. statements["getUsername"] returns "SELECT username FROM user".
 * 
 * @param statementsFileName a name of a file containing statements
 */
void Database::loadStatements(const std::string statementsFileName){
    // Setup variables
    std::ifstream statementsData(statementsFileName);
    std::string line, statementKey, statement;

    // Clear "statements" attribute if it isn't empty.
    // It should be cleared every time to avoid any possible bug upon loading.
    if(!statements.empty()){
        statements.clear();
    }

    // Read a file line by line
    while (std::getline(statementsData, line)){
        
        // If the line starts with a double dash
        if (line.rfind("--", 0) == 0) { 
            // If statement key is not null, put statement as a value to statementKey
            if (!statementKey.empty()) {
                statements[statementKey] = statement;
            }
            statementKey = line.substr(3); // Remove '-- ' from the comment
            statement.clear();
        } 

        // If the line doesn't start with a double dash, it's part of a statement.
        else {
            statement += line + "\n";
        }
    }

    // After the end of the file, load the last statement if one exists.
    if (!statementKey.empty()) {
        statements[statementKey] = statement;
    }
}

/**
 * @brief get from a key a statement in "statements" attribute 
 * @return a statement corresponding to a particular key
 */
std::string Database::getStatement(const std::string key){
    std::cout << statements.at(key) << std::endl;
    return statements.at(key);
}


/**
 * @brief prepare a statement (the resultant statement is found in "&stmt".)
 * to prevent "SQL Injection"
 * @param statement: a statement to prepare
 */
void Database::prepareStatement(const char* statement){
    int returnCode = sqlite3_prepare_v2(db, statement, -1, &stmt, nullptr);
    if (returnCode != SQLITE_OK)  fprintf(stderr, "ERROR PREPARING STATEMENT!");
    std::cout << returnCode << std::endl;
}


/**
 * @brief bind a TEXT value to a statement (&stmt)
 * @param value: a value to be binded
 * @param index: a column index (starting from 1 (not zero!) from the most left)
 */
void Database::bindText(const char* value, int index){
    int returnCode = sqlite3_bind_text(stmt, index, value, static_cast<int>(strlen(value)), SQLITE_STATIC);
    if (returnCode != SQLITE_OK)  fprintf(stderr, "ERROR BINDING VALUE!");
    std::cout << returnCode << std::endl;
    if (stmt == nullptr) std::cout << "nullptr here" << std::endl;
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