#include "serverFunctions.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <cstring>

using namespace std;

vector<int> clients;

void broadcastMessage(const string& message, int sender_fd) {
    for (int client_fd : clients) {
        if (client_fd != sender_fd) {
            send(client_fd, message.c_str(), message.length(), 0);
        }
    }
}

int handleClient(int server_fd, struct sockaddr_in& address, size_t addressLen) {
    while (true) {
        int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addressLen);
        if (client_fd < 0) {
            perror("Erreur lors de l'acceptation de la connexion");
            return -1;
        }
        cout << "Nouveau client connecté : " << client_fd << endl;
        clients.push_back(client_fd);
        
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
    }
    return 0;
}

