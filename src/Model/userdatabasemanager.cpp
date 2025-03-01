#include "userdatabasemanager.hpp"


/**
 * @brief Try to create an account with the given username and password
 * @param username A username passed by user
 * @param password A password passed by user
 * @return true if the username is unique and valid (the new account is created). 
 * Else returns false (the new account is not created.)
 */
bool UserDatabaseManager::newAccountCreated(const char* username, const char* password){
    bool newAccountCreated;

    // If the username is valid and unique AND the password is not blank, 
    // then save login info into database.
    if (isUniqueUsername(username) && isValidUsername(username) && strlen(password) != 0){ 
        // Setup SQL statement and values to be binded
        const char* statement = "INSERT INTO login (username, password) VALUES (?, ?)";
        std::string plainPassword = password;
        std::string hashedPassword = bcrypt::generateHash(plainPassword);
        
        // Prepare Statement and bind values to "?" symbols in a statement
        database.prepareStatement(statement);
        database.bindText(username, 1);
        database.bindText(hashedPassword.c_str(), 2);

        // Execute then destruct the statement
        database.execute();
        if (database.getReturnCode() == SQLITE_OK){
            std::cout << "Account created successfully...\n";
            newAccountCreated = true;
        }
        database.destructStatement();
    }
    else { // If the username is already taken then print error
        std::cerr << "Username already used.\n";
        newAccountCreated = false;
    }
    return newAccountCreated;
}


/**
 * @brief Compare the user's hashed password in a database with the input's hash. 
 * @param username A username passed by user
 * @param password A password passed by user
 * @return true if both hashes from the database and from the input's hash are 
 * matched. Else return false
 */
bool UserDatabaseManager::isCorrectLogin(const char* username, const char* password){
    // Always check the user input first
    bool isCorrect = false;
    if (isValidUsername(username) && strlen(password) != 0){
        // Prepare Statement and bind values to "?" then execute
        const char* query = "SELECT password FROM login WHERE username = ?";
        database.prepareStatement(query);
        database.bindText(username, 1);
        database.execute();

        // If the username exists, get a resulting hash in a database and convert 
        // it to string type then destruct the statement after getting the result
        if (database.getReturnCode() == SQLITE_ROW){
            const unsigned char* resultingHash = database.getTextValueFromColumn(0);
            std::string stringResultingHash = reinterpret_cast<const char*>(resultingHash);
            database.destructStatement();

            // Compare both hashes from the database and from the user input
            isCorrect = bcrypt::validatePassword(password, stringResultingHash);

            // If the login is correct, change the login status to 1.
            if(isCorrect){
                const char* query = "UPDATE login SET logged_in = 1 WHERE username = ?";
                database.prepareStatement(query);
                database.bindText(username, 1);

                // Execute the statement then destruct the statement
                database.execute();
                database.destructStatement();
            }
        }
    }
    return isCorrect;
}


/**
 * @brief Check whether a particular user is logged in or not.
 * @param username a username to check with the database
 * @return true if logged in. Else return false
 */
bool UserDatabaseManager::isLoggedIn(const char* username){
    // Always check the user input first
    bool isLoggedIn = false;
    if (isValidUsername(username)){
        // Prepare Statement and bind values to "?" symbols in a statement
        const char* query = "SELECT logged_in FROM login WHERE username = ?";
        database.prepareStatement(query);
        database.bindText(username, 1);

        // Execute the statement and get the result (0 or 1)
        database.execute(); 
        if (database.getReturnCode() == SQLITE_ROW){
            isLoggedIn = static_cast<bool>(database.getIntValueFromColumn(0));
        }
        
        // Destruct the statement after getting the result then return a result
        database.destructStatement();
    }
    return isLoggedIn;
}


/**
 * @brief Find a user ID from the username. The method returns the particular 
 * user ID only and only if the user has already logged in (to avoid data leakage).
 * @param username a username to check with the database
 * @return a user ID (> 0) else return -1
 */
int UserDatabaseManager::getUserId(const char* username){
    int userId;
    // Prepare Statement and bind username to "?" symbols in a statement
    const char* query = "SELECT user_id FROM login WHERE username = ? \
                                                   AND logged_in = 1";
    database.prepareStatement(query);
    database.bindText(username, 1);
    database.execute(); // Execute the statement

    // If there is a result, get the user ID. Else return -1    
    if (database.getReturnCode() == SQLITE_ROW){
        userId = database.getIntValueFromColumn(0);
    }
    else{
        userId = -1;
    }

    // Destruct the statement after getting the result then return a result
    database.destructStatement();
    return userId;
}


/**
 * @brief Change the username to a new username of the particular user ID
 * @param newUsername a new username
 * @param userId a user ID requesting to change the username
 * @return true if the username is valid and unique. Else return false
 */
bool UserDatabaseManager::usernameChanged(int userId, const char* newUsername){
    bool usernameChanged = false;
    if (isValidUsername(newUsername) && isUniqueUsername(newUsername)){
        // Prepare Statement and bind new username to a statement
        const char* query = "UPDATE login SET username = ? WHERE user_id = ?";
        database.prepareStatement(query);
        database.bindText(newUsername, 1);
        database.bindInt(userId, 2);

        // Execute the statement, destruct the statement, then set the return value
        database.execute();
        database.destructStatement();
        usernameChanged = true;
    }
    return usernameChanged;
}


/**
 * @brief Change the password to a new password of the particular user ID
 * @param newPassword a new password
 * @param userId a user ID requesting to change the password
 */
void UserDatabaseManager::changePassword(int userId, const char* newPassword){
    // Prepare Statement and bind new password to a statement
    const char* query = "UPDATE login SET password = ? WHERE user_id = ?";
    database.prepareStatement(query);
    database.bindText(newPassword, 1);
    database.bindInt(userId, 2);

    // Execute the statement and destruct the statement
    database.execute();
    database.destructStatement();
}


/**
 * @brief Set the sending file descriptor of a particular user ID
 * @param fileDescriptor a file descriptor
 * @param userId a user ID
 */
void UserDatabaseManager::setSendingFileDescriptor(int userId, int fileDescriptor){
    // Prepare Statement and bind sending file descriptor to a statement
    const char* query = "UPDATE login SET sending_file_descriptor = ? WHERE user_id = ?";
    database.prepareStatement(query);
    database.bindInt(fileDescriptor, 1);
    database.bindInt(userId, 2);

    // Execute the statement and destruct the statement
    database.execute();
    database.destructStatement();
}


/**
 * @brief Set the receiving file descriptor of a particular user ID
 * @param fileDescriptor a file descriptor
 * @param userId a user ID
 */
void UserDatabaseManager::setReceivingFileDescriptor(int userId, int fileDescriptor){
    // Prepare Statement and bind receiving file descriptor to a statement
    const char* query = "UPDATE login SET receiving_file_descriptor = ? WHERE user_id = ?";
    database.prepareStatement(query);
    database.bindInt(fileDescriptor, 1);
    database.bindInt(userId, 2);

    // Execute the statement and destruct the statement
    database.execute();
    database.destructStatement();
}


/**
 * @brief Log out the user from the system
 * @param userId a user ID
 */
void UserDatabaseManager::logOut(int userId){
    // Prepare Statement and bind user ID  to a statement
    const char* query = "UPDATE login SET logged_in = 0 WHERE user_id = ?";
    database.prepareStatement(query);
    database.bindInt(userId, 1);

    // Execute the statement then destruct the statement
    database.execute();
    database.destructStatement();
}


/**
 * @brief Delete the user account and all their data from the system
 * @param userId a user ID
 */
void UserDatabaseManager::deleteAccount(int userId){
    // Prepare Statement and bind user ID  to a statement
    const char* query = "DELETE FROM login WHERE user_id = ?";
    database.prepareStatement(query);
    database.bindInt(userId, 1);

    // Execute the statement then destruct the statement
    database.execute();
    database.destructStatement();
}


/**
 * @brief Check if the given username already exists in the database or not
 *        by querying the count result from the given username. 
 * @param username a username to check with the database
 * @return false if the given username already exists in the database. Else return true
 */
bool UserDatabaseManager::isUniqueUsername(const char* username){
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


/**
 * @brief Check if the given username has only alphanumeric characters. (A-Z & 0-9)
 * @param username a username to check with the database
 * @return true if the given username has only alphanumeric characters AND not blank. 
 * Otherwise, return false
 */
bool UserDatabaseManager::isValidUsername(const char* username){
    bool isValid = true;

    if (strlen(username) == 0){    // If the username is blank
        isValid = false;
    } 
    // If not blank, check each character
    else { 
        while (*username && isValid) {
            isValid = std::isalnum(static_cast<unsigned char>(*username));
            ++username;
        }
    }
    return isValid;
}


/**
 * @brief Check of a user ID exists in the HighestScore table or not
 * @param userId a user ID
 * @return true if the user ID already exists in the table. Else return false
 */
bool UserDatabaseManager::isInHighestScoreTable(int userId){
    // Prepare Statement and bind user ID to a statement
    const char* query = "SELECT count(user_id) FROM HighestScore WHERE user_id = ?";
    database.prepareStatement(query);
    database.bindInt(userId, 1);

    // Get count result from the column (index = 0 on the most left column)
    database.execute();
    int countResult = database.getIntValueFromColumn(0);

    // Destruct the statement after getting the result
    database.destructStatement();

    // Return the result
    bool isInTable;
    if (countResult == 0) // i.e. the user ID doesn't yet exist in the table.
        isInTable = false;
    else
        isInTable = true;

    return isInTable;
}


/**
 * @brief Set the highest score of a particular user ID
 * @param highestScore a highest score
 * @param userId a user ID
 */
void UserDatabaseManager::setHighestScore(int userId, int highestScore){
    // If the user ID is not yet in a table, use "INSERT". Else use "UPDATE"
    const char* statement;
    if (isInHighestScoreTable(userId)){
        // UPDATE ONLY IF highest score is greater than the current score in DB
        statement = "UDPATE HighestScore SET highest_score = ? WHERE user_id = ? \
                     AND highest_score < ?"; 
        database.prepareStatement(statement);
        database.bindInt(highestScore, 3);
    }
    else{
        statement = "INSERT INTO HighestScore (highest_score, user_id) VALUES (?, ?)";
        database.prepareStatement(statement);
    }

    // Bind the common values
    database.bindInt(highestScore, 1);
    database.bindInt(userId, 2);
        
    // Execute then destruct the statement
    database.execute();
    database.destructStatement();
}

/**
 * @brief Delete the highest score of a particular user ID
 * @param userId a user ID
 */
void UserDatabaseManager::deleteHighestScore(int userId){
    // Prepare Statement and bind user ID  to a statement
    const char* query = "DELETE FROM HighestScore WHERE user_id = ?";
    database.prepareStatement(query);
    database.bindInt(userId, 1);

    // Execute the statement then destruct the statement
    database.execute();
    database.destructStatement();
}