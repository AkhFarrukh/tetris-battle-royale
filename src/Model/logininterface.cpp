#include "logininterface.hpp"


/**
 * @brief Create an account: if the username is unique, the account is created 
 * then returns true. Else the account creation is failed and returns false.
 * @param username A username passed by user
 * @param password A password passed by user
 */
bool LoginInterface::newAccountCreated(const char* username, const char* password){
    bool newAccountCreated;
    // Check the uniqueness of a username first
    if(isUniqueUsername(username)) { // Then save login info into database
        // Prepare Statement and bind username (to "?" symbol) to a statement
        const char* statement = "INSERT INTO login (username, password) VALUES (?, ?)";
        std::string stringPassword = password;
        std::string hashedPassword = bcrypt::generateHash(stringPassword);

        database.prepareStatement(statement);
        database.bindText(username, 1);
        database.bindText(hashedPassword.c_str(), 2);

        // Execute then destruct the statement
        database.execute();
        database.destructStatement();
        std::cout << "Account created successfully...\n";
        newAccountCreated = true;
    }
    else { // Then print error
        std::cerr << "Username already used.\n";
        newAccountCreated = false;
    }
    return newAccountCreated;
}


/**
 * @brief Compare the user's hashed password in a database with the entered password. 
 * Return true if both are matched else return false.
 * @param username A username passed by user
 * @param password A password passed by user
 */
bool LoginInterface::isSuccessfulLogin(const char* username, const char* password){
    // Prepare Statement and bind username (to "?" symbol) to a statement
    const char* query = "SELECT password FROM login WHERE username = ?";
    database.prepareStatement(query);
    database.bindText(username, 1);

    // Get hashed password in a database
    database.execute();
    const unsigned char* hashedPassword = database.getTextValueFromColumn(0);
    std::string stringHashedPassword = reinterpret_cast<const char*>(hashedPassword);

    // Destruct the statement after getting the result
    database.destructStatement();

    // Compare both hashes
    bool successful = bcrypt::validatePassword(password, stringHashedPassword);
    return successful;
}


/**
 * @brief Check if the given username already exists in the database or not
 *        by querying the count result from the given username. 
 * @param username a username to check with the database
 * 
 * Return false if the given username already exists in the database. 
 * Else return true
 */
bool LoginInterface::isUniqueUsername(const char* username){
    // Prepare Statement and bind username (to "?" symbol) to a statement
    const char* query = "SELECT count(username) FROM login WHERE username = ?";
    database.prepareStatement(query);
    database.bindText(username, 1);

    // Get count result from the column (index = 0 on the most left column)
    database.execute();
    int countResult = database.getIntValueFromColumn(0);

    // Destruct the statement after getting the result
    database.destructStatement();

    // Return the result
    bool isUnique;
    if (countResult == 0) // i.e. the username is unique
        isUnique = true;
    else
        isUnique = false;

    return isUnique;
}