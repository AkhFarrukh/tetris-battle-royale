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

using namespace std;

vector<int> clients;

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
        clients.push_back(client_fd);
        
        /* Créé un thread pour le client */
        pthread_t threadID;
        if((pthread_create(&threadID, nullptr, threadRoutineClient, nullptr)) < 0){
            perror("Error on thread creation : ");
            return -1;
            //Maybe need to close fd, idk
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
    /* Il faudrait passer le username en arg pour pouvoir récup l'ensemble de ses données sur la db je pense ?*/
    string* userName = static_cast<string*>(arg);

    /* Une fois l'arg lu, on l'unlock et notify outside the thread */
    lockThread = true;
    cv.notify_one();

    char buffer[1024] = {0};
    while (true) {
        int bytesReceived = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client déconnecté : " << client_fd << endl;
            close(client_fd);
            erase(clients, client_fd);
            break;
        }
        string message(buffer, bytesReceived);
        cout << "Message reçu de " << client_fd << " : " << message << endl;
        broadcastMessage(message, client_fd);
    }
    return nullptr;
}

