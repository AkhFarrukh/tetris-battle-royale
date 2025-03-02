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
        "CREATE TABLE IF NOT EXISTS utilisateurs (id INTEGER PRIMARY KEY, nom TEXT UNIQUE);";
        prepareStatement(sql);
        execute();
        sql = "CREATE TABLE IF NOT EXISTS relations (utilisateur_id INTEGER, ami_id INTEGER, FOREIGN KEY(utilisateur_id) REFERENCES utilisateurs(id), FOREIGN KEY(ami_id) REFERENCES utilisateurs(id), PRIMARY KEY (utilisateur_id, ami_id));";
        prepareStatement(sql);
        execute();
        sql = "CREATE TABLE IF NOT EXISTS friendsRequest (utilisateur_id INTEGER, ami_id INTEGER, FOREIGN KEY(utilisateur_id) REFERENCES utilisateurs(id), FOREIGN KEY(ami_id) REFERENCES utilisateurs(id), PRIMARY KEY (utilisateur_id, ami_id));";
        prepareStatement(sql);
        execute();
    sql ="DELETE FROM utilisateurs WHERE nom = 'Luc';";
    prepareStatement(sql);
    execute();
    sql ="DELETE FROM friendsRequest WHERE utilisateur_id = (SELECT id FROM utilisateurs WHERE nom = 'Jérôme') OR ami_id = (SELECT id FROM utilisateurs WHERE nom = 'Jérôme');";
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

vector<string> Database::friendsList(const string& username){
    std::vector<std::string> friendsList;

    // Préparer la requête SQL pour récupérer les amis de l'utilisateur
    const char* sql =
        "SELECT u.nom FROM utilisateurs u "
        "JOIN relations r ON u.id = r.ami_id "
        "WHERE r.utilisateur_id = (SELECT id FROM utilisateurs WHERE nom = ?);";

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
    "DELETE FROM relations WHERE "
    "(utilisateur_id = (SELECT id FROM utilisateurs WHERE nom = '" + user + "') "
    "AND ami_id = (SELECT id FROM utilisateurs WHERE nom = '" + friendDelete + "')) "
    "OR "
    "(utilisateur_id = (SELECT id FROM utilisateurs WHERE nom = '" + friendDelete + "') "
    "AND ami_id = (SELECT id FROM utilisateurs WHERE nom = '" + user+ "'));";
    prepareStatement(sql.c_str());
    execute();
}

void Database::addFriend(const string& user, const string& newFriend){
    std::string sql =
      "INSERT INTO relations (utilisateur_id, ami_id) VALUES "
      "((SELECT id FROM utilisateurs WHERE nom = '" + user + "'), "
      "(SELECT id FROM utilisateurs WHERE nom = '" + newFriend + "')), "
      "((SELECT id FROM utilisateurs WHERE nom = '" + newFriend + "'), "
      "(SELECT id FROM utilisateurs WHERE nom = '" + user + "'));";
    prepareStatement(sql.c_str());
    execute();
}

int Database::verifyRequestFriend(const string& user, const string& newFriend){
    int result = -1; //erreur pas prise en charge
    std::string sql =
    "SELECT CASE " //fonctionne comme if-else
    "WHEN NOT EXISTS (SELECT 1 FROM utilisateurs WHERE nom = '" + newFriend + "') THEN 1 " //return 1 si pseude inconnnu
    "WHEN EXISTS (SELECT 1 FROM relations r "
    "             JOIN utilisateurs u1 ON u1.id = r.utilisateur_id "
    "             JOIN utilisateurs u2 ON u2.id = r.ami_id "
    "             WHERE u1.nom = '" + user + "' AND u2.nom = '" + newFriend + "') THEN 2 " //ret 2 si deja amis
    "WHEN EXISTS (SELECT 1 FROM friendsRequest fr "
    "             JOIN utilisateurs u1 ON u1.id = fr.utilisateur_id "
    "             JOIN utilisateurs u2 ON u2.id = fr.ami_id "
    "             WHERE u1.nom = '" + user + "' AND u2.nom = '" + newFriend + "') THEN 3 " //dmd d'amis deja envoyé
    "WHEN EXISTS (SELECT 1 FROM friendsRequest fr "
    "             JOIN utilisateurs u1 ON u1.id = fr.ami_id "
    "             JOIN utilisateurs u2 ON u2.id = fr.utilisateur_id "
    "             WHERE u1.nom = '" + user + "' AND u2.nom = '" + newFriend + "') THEN 4 " //l'autre a deja dmd en ami
    "ELSE 0 END;";
    prepareStatement(sql.c_str());
    execute();
    if(getReturnCode() == SQLITE_ROW){
      result = getIntValueFromColumn(0);
    }
    destructStatement();
    return result;
}

void Database::requestFriend(const string& user, const string& newFriend){
    int result = -1; //erreur pas prise en charge
    std::string sql = "INSERT INTO friendsRequest (utilisateur_id, ami_id) VALUES ((SELECT id FROM utilisateurs WHERE nom = '" + user + "'), (SELECT id FROM utilisateurs WHERE nom = '" + newFriend + "'));";
    prepareStatement(sql.c_str());
    execute();
}

void Database::deleteRequestFriend(const string& user, const string& friendDelete){
    std::string sql =
    "DELETE FROM friendsRequest WHERE "
    "(utilisateur_id = (SELECT id FROM utilisateurs WHERE nom = '" + user + "') "
    "AND ami_id = (SELECT id FROM utilisateurs WHERE nom = '" + friendDelete + "')) "
    "OR "
    "(utilisateur_id = (SELECT id FROM utilisateurs WHERE nom = '" + friendDelete + "') "
    "AND ami_id = (SELECT id FROM utilisateurs WHERE nom = '" + user+ "'));";
    prepareStatement(sql.c_str());
    execute();
}

int Database::getNumberFriendRequestsReceived(const string& user){
    int numberRequests = 0;
    std::string sql =
    "SELECT COUNT(*) FROM friendsRequest "
    "WHERE ami_id = (SELECT id FROM utilisateurs WHERE nom = '" + user + "');";
    prepareStatement(sql.c_str());
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
        "SELECT u.nom FROM utilisateurs u "
        "JOIN friendsRequest r ON u.id = r.utilisateur_id "
        "WHERE r.ami_id = (SELECT id FROM utilisateurs WHERE nom = ?);";

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
        "SELECT u.nom FROM utilisateurs u "
        "JOIN friendsRequest r ON u.id = r.ami_id "
        "WHERE r.utilisateur_id = (SELECT id FROM utilisateurs WHERE nom = ?);";

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