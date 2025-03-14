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
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT, logged_in INTEGER DEFAULT 0, receiving_file_descriptor INTEGER DEFAULT 0, sending_file_descriptor INTEGER DEFAULT 0);";
        prepareStatement(sql);
        execute();
        sql = "CREATE TABLE IF NOT EXISTS friends (user_id INTEGER, friend_id INTEGER, in_conversation INTEGER DEFAULT 0, FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE, FOREIGN KEY(friend_id) REFERENCES users(id) ON DELETE CASCADE, PRIMARY KEY (user_id, friend_id));";
        prepareStatement(sql);
        execute();
        sql = "CREATE TABLE IF NOT EXISTS friendsRequest (user_id INTEGER, friend_id INTEGER, FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE, FOREIGN KEY(friend_id) REFERENCES users(id) ON DELETE CASCADE, PRIMARY KEY (user_id, friend_id));";
        prepareStatement(sql);
        execute();
        sql = "CREATE TABLE IF NOT EXISTS sessions (user_id INTEGER, client_fd INTEGER, FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE, PRIMARY KEY (user_id));";
        prepareStatement(sql);
        execute();
        sql = "CREATE TABLE IF NOT EXISTS MessageNoRead (message_id INTEGER PRIMARY KEY AUTOINCREMENT, sender_id INTEGER, receiver_id INTEGER, message TEXT, FOREIGN KEY(sender_id) REFERENCES users(id) ON DELETE CASCADE, FOREIGN KEY(receiver_id) REFERENCES users(id) ON DELETE CASCADE);";
        prepareStatement(sql);
        execute();
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
    int returnCode = sqlite3_prepare_v2(db, statement, static_cast<int>(strlen(statement)), &stmt, nullptr);
    if (returnCode != SQLITE_OK)  fprintf(stderr, "ERROR PREPARING STATEMENT!");

}


/**
 * @brief bind a TEXT value to a statement (&stmt)
 * @param value: a value to be binded
 * @param index: a column index (starting from 1 (not zero!) from the most left)
 */
void Database::bindText(const char* value, int index){
    int returnCode = sqlite3_bind_text(stmt, index, value, static_cast<int>(strlen(value)), SQLITE_STATIC);
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


int Database::getUserId(const char* username){
    const char* begSql = "BEGIN TRANSACTION;";
    prepareStatement(begSql);
    execute();
    destructStatement();
    int userId;
    // Prepare Statement and bind username to "?" symbols in a statement
    const char* sql = "SELECT id FROM users WHERE username = ?";

    prepareStatement(sql);
    bindText(username, 1);
    execute();

    // If there is a result, get the user ID. Else return -1
    if (getReturnCode() == SQLITE_ROW){
        userId = getIntValueFromColumn(0);
    }
    else{
        userId = -1;
    }

    // Destruct the statement after getting the result then return a result
    destructStatement();

    const char* commit = "COMMIT;";
    prepareStatement(commit);
    execute();
    destructStatement();

    return userId;
}


vector<string> Database::friendsList(const string& username){
    std::vector<std::string> friendsList;

    // Préparer la requête SQL pour récupérer les amis de l'utilisateur
    const char* sql =
        "SELECT u.username FROM users u "
        "JOIN friends r ON u.id = r.friend_id "
        "WHERE r.user_id = (SELECT id FROM users WHERE username = ?);";

    // Préparer la déclaration
    prepareStatement(sql);

    // Binder le nom de l'utilisateur (par exemple, "David")
    bindText(username.c_str(),1);

    execute();
    // Exécuter la requête et récupérer les résultats ligne par ligne
    while (getReturnCode() == SQLITE_ROW) {
        // Récupérer le nom de l'ami
        const unsigned char* ami = getTextValueFromColumn(0);
        friendsList.push_back(reinterpret_cast<const char*>(ami));  // Ajouter l'ami au vecteur
        execute();
    }

    // Libérer la mémoire après exécution
    destructStatement();  // Libération de la mémoire après exécution

    return friendsList;  // Retourner le vecteur des amis
}

void Database::deleteFriend(const string& user, const string& friendDelete){
    std::string sql =
    "DELETE FROM friends WHERE "
    "(user_id = (SELECT id FROM users WHERE username = '" + user + "') "
    "AND friend_id = (SELECT id FROM users WHERE username = '" + friendDelete + "')) "
    "OR "
    "(user_id = (SELECT id FROM users WHERE username = '" + friendDelete + "') "
    "AND friend_id = (SELECT id FROM users WHERE username = '" + user+ "'));";
    prepareStatement(sql.c_str());
    execute();
}

void Database::addFriend(const string& user, const string& newFriend){
    std::string sql =
      "INSERT INTO friends (user_id, friend_id) VALUES "
      "((SELECT id FROM users WHERE username = '" + user + "'), "
      "(SELECT id FROM users WHERE username = '" + newFriend + "')), "
      "((SELECT id FROM users WHERE username = '" + newFriend + "'), "
      "(SELECT id FROM users WHERE username = '" + user + "'));";
    prepareStatement(sql.c_str());
    execute();
}

int Database::verifyRequestFriend(const string& user, const string& newFriend){
    int result = -1; //erreur pas prise en charge
    std::string sql =
    "SELECT CASE " //fonctionne comme if-else
    "WHEN NOT EXISTS (SELECT 1 FROM users WHERE username = '" + newFriend + "') THEN 1 " //return 1 si pseude inconnnu
    "WHEN EXISTS (SELECT 1 FROM friends r "
    "             JOIN users u1 ON u1.id = r.user_id "
    "             JOIN users u2 ON u2.id = r.friend_id "
    "             WHERE u1.username = '" + user + "' AND u2.username = '" + newFriend + "') THEN 2 " //ret 2 si deja amis
    "WHEN EXISTS (SELECT 1 FROM friendsRequest fr "
    "             JOIN users u1 ON u1.id = fr.user_id "
    "             JOIN users u2 ON u2.id = fr.friend_id "
    "             WHERE u1.username = '" + user + "' AND u2.username = '" + newFriend + "') THEN 3 " //dmd d'amis deja envoyé
    "WHEN EXISTS (SELECT 1 FROM friendsRequest fr "
    "             JOIN users u1 ON u1.id = fr.friend_id "
    "             JOIN users u2 ON u2.id = fr.user_id "
    "             WHERE u1.username = '" + user + "' AND u2.username = '" + newFriend + "') THEN 4 " //l'autre a deja dmd en ami
    "ELSE 0 END;";
    prepareStatement(sql.c_str());
    execute();
    if(getReturnCode() == SQLITE_ROW){
      result = getIntValueFromColumn(0);
    }
    destructStatement();
    return result;
}

int Database::requestFriend(const string& user, const string& newFriend){
    std::string sql = "INSERT INTO friendsRequest (user_id, friend_id) VALUES ((SELECT id FROM users WHERE username = '" + user + "'), (SELECT id FROM users WHERE username = '" + newFriend + "'));";
    prepareStatement(sql.c_str());
    execute();
    return(getReturnCode() != SQLITE_ERROR);
}

void Database::deleteRequestFriend(const string& user, const string& friendDelete){
    std::string sql =
    "DELETE FROM friendsRequest WHERE "
    "(user_id = (SELECT id FROM users WHERE username = '" + user + "') "
    "AND friend_id = (SELECT id FROM users WHERE username = '" + friendDelete + "')) "
    "OR "
    "(user_id = (SELECT id FROM users WHERE username = '" + friendDelete + "') "
    "AND friend_id = (SELECT id FROM users WHERE username = '" + user+ "'));";
    prepareStatement(sql.c_str());
    execute();
}

int Database::getNumberFriendRequestsReceived(const string& user){
    int numberRequests = 0;
    std::string sql =
    "SELECT COUNT(*) FROM friendsRequest "
    "WHERE friend_id = (SELECT id FROM users WHERE username = ?);";
    prepareStatement(sql.c_str());
    bindText(user.c_str(),1);
    execute();
    if(getReturnCode() == SQLITE_ROW){
        numberRequests = getIntValueFromColumn(0);
    }
    destructStatement();
    return numberRequests;
}

vector<string> Database::requestReceivedList(const string& username){
    std::vector<std::string> friendsList;

    // Préparer la requête SQL pour récupérer les amis de l'utilisateur
    const char* sql =
        "SELECT u.username FROM users u "
        "JOIN friendsRequest r ON u.id = r.user_id "
        "WHERE r.friend_id = (SELECT id FROM users WHERE username = ?);";

    // Préparer la déclaration
    prepareStatement(sql);

    // Binder le nom de l'utilisateur (par exemple, "David")
    bindText(username.c_str(),1);

    execute();
    // Exécuter la requête et récupérer les résultats ligne par ligne
    while (getReturnCode() == SQLITE_ROW) {
        // Récupérer le nom de l'ami
        const unsigned char* ami = getTextValueFromColumn(0);
        friendsList.push_back(reinterpret_cast<const char*>(ami));  // Ajouter l'ami au vecteur
        execute();
    }

    // Libérer la mémoire après exécution
    destructStatement();  // Libération de la mémoire après exécution

    return friendsList;  // Retourner le vecteur des amis
}

vector<string> Database::requestSentList(const string& username){
    std::vector<std::string> friendsList;

    // Préparer la requête SQL pour récupérer les amis de l'utilisateur
    const char* sql =
        "SELECT u.username FROM users u "
        "JOIN friendsRequest r ON u.id = r.friend_id "
        "WHERE r.user_id = (SELECT id FROM users WHERE username = ?);";

    // Préparer la déclaration
    prepareStatement(sql);

    // Binder le nom de l'utilisateur (par exemple, "David")
    bindText(username.c_str(),1);

    execute();
    // Exécuter la requête et récupérer les résultats ligne par ligne
    while (getReturnCode() == SQLITE_ROW) {
        // Récupérer le nom de l'ami
        const unsigned char* ami = getTextValueFromColumn(0);
        friendsList.push_back(reinterpret_cast<const char*>(ami));  // Ajouter l'ami au vecteur
        execute();
    }

    // Libérer la mémoire après exécution
    destructStatement();  // Libération de la mémoire après exécution

    return friendsList;  // Retourner le vecteur des amis
}


void Database::conversationOpen(int user_id, int friend_id){
    std::string sql = "UPDATE friends SET in_conversation = "
        "CASE WHEN in_conversation = 0 THEN 1 "
        "ELSE in_conversation END "
        "WHERE (user_id = ? AND friend_id = ?);";

    prepareStatement(sql.c_str());
    bindInt(user_id, 1);
    bindInt(friend_id, 2);
    execute();
    destructStatement();
}

void Database::conversationClose(int user_id, int friend_id){
    std::string sql = "UPDATE friends SET in_conversation = "
        "CASE WHEN in_conversation = 1 THEN 0 "
        "ELSE in_conversation END "
        "WHERE (user_id = ? AND friend_id = ?);";

    prepareStatement(sql.c_str());
    bindInt(user_id, 1);
    bindInt(friend_id, 2);
    execute();
    destructStatement();
}

int Database::getInConversation(int user_id, int friend_id){
    int in_conversation = -1;
    std::string sql =
        "SELECT in_conversation FROM friends "
        "WHERE (user_id = ? AND friend_id = ?);";

    prepareStatement(sql.c_str());
    bindInt(friend_id, 1);
    bindInt(user_id, 2);
    execute();

    // Vérifier s'il y a un résultat
    if (getReturnCode() == SQLITE_ROW) {
        in_conversation = getIntValueFromColumn(0);
    }

    destructStatement();
    return in_conversation;
}

void Database::addMessage(int sender_id, int receiver_id, const std::string& message) {
    std::string sql = "INSERT INTO MessageNoRead (sender_id, receiver_id, message) "
                      "VALUES (?, ?, ?);";

    prepareStatement(sql.c_str());
    bindInt(sender_id, 1);
    bindInt(receiver_id, 2);
    bindText(message.c_str(), 3);
    execute();
    destructStatement();
}

void Database::deleteMessages(int sender_id, int receiver_id) {
    std::string sql = "DELETE FROM MessageNoRead "
                      "WHERE sender_id = ? AND receiver_id = ?;";

    prepareStatement(sql.c_str());
    bindInt(sender_id, 1);
    bindInt(receiver_id, 2);
    execute();
    destructStatement();
}

std::vector<std::string> Database::getMessages(int sender_id, int receiver_id) {
    std::vector<std::string> messages;
    std::string sql = "SELECT message FROM MessageNoRead "
                      "WHERE sender_id = ? AND receiver_id = ?;";


    prepareStatement(sql.c_str());

    bindInt(sender_id, 1);
    bindInt(receiver_id, 2);

    execute();

    while (getReturnCode() == SQLITE_ROW) {
        const char* message = reinterpret_cast<const char*>(getTextValueFromColumn(0));  // Récupérer le message
        messages.push_back(message);  // Ajouter le message au vecteur (au cas où il serait NULL)
        execute();
    }

    destructStatement();

    return messages;
}


int Database::getNumberMessageNoRead(int user_id, int friend_id){
    int numberRequests = 0;
    std::string sql =
    "SELECT COUNT(*) FROM MessageNoRead "
    "WHERE sender_id = ? AND receiver_id = ?;";
    prepareStatement(sql.c_str());
    bindInt(friend_id,1);
    bindInt(user_id,2);
    execute();
    if(getReturnCode() == SQLITE_ROW){
        numberRequests = getIntValueFromColumn(0);
    }
    destructStatement();
    return numberRequests;
}

void Database::setNotInConversation(int user_id){
    std::string sql =
    "UPDATE friends SET in_conversation = 0 WHERE user_id = ?;";
    prepareStatement(sql.c_str());
    bindInt(user_id,1);
    execute();
    destructStatement();
}

bool Database::isUserInSession(int user_id) {
  const char* begSql = "BEGIN TRANSACTION;";
    prepareStatement(begSql);
    execute();
    destructStatement();

    const char* sql = "SELECT 1 FROM sessions WHERE user_id = ? LIMIT 1;";

    prepareStatement(sql);
	bindInt(user_id,1);

    execute();
    bool exists = (getReturnCode() == SQLITE_ROW);
    destructStatement();

    const char* commit = "COMMIT;";
    prepareStatement(commit);
    execute();
    destructStatement();

    return exists;
}

void Database::deleteSession(int user_id){
    const char* begSql = "BEGIN TRANSACTION;";
    prepareStatement(begSql);
    execute();
    destructStatement();

    const char* sql = "DELETE FROM sessions WHERE user_id = ?;";
    prepareStatement(sql);
    bindInt(user_id,1);
    execute();
    destructStatement();

    const char* commit = "COMMIT;";
    prepareStatement(commit);
    execute();
    destructStatement();
}
