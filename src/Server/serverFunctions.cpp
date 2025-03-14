#include "serverFunctions.hpp"
#include <algorithm>


/**
 * TODO: COMMENTS TO BE ADDED BY THOSE WHO WROTE THIS FUNCTION/METHOD. STP
 */
void Server::broadcastMessage(const char* buffer, const std::string& sender_username) {
    size_t friendNameLength = std::find(buffer, buffer+strlen(buffer), '\0') - buffer;

    // Extraire le nom d'utilisateur
    std::string friendName(buffer, friendNameLength);

    // Extraire le message (après le nom d'utilisateur)
    std::string message(buffer + friendNameLength + 1);
    for (const auto client : clients) {
        if (client.username == friendName) {
            std::vector<char> messageToSend(sender_username.size()+1+message.size());
            memcpy(messageToSend.data(), sender_username.c_str(), sender_username.size()+1);
            memcpy(messageToSend.data() + sender_username.size()+1, message.c_str(), message.size());
            int bytesReceived = send(client.client_fd, messageToSend.data(), messageToSend.size(), 0);
            if (bytesReceived <= 0) {
                std::cout << "Client déconnecté : " << client.client_fd << std::endl;
                //TODO l'enlever de la liste?
            }
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
int Server::handleClient(int server_fd, struct sockaddr_in& address, size_t addressLen, std::shared_ptr<std::vector<std::shared_ptr<Room>>> rooms){
    roomsList = rooms;
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

        /* CHECK AFTER MERGE */
        // The password MUST be a plain password (what received from the client).
        // The User Database Manager will take care of password hash validation.

        printf("goood\n");
        // Create a "new session" with client info to be sent to thread routine
        // and push the client into a vector of clients
        ClientSession new_session;
        new_session.client_fd = client_fd;
        clients.push_back(new_session);
        //printf("%s, %s\n", username.c_str(),password.c_str());
        // Create a smart pointer containing data to be passed into thread routine.

        std::shared_ptr<ThreadData> threadData = std::make_shared<ThreadData>();
        threadData->client = ClientSession(new_session);
        threadData->serverInstance = this; // "this" = a pointer of server object
        
        // Create a thread for a user if the username and password is correct.
        pthread_t threadID;
        if(pthread_create(&threadID, nullptr, threadRoutineClient, threadData.get()) < 0){
            perror("Error on thread creation : ");
            close(client_fd);
            clients.pop_back();
            return -1;
        }
        // Unlock mutex when got the signal from inside the thread
        cv.wait(lock, [this]{return lockThread;});
        std::cout << "Thread successfully created !" << std::endl;
        lock.unlock();
        //pthread_join(threadID, nullptr);
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
    bool flagConnexion = true;
    bool successfulLogin = false;

    while(flagConnexion){
        int type;
        int size;
        int bytesReceivedType = recvAll(client.client_fd, &type, sizeof(int));
        int bytesReceivedSize = recvAll(client.client_fd, &size, sizeof(int));
        Connexion connexionData;
        if(type == MessageType::T_Connexion && size == sizeof(struct Connexion)){
            if(!recvAll(client.client_fd, &connexionData, size)){
                std::cout << "Erreur lors de l'envoie des confidentials" << std::endl;
                return 0;
            }
        }
        else if(type == MessageType::T_Exit){
             flagConnexion = false;
             break;
          }
        else{
            std::cout << "Mauvais type de confidentials" << std::endl;
            return 0;
        }

        std::cout<< "Initialised and got the args" << std::endl;
        // Check if both username and password are correct or not.
        successfulLogin = server->udbm.isCorrectLogin(client.username.c_str(), client.password.c_str());
        printf("%d\n", successfulLogin);
        int type_;
        if(successfulLogin){
            type = MessageType::T_AcceptConnexion;
            int id = udbm.getUserId(client.username);
            bool doubleSession = db.isUserInSession(user_id);
        }
        else{
            type = MessageType::T_RefuseConnexion;
        }
        int size_ = sizeof(int);
        int bytesSentType = sendAll(client.client_fd, &type_, sizeof(int));
        int bytesSentSize = sendAll(client.client_fd, &size_, sizeof(int));
    }
    if (successfulLogin){
        // Get user ID and insert user into sessions table
        client.userId = server->udbm.getUserId(client.username.c_str());
        server->udbm.addSession(client.userId, client.client_fd);

        //Unlock the mutex and notifies it outside the thread
        server->lockThread = true;
        server->cv.notify_one();
        
        // Keep looking for any message sent by the user to the server
        // Using bool "UserIsOnline" for better readability of while loop

        bool userIsOnline = true;
        bool userIsInGame = false;

        while (userIsOnline) {
            std::cout << "start listening cuh" << std::endl;
            //int bytesReceived = recv(client.client_fd, buffer, sizeof(buffer), 0);

            //To have info on the data received
            int type = {0};
            int size = {0};
            

            //Sending info to server
            int bytesReceivedType = recvAll(client.client_fd, &type, sizeof(int));
            int bytesReceivedSize = recvAll(client.client_fd, &size, sizeof(int));
            std::cout << "fdp ma lumiere : " << type << std::endl;
            std::cout << "fdp ma lumiere : " << size << std::endl;

            //Client disconnected or error
            if (bytesReceivedType <= 0 || bytesReceivedSize <= 0) {
                std::cout << "Client déconnecté : " << client.client_fd << std::endl;
                server->udbm.removeSession(client.userId);
                close(client.client_fd);
                
                auto it = std::find_if(server->clients.begin(), server->clients.end(),
                                       [&client](const ClientSession& c) { 
                                       return c.client_fd == client.client_fd; 
                                       });
                if (it != server->clients.end()) {
                    server->clients.erase(it);
                }
                userIsOnline = false; // Stop the loop
            }
            

            else if(type == MessageType::T_Menu){
                if (size != sizeof(Menu)) {
                    std::cout << "fdp ma lumiere : " << client.client_fd << std::endl;
                }
                std::cout << "Menu" << std::endl;
                Menu menu;
                if (!recvAll(client.client_fd, &menu, sizeof(Menu))){
                    //Repetition de code, pas ouf, need method
                    std::cout << "Client déconnecté : " << client.client_fd << std::endl;
                    server->udbm.removeSession(client.userId);
                    close(client.client_fd);
                    userIsOnline = false; // Stop the loop
                }
                else if (menu.action == MenuActions::A_Play) {
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
                    //autre mode
                }
                else if (menu.action == MenuActions::A_CreateRoom) {
                    auto r = std::make_shared<Room>(client.client_fd, menu.mode);
                    server->roomsList->push_back(r);
                    server->myRoom = r;
                    std::cout << "Created new room with host fd=" << client.client_fd << std::endl;

                }
                else if (menu.action == MenuActions::A_JoinRoom) {
                    int nbRooms = (int)server->roomsList->size();
                    sendAll(client.client_fd, &nbRooms, sizeof(nbRooms));

                    for (int i = 0; i < nbRooms; i++) {
                        int count = server->roomsList->at(i)->getPlayerCount();
                        sendAll(client.client_fd, &count, sizeof(count));
                    }

                    int chosenIndex;
                    if (!recvAll(client.client_fd, &chosenIndex, sizeof(chosenIndex))) {
                        std::cout << "Disconnected while picking room\n";
                        close(client.client_fd);
                        userIsOnline = false;
                        break;
                    }

                    if (chosenIndex >= 0 && chosenIndex < nbRooms) {
                        auto chosenRoom = server->roomsList->at(chosenIndex);
                        if (!chosenRoom->addPlayer(client.client_fd)) {
                            std::cout << "Failed to join room\n";
                        } else {
                            std::cout << "Client joined room index " << chosenIndex << std::endl;
                        }
                    }
                }
                else if (menu.action == MenuActions::A_ForceStart) {
                    server->myRoom->forceStart();
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
