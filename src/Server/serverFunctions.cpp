#include "serverFunctions.hpp"

/**
 * TODO: COMMENTS TO BE ADDED BY THOSE WHO WROTE THIS FUNCTION/METHOD. STP
 */
void Server::broadcastMessage(const std::string& message, int sender_fd) {
    for (const auto client : clients) {
        if (client.client_fd != sender_fd) {
            send(client.client_fd, message.c_str(), message.length(), 0);
        }
    }
}



/**
 * @brief A loop which handles a client and assign to them their port and thread
 * 
 * @param server_fd Server socket
 * @param address Server address
 * @param addressLen Address length
 * @return return -1 in case of error else return 0
 */
int Server::handleClient(int server_fd, struct sockaddr_in& address, size_t addressLen) {
    while (true) {

        // Mutex
        std::unique_lock<std::mutex> lock(mtx);
        lockThread = false;

        // Accept and assign a file descrpitor to new client
        int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addressLen);
        if (client_fd < 0) {
            perror("Erreur lors de l'acceptation de la connexion");
            return -1;
        }
        std::cout << "Nouveau client connecté : " << client_fd << std::endl;


        // Create a "new session" with client info to be sent to thread routine
        // and push the client into a vector of clients
        ClientSession new_session;
        new_session.client_fd = client_fd;
        clients.push_back(new_session);

        // Create a smart pointer containing data to be passed into thread routine.
        std::shared_ptr<ThreadData> threadData = std::make_shared<ThreadData>();
        threadData->client = ClientSession(new_session);
        threadData->serverInstance = this; // "this" = a pointer of server object
        
        // Create a thread for a user if the username and password is correct.
        pthread_t threadID;
        if(pthread_create(&threadID, nullptr, threadRoutineClient, threadData.get()) < 0){
            perror("Error on thread creation : ");
            close(client_fd);
            return -1;
        }
        
        // Unlock mutex when got the signal from inside the thread
        cv.wait(lock, [this]{return lockThread;});
        std::cout << "Thread successfully created !" << std::endl;
        lock.unlock();
    }
    return 0;
}


/**
 * @brief Routine du thread client, attends les events de celui-ci et lui communique 
 * des infos en fonction
 * @param arg has the client info and the server pointer
 * @return void* 
 * @paragraph it's a STATIC method but in .cpp file we don't need to write "static" 
 * again in declaration because we already declare it "static" in .hpp.
 */
void* Server::threadRoutineClient(void* arg){
    std::cout << "Starting thread routine for client" << std::endl;
    
    // Get thread data from arg

    shared_ptr<EventQueue> eventQueue = make_shared<EventQueue>();
    ThreadData* threadData = reinterpret_cast<ThreadData*>(arg);
    ThreadGameData threadGameData;
    GameController gameController(eventQueue);
    Server* server = threadData->serverInstance;
    ClientSession& client = threadData->client;

    // Check if both username and password are correct or not.
    bool successfulLogin = true;

    if (successfulLogin){
        // Get user ID and insert user into sessions table
        client.userId = server->udbm.getUserId(client.username.c_str());
        server->udbm.addSession(client.userId, client.client_fd);

        /* Unlock the mutex and notifies it outside the thread */
        server->lockThread = true;
        server->cv.notify_one();
        
        // Keep looking for any message sent by the user to the server
        // Using bool "UserIsOnline" for better readability of while loop
        char buffer[1024] = {0};
        bool userIsOnline = true;



        while (userIsOnline) {
            std::cout << "start listening cuh" << std::endl;
            //int bytesReceived = recv(client.client_fd, buffer, sizeof(buffer), 0);

            //To have info on the data received
            int type;
            int size;
            

            //Sending info to server
            int bytesReceivedType = recv (client.client_fd, &type, sizeof(int), 0);
            int bytesReceivedSize = recv (client.client_fd, &size, sizeof(int), 0);
            std::cout << "fdp ma lumiere : " << type << std::endl;
            std::cout << "fdp ma lumiere : " << size << std::endl;
            
            //Client disconnected or error
            if (bytesReceivedType <= 0 || bytesReceivedSize <= 0) {
                std::cout << "Client déconnecté : " << client.client_fd << std::endl;
                server->udbm.removeSession(client.userId);
                close(client.client_fd);
                userIsOnline = false; // Stop the loop
            }
            

            else if(type == MessageType::T_Menu){
                if (size != sizeof(Menu)) {
                    std::cout << "fdp ma lumiere : " << client.client_fd << std::endl;
                }
                std::cout << "Menut" << std::endl;
                Menu menu;
                if (!recvAll(client.client_fd, &menu, sizeof(Menu))){
                    //Repetition de code, pas ouf, need method
                    std::cout << "Client déconnecté : " << client.client_fd << std::endl;
                    server->udbm.removeSession(client.userId);
                    close(client.client_fd);
                    userIsOnline = false; // Stop the loop
                }

                if (menu.action == MenuActions::A_Play) {
                    if (menu.mode == Modes::Endless) {
                        
                        std::cout << "Client wants Endless mode\n";

                        threadGameData.client_fd = client.client_fd;
                        threadGameData.eventQueue = eventQueue;
                        std::vector<ThreadGameData> data;
                        data.push_back(threadGameData);
                        
                        std::shared_ptr<Game> soloGame = std::make_shared<Game>(data, 1, "Endless");

                        std::thread gameThread([soloGame]() {
                            soloGame->run();
                        });
                        gameThread.detach();
                    }
                    else if (menu.mode == Modes::Classic) {
                        std::cout << "Client wants Classic mode\n";
                        // ...
                    }
                    else {
                        std::cout << "Unknown game mode: " << menu.mode << "\n";
                    }
                }
                
            }
            else if (type == MessageType::T_Input) {

                KeyboardAction action;

                if (!recvAll(client.client_fd, &action, sizeof(KeyboardAction))) {
                    std::cout << "Client disconnected while reading Input\n";
                    close(client.client_fd);
                    userIsOnline = false;
                    break;
                }
                
                gameController.getAction(action);
            }
            
            // Display message if there's one.
            /* ? what's the purpose of these lines, why send back to client a message, + message isn't always/almost never a string meant to be shown */
            //std::string message(buffer, bytesReceived);
            //std::cout << "Message reçu de " << client.username << " : " << message << std::endl;
            //server->broadcastMessage(message, client.client_fd);
        }
    }
    else {
        std::cerr << "login failed";
    }
    return nullptr;
}