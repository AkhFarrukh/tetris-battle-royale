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
        std::string statement = database.getStatement("newAccountCreated");
        std::string plainPassword = password;
        std::string hashedPassword = bcrypt::generateHash(plainPassword);
        
        // Prepare Statement and bind values to "?" symbols in a statement
        database.prepareStatement(statement.c_str());
        database.bindText(username, 1);
        database.bindText(hashedPassword.c_str(), 2);

        // Execute then destruct the statement
        database.execute();
        if (database.getReturnCode() == SQLITE_OK){
            std::cout << "\nAccount created successfully...\n";
            newAccountCreated = true;
        }
        database.destructStatement();
    }
    else { // If the username is already taken then print error
        std::cerr << "\nUsername already used.\n";
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
        std::string query = database.getStatement("isCorrectLogin");
        database.prepareStatement(query.c_str());
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
        }
    }
    return isCorrect;
}

void UserDatabaseManager::setLogin(int user_id){
    std::string query = database.getStatement("setLogin");
    database.prepareStatement(query.c_str());
    database.bindInt(user_id, 1);

    // Execute the statement then destruct the statement
    database.execute();
    database.destructStatement();
}

/**
 * @brief Check whether a particular user is logged in or not.
 * @param username a username to check with the database
 * @return true if logged in. Else return false
 */
bool UserDatabaseManager::isLoggedIn(const char* username){
    bool isLoggedIn = false;

    // Always check the user input first
    if (isValidUsername(username)){
        // Prepare Statement and bind values to "?" symbols in a statement
        std::string query = database.getStatement("getLogInStatus");
        database.prepareStatement(query.c_str());
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
    std::string query = database.getStatement("getUserId");
    database.prepareStatement(query.c_str());
    database.bindText(username, 1);
    database.execute(); // Execute the statement

    // If there is a result, get the user ID. Else return -1    
    if (database.getReturnCode() == SQLITE_ROW){
        userId = database.getIntValueFromColumn(0);
    }
    else {
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
        std::string query = database.getStatement("changeUsername");
        database.prepareStatement(query.c_str());
        database.bindText(newUsername, 1);
        database.bindInt(userId, 2);

        // Execute the statement, destruct the statement, then set the return value
        database.execute();
        database.destructStatement();
        if (database.getReturnCode() == SQLITE_DONE) {
            usernameChanged = true;
        }
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
    std::string query = database.getStatement("changePassword");
    database.prepareStatement(query.c_str());
    database.bindText(newPassword, 1);
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
    std::string query = database.getStatement("logOut");
    database.prepareStatement(query.c_str());
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
    // Prepare Statement and bind user ID to a statement
    std::string query = database.getStatement("deleteAccount");
    database.prepareStatement(query.c_str());
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
    std::string query = database.getStatement("isUniqueUsername");
    database.prepareStatement(query.c_str());
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
 * @brief Add the session and a client file descriptor
 * @param client_fd a client file descriptor
 * @param userId a user ID
 */
void UserDatabaseManager::addSession(int userId, int client_fd){
    // Prepare Statement and bind values to a statement
    std::string query = database.getStatement("addSession");
    database.prepareStatement(query.c_str());
    database.bindInt(client_fd, 2);
    database.bindInt(userId, 1);

    // Execute the statement and destruct the statement
    database.execute();
    database.destructStatement();
}


/**
 * @brief Remove the session of a particular user ID
 * @param userId a user ID
 */
void UserDatabaseManager::removeSession(int userId){
    // Prepare Statement and bind user ID to a statement
    std::string query = database.getStatement("removeSession");
    database.prepareStatement(query.c_str());
    database.bindInt(userId, 1);

    // Execute the statement then destruct the statement
    database.execute();
    database.destructStatement();
}


void UserDatabaseManager::addScore(int userId, int score) {
    std::string insertQuery = database.getStatement("addScore");

    database.prepareStatement(insertQuery.c_str());
    database.bindInt(userId, 1);
    database.bindInt(score, 2);
    
    database.execute();
    
    if (database.getReturnCode() != SQLITE_DONE) {
        std::cerr << "Error inserting score" << std::endl;
    }
    
    database.destructStatement();
}

std::vector<std::pair<std::string, int>> UserDatabaseManager::getTopScores(int limit) {
    std::vector<std::pair<std::string, int>> topScores;
    std::string query = database.getStatement("getTopScores");

    database.prepareStatement(query.c_str());
    database.bindInt(limit, 1);
    
    database.execute();
    
    while (database.getReturnCode() == SQLITE_ROW) {
        std::string username = reinterpret_cast<const char*>(database.getTextValueFromColumn(0));
        int score = database.getIntValueFromColumn(1);
        topScores.push_back({username, score});
        
        database.execute();  // Move to next row
    }
    
    database.destructStatement();
    return topScores;
}


std::vector<std::string> UserDatabaseManager::getFriendsList(const std::string& username){
    std::vector<std::string> friendsList;

    // Préparer la requête SQL pour récupérer les amis de l'utilisateur
    std::string sql = database.getStatement("getFriendsList");

    // Préparer la déclaration
    database.prepareStatement(sql.c_str());

    // Binder le nom de l'utilisateur (par exemple, "David")
    database.bindText(username.c_str(), 1);

    database.execute();

    // Exécuter la requête et récupérer les résultats ligne par ligne
    while (database.getReturnCode() == SQLITE_ROW) {
        // Récupérer le nom de l'ami
        const unsigned char* ami = database.getTextValueFromColumn(0);
        friendsList.push_back(reinterpret_cast<const char*>(ami));  // Ajouter l'ami au vecteur
        database.execute();
    }

    // Libérer la mémoire après exécution
    database.destructStatement();  // Libération de la mémoire après exécution

    return friendsList;  // Retourner le vecteur des amis
}

void UserDatabaseManager::deleteFriend(const std::string& user, const std::string& friendDelete){
    std::string sql = database.getStatement("deleteFriend");
    database.prepareStatement(sql.c_str());

    database.bindText(user.c_str(), 1);
    database.bindText(friendDelete.c_str(), 2);
    database.bindText(friendDelete.c_str(), 3);
    database.bindText(user.c_str(), 4);

    database.execute();
}

void UserDatabaseManager::addFriend(const std::string& user, const std::string& newFriend){
    std::string sql = database.getStatement("addFriend");
    database.prepareStatement(sql.c_str());

    database.bindText(user.c_str(), 1);
    database.bindText(newFriend.c_str(), 2);
    database.bindText(newFriend.c_str(), 3);
    database.bindText(user.c_str(), 4);

    database.execute();
}

int UserDatabaseManager::verifyFriendRequest(const std::string& user, const std::string& newFriend){
    int result = -1; //erreur pas prise en charge
    std::string sql = database.getStatement("verifyFriendRequest");
    database.prepareStatement(sql.c_str());

    database.bindText(newFriend.c_str(), 1);
    database.bindText(user.c_str(), 2);
    database.bindText(newFriend.c_str(), 3);
    database.bindText(user.c_str(), 4);
    database.bindText(newFriend.c_str(), 5);
    database.bindText(user.c_str(), 6);
    database.bindText(newFriend.c_str(), 7);
    
    database.execute();

    // The returned int value means:
    // 1: unknown username
    // 2: already be friend
    // 3: friend request was already sent.
    // 4: the other friend has already sent the friend request.
    // 0: other cases
    if(database.getReturnCode() == SQLITE_ROW){
      result = database.getIntValueFromColumn(0);
    }
    database.destructStatement();

    return result;
}

int UserDatabaseManager::sendFriendRequest(const std::string& user, const std::string& newFriend){
    std::string sql = database.getStatement("sendFriendRequest");
    
    database.prepareStatement(sql.c_str());
    database.bindText(user.c_str(), 1);
    database.bindText(newFriend.c_str(), 2);
    database.execute();

    return(database.getReturnCode() != SQLITE_ERROR);
}

void UserDatabaseManager::deleteRequestFriend(const std::string& user, const std::string& friendDelete){
    std::string sql = database.getStatement("deleteFriendRequest");
    database.prepareStatement(sql.c_str());

    database.bindText(user.c_str(), 1);
    database.bindText(friendDelete.c_str(), 2);
    database.bindText(friendDelete.c_str(), 3);
    database.bindText(user.c_str(), 4);
    
    database.execute();
}

int UserDatabaseManager::getNumberFriendRequestsReceived(const std::string& user){
    int numberRequests = 0;
    std::string sql = database.getStatement("getNumberFriendRequestsReceived");

    database.prepareStatement(sql.c_str());
    database.bindText(user.c_str(),1);
    database.execute();

    if(database.getReturnCode() == SQLITE_ROW){
        numberRequests = database.getIntValueFromColumn(0);
    }
    database.destructStatement();

    return numberRequests;
}

std::vector<std::string> UserDatabaseManager::getReceivedRequestsList(const std::string& username){
    std::vector<std::string> friendsList;

    // Préparer la requête SQL pour récupérer les amis de l'utilisateur
    std::string sql = database.getStatement("getReceivedRequestsList");

    // Préparer la déclaration
    database.prepareStatement(sql.c_str());

    // Binder le nom de l'utilisateur (par exemple, "David")
    database.bindText(username.c_str(),1);

    database.execute();
    // Exécuter la requête et récupérer les résultats ligne par ligne
    while (database.getReturnCode() == SQLITE_ROW) {
        // Récupérer le nom de l'ami
        const unsigned char* ami = database.getTextValueFromColumn(0);
        friendsList.push_back(reinterpret_cast<const char*>(ami));  // Ajouter l'ami au vecteur
        database.execute();
    }

    // Libérer la mémoire après exécution
    database.destructStatement();  // Libération de la mémoire après exécution

    return friendsList;  // Retourner le vecteur des amis
}

std::vector<std::string> UserDatabaseManager::getSentRequestsList(const std::string& username){
    std::vector<std::string> friendsList;

    // Préparer la requête SQL pour récupérer les amis de l'utilisateur
    std::string sql = database.getStatement("getSentRequestsList");

    // Préparer la déclaration
    database.prepareStatement(sql.c_str());

    // Binder le nom de l'utilisateur (par exemple, "David")
    database.bindText(username.c_str(),1);

    database.execute();

    // Exécuter la requête et récupérer les résultats ligne par ligne
    while (database.getReturnCode() == SQLITE_ROW) {
        // Récupérer le nom de l'ami
        const unsigned char* ami = database.getTextValueFromColumn(0);
        friendsList.push_back(reinterpret_cast<const char*>(ami));  // Ajouter l'ami au vecteur
        database.execute();
    }

    // Libérer la mémoire après exécution
    database.destructStatement();  // Libération de la mémoire après exécution

    return friendsList;  // Retourner le vecteur des amis
}


void UserDatabaseManager::conversationOpen(int user_id, int friend_id){
    std::string sql = database.getStatement("openConversation");

    database.prepareStatement(sql.c_str());
    database.bindInt(user_id, 1);
    database.bindInt(friend_id, 2);
    database.execute();
    database.destructStatement();
}

void UserDatabaseManager::conversationClose(int user_id, int friend_id){
    std::string sql = database.getStatement("closeConversation");

    database.prepareStatement(sql.c_str());
    database.bindInt(user_id, 1);
    database.bindInt(friend_id, 2);
    database.execute();
    database.destructStatement();
}

int UserDatabaseManager::getInConversation(int user_id, int friend_id){
    int in_conversation = -1;
    std::string sql = database.getStatement("getInConversation");

    database.prepareStatement(sql.c_str());
    database.bindInt(friend_id, 1);
    database.bindInt(user_id, 2);
    database.execute();

    // Vérifier s'il y a un résultat
    if (database.getReturnCode() == SQLITE_ROW) {
        in_conversation = database.getIntValueFromColumn(0);
    }

    database.destructStatement();
    return in_conversation;
}

void UserDatabaseManager::addMessage(int sender_id, int receiver_id, const std::string& message) {
    std::string sql = database.getStatement("addMessage");

    database.prepareStatement(sql.c_str());
    database.bindInt(sender_id, 1);
    database.bindInt(receiver_id, 2);
    database.bindText(message.c_str(), 3);
    database.execute();
    database.destructStatement();
}

void UserDatabaseManager::deleteMessages(int sender_id, int receiver_id) {
    std::string sql = database.getStatement("deleteMessages");

    database.prepareStatement(sql.c_str());
    database.bindInt(sender_id, 1);
    database.bindInt(receiver_id, 2);
    database.execute();
    database.destructStatement();
}

std::vector<std::string> UserDatabaseManager::getMessages(int sender_id, int receiver_id) {
    std::vector<std::string> messages;
    std::string sql = database.getStatement("getMessagesNoRead");

    database.prepareStatement(sql.c_str());

    database.bindInt(sender_id, 1);
    database.bindInt(receiver_id, 2);

    database.execute();

    while (database.getReturnCode() == SQLITE_ROW) {
        const char* message = reinterpret_cast<const char*>(database.getTextValueFromColumn(0));  // Récupérer le message
        messages.push_back(message);  // Ajouter le message au vecteur (au cas où il serait NULL)
        database.execute();
    }

    database.destructStatement();

    return messages;
}


int UserDatabaseManager::getNumberMessageNoRead(int user_id, int friend_id){
    int numberRequests = 0;
    std::string sql = database.getStatement("getNumberMessageNoRead");

    database.prepareStatement(sql.c_str());
    database.bindInt(friend_id,1);
    database.bindInt(user_id,2);
    database.execute();

    if(database.getReturnCode() == SQLITE_ROW){
        numberRequests = database.getIntValueFromColumn(0);
    }
    database.destructStatement();

    return numberRequests;
}

void UserDatabaseManager::setNotInConversation(int user_id){
    std::string sql = database.getStatement("setNotInConversation");
    database.prepareStatement(sql.c_str());
    database.bindInt(user_id,1);

    database.execute();
    database.destructStatement();
}

bool UserDatabaseManager::isUserInSession(int user_id) {

    std::string sql = database.getStatement("isUserInSession");

    database.prepareStatement(sql.c_str());
	database.bindInt(user_id,1);

    database.execute();
    bool exists = (database.getReturnCode() == SQLITE_ROW);
    database.destructStatement();

    return exists;
}

void UserDatabaseManager::deleteSession(int user_id){

    std::string sql = database.getStatement("deleteSession");
    database.prepareStatement(sql.c_str());
    database.bindInt(user_id,1);

    database.execute();
    database.destructStatement();
}

std::vector<int> UserDatabaseManager::getUserIdSession() {
    std::vector<int> userIds;
    std::string sql = database.getStatement("getUserIdSession");

    database.prepareStatement(sql.c_str());
    database.execute();

    while (database.getReturnCode() == SQLITE_ROW) {
        int userId = database.getIntValueFromColumn(0);
        userIds.push_back(userId);
        database.execute();
    }

    database.destructStatement();

    return userIds;
}

int UserDatabaseManager::getUserSocket(int user_id){
    int sockUser = -1;
    std::string sql = database.getStatement("getUserSocket");

    database.prepareStatement(sql.c_str());
    database.bindInt(user_id, 1);
    database.execute();

    // Vérifier s'il y a un résultat
    if (database.getReturnCode() == SQLITE_ROW) {
        sockUser = database.getIntValueFromColumn(0);
    }

    database.destructStatement();
    return sockUser;
}