#include "serverFunctions.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <mutex>                //Concurent access
#include <condition_variable>   //Thread sync
#include <pthread.h>            //Threads
#include <sqlite3.h>
#include <bcrypt/BCrypt.hpp>

using namespace std;

struct ClientSession{
  int client_fd;
  string username;
  string password;
};

vector<ClientSession> clients;

//Synchro management
mutex mtx;
condition_variable cv;
bool lockThread;


void broadcastMessage(const string& message, int sender_fd) {
    
    for (int client_fd : clients) {
        if (client_fd != sender_fd) {
            send(client_fd, message.c_str(), message.length(), 0);
        }
    }
}

/**
 * @brief Boucle qui handle le client, lui assigne un port et un thread
 * 
 * @param server_fd socket du server
 * @param address adresse du server
 * @param addressLen adress length
 * @return int, <= -1 si prob
 */
int handleClient(int server_fd, struct sockaddr_in& address, size_t addressLen) {
    while (true) {
        /* Mutex */
        unique_lock<mutex> lock(mtx);
        lockThread = false;
        /* Accept et assigne un fd au nouveau client */
        int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addressLen);
        if (client_fd < 0) {
            perror("Erreur lors de l'acceptation de la connexion");
            return -1;
        }
        cout << "Nouveau client connecté : " << client_fd << endl;

        // Le mot de passe envoyé doit déjà être un hash bcrypte (l'implemntation de pao)
        char username[256] = {0};
        char password[256] = {0};

        if (recv(client_fd, username, sizeof(username), 0) <= 0 ||
            recv(client_fd, password, sizeof(password), 0) <= 0) {
            perror("Erreur lors de la réception des credentials");
            close(client_fd);
            continue;
        }

        ClientSession new_session;
        new_session.client_fd = client_fd;
        new_session.username = string(username);
        new_session.password = string(password);

        clients.push_back(new_session);
        ClientSession* client = new ClientSession(new_session);  
        
        /* Créé un thread pour le client */
        pthread_t threadID;
        if((pthread_create(&threadID, nullptr, threadRoutineClient, client)) < 0){
            perror("Error on thread creation : ");
            close(client_fd);
            delete client;
            return -1;
        }
        /* Unlock mutex when got the signal from inside the thread */
        cv.wait(lock, []{return lockThread;});
        cout << "Thread successfully created !" << endl;
        lock.unlock();
    }
    return 0;
}

/**
 * @brief Routine du thread client, attends les events de celui-ci et lui communique des infos en fonction
 * 
 * @param arg contient l'username du client
 * @return void* 
 */
void* threadRoutineClient(void* arg){
    cout << "Starting thread routine for client" << endl;
    /* Il faudrait passer le username en arg pour pouvoir récup l'ensemble de ses données sur la db je pense ?*
     */
   // string* userName = static_cast<string*>(arg);

    ClientSession* client = static_cast<ClientSession*>(arg);
    if(login_logic(*client)){
      insertSession(client->username, client->client_fd);
      /* Une fois l'arg lu, on l'unlock et notify outside the thread */
      lockThread = true;
      cv.notify_one();

      char buffer[1024] = {0};
      while (true) {
        int bytesReceived = recv(client->client_fd, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client déconnecté : " << client->client_fd << endl;
            removeSession(client->client_fd);
            close(client->client_fd);
            clients.erase(remove_if(clients.begin(), clients.end(),
                                    [client](const ClientSession& c) { return c.client_fd == client->client_fd; }),
                          clients.end());
            break;
        }
        string message(buffer, bytesReceived);
        cout << "Message reçu de " << client->username << " : " << message << endl;
        broadcastMessage(message, client->client_fd);
      }
    }else{
      cerr << "login failed";
    }
    return nullptr;
}


void initDatabase() {
    if (sqlite3_open("../../tetris.db", &db) != SQLITE_OK) {
        cerr << "Erreur lors de l'ouverture de la db: " << sqlite3_errmsg(db) << std::endl;
    } else {
        cout << "DB SQLite connectée !" << std::endl;
    }
}

bool login(ClientSession client) {
    std::string sql = "SELECT password FROM login WHERE username = ?;";
    sqlite3_stmt* stmt;
    std::string storedHash;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, client.username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    if (storedHash.empty()) {
        //Utilisateur non trouvé
        return false;
    }
    if (BCrypt::validatePassword(client.password, storedHash)) {
        std::cout << "Connexion réussie pour " << client.username << std::endl;
        return true;
    } else {
        return false;
    }
}


void insertSession(const std::string& username, int client_fd) {
    std::string sql = "INSERT INTO sessions (username, client_fd) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, client_fd);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            std::cout << "Session ajoutée : " << username << " pour fd: " << client_fd << "" << std::endl;
        } else {
            std::cerr << "Erreur lors de l'insertion de la session." << std::endl;
        }
        sqlite3_finalize(stmt);
    }
}

void removeSession(int client_fd) {
    std::string sql = "DELETE FROM sessions WHERE client_fd = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, client_fd);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            std::cout << "Session supprimée pour fd: " << client_fd << std::endl;
        } else {
            std::cerr << "Erreur lors de la suppression de la session." << std::endl;
        }
        sqlite3_finalize(stmt);
    }
}

std::string getUsernameFromFd(int client_fd) {
    std::string sql = "SELECT username FROM sessions WHERE client_fd = ?;";
    sqlite3_stmt* stmt;
    std::string username = "";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, client_fd);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return username;
}
