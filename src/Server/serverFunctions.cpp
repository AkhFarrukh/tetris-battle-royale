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
    auto roomsList = rooms;
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
        
        pthread_t threadID;
        std::shared_ptr<ThreadData>* threadDataCopy = new std::shared_ptr<ThreadData>(threadData);

        if(pthread_create(&threadID, nullptr, Server::threadRoutineClient, threadDataCopy) < 0) {
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
    std::shared_ptr<ThreadData> threadData = *static_cast<std::shared_ptr<ThreadData>*>(arg);
    delete static_cast<std::shared_ptr<ThreadData>*>(arg);

    ThreadGameData threadGameData;
    GameController gameController(eventQueue);
    Server* server = threadData->serverInstance;
    ClientSession& client = threadData->client;

    server->lockThread = true;
    server->cv.notify_one();

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
        else if(type == MessageType::T_ForcedExit || type == MessageType::T_Exit){
            std::cout << "Client déconnecté : " << client.client_fd << std::endl;
            close(client.client_fd);
            auto it = std::find_if(server->clients.begin(), server->clients.end(),
                                   [&client](const ClientSession& c) {
                                   return c.client_fd == client.client_fd;
                                   });
            if (it != server->clients.end()) {
                server->clients.erase(it);
            }
            return nullptr;
        }
        else{
            std::cout << "Mauvais type de confidentials" << std::endl;
            return 0;
        }

        std::cout<< "Initialised and got the args" << std::endl;
        // Check if both username and password are correct or not.
        std::string username = connexionData.username;
        std::string password = connexionData.password;
        successfulLogin = server->udbm.isCorrectLogin(username.c_str(), password.c_str());
        int typeConnexion;
        int doubleSession;
        if(successfulLogin){
            typeConnexion = MessageType::T_AcceptConnexion;
            int user_id = server->udbm.getUserId(username.c_str());
            bool doubleSession_ = server->udbm.isUserInSession(user_id);
            if(doubleSession_){successfulLogin = false;doubleSession = MessageType::T_RefuseConnexion;}
            else{
                doubleSession = MessageType::T_AcceptConnexion;
                client.userId = user_id;
                client.username = connexionData.username;
                client.password = connexionData.password;
                flagConnexion = false;
            }
            int size_ = sizeof(int);
            int bytesSentType = sendAll(client.client_fd, &typeConnexion, sizeof(int));
            int bytesSentSize = sendAll(client.client_fd, &size_, sizeof(int));
            bytesSentType = sendAll(client.client_fd, &doubleSession, sizeof(int));
            bytesSentSize = sendAll(client.client_fd, &size_, sizeof(int));
        }
        else{
            type = MessageType::T_RefuseConnexion;
            int size_ = sizeof(int);
            int bytesSentType = sendAll(client.client_fd, &typeConnexion, sizeof(int));
            int bytesSentSize = sendAll(client.client_fd, &size_, sizeof(int));
        }
    }
    if (successfulLogin){
        // Get user ID and insert user into sessions table
        server->udbm.setLogin(client.userId);
        server->udbm.addSession(client.userId, client.client_fd);

        
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
            //Client disconnected or error
            if (recvAll(client.client_fd, &type, sizeof(int)) == false || recvAll(client.client_fd, &size, sizeof(int)) == false) {
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
                    break;
                }
                else if (menu.action == MenuActions::A_Play) {
                    if (menu.mode == Modes::Endless_) {
                        
                        std::cout << "Client wants Endless mode\n";

                        threadGameData.client_fd = client.client_fd;
                        threadGameData.eventQueue = eventQueue;
                        std::vector<ThreadGameData> data;
                        data.push_back(threadGameData);
                        
                        std::shared_ptr<Game> soloGame = std::make_shared<Game>(data, 1, "Endless");
                        auto* gamePtrWrapper = new shared_ptr<Game> (soloGame);
                        pthread_t threadGamePID;
                        if (pthread_create(&threadGamePID, nullptr, threadRoutineGame, gamePtrWrapper) < 0) {
                            perror("Error on thread creation for game : ");
                            server->udbm.removeSession(client.userId);
                            close(client.client_fd);
                            userIsOnline = false; // Stop the loop
                            break;
                        }
                    }
                    //autre mode
                }
                else if (menu.action == MenuActions::A_CreateRoom) {
                    auto r = std::make_shared<Room>(client.client_fd, menu.mode);
                    server->roomsList->push_back(r);
                    server->myRoom = r;
                    std::cout << "Created new room with host fd=" << client.client_fd << std::endl;
                
                    RoomInfo info;
                    info.roomID = r->getRoomID();
                    info.currentPlayersCount = r->getPlayerCount();
                    info.isStarted = r->isStarted();
                
                    int typeToSend = MessageType::T_RoomInfo;
                    int sizeToSend = 1;
                
                    sendAll(client.client_fd, &typeToSend, sizeof(typeToSend));
                    sendAll(client.client_fd, &sizeToSend, sizeof(sizeToSend));
                    sendAll(client.client_fd, &info, sizeof(RoomInfo));
                
                }
                else if (menu.action == MenuActions::A_JoinRoom) {
                    std::vector<std::shared_ptr<Room>> filteredRooms;
                    for (auto& r : *server->roomsList) {
                        if (r->getRoomMode() == menu.mode) {
                            filteredRooms.push_back(r);
                        }
                    }
                
                    int nbRooms = (int)filteredRooms.size();
                    sendAll(client.client_fd, &nbRooms, sizeof(nbRooms));
                
                    for (int i = 0; i < nbRooms; i++) {
                        int count = filteredRooms[i]->getPlayerCount();
                        sendAll(client.client_fd, &count, sizeof(count));
                    }
                
                }  
                
                else if (menu.action == MenuActions::A_ForceStart) {
                    server->myRoom->forceStart();
                }
            }

            else if (type == MessageType::T_Refresh){
                RoomInfo info;
                info.roomID = server->myRoom->getRoomID();
                info.currentPlayersCount = server->myRoom->getPlayerCount();
                info.isStarted = server->myRoom->isStarted();
            
                int typeToSend = MessageType::T_RoomInfo;
                int sizeToSend = 1;
            
                sendAll(client.client_fd, &typeToSend, sizeof(typeToSend));
                sendAll(client.client_fd, &sizeToSend, sizeof(sizeToSend));
                sendAll(client.client_fd, &info, sizeof(RoomInfo));
            }

            else if (type == MessageType::T_Join) {
                int ownerFd;
                if (!recvAll(client.client_fd, &ownerFd, sizeof(ownerFd))) {
                    break;
                }
            
                std::shared_ptr<Room> chosenRoom = nullptr;
                for (auto &r : *server->roomsList) {
                    if (r->getOwnerFd() == ownerFd) {
                        chosenRoom = r;
                        break;
                    }
                }
            
                if (!chosenRoom) {
                    std::cout << "No matching room or invalid ownerFd = " << ownerFd << "\n";
                } else {
                    if (!chosenRoom->addPlayer(client.client_fd)) {
                        std::cout << "Failed to join room, maybe full or started\n";
                    } else {
                        std::cout << "Client joined room with hostFd = " << chosenRoom->getOwnerFd() << std::endl;
            
                        RoomInfo info;
                        info.roomID               = chosenRoom->getRoomID();
                        info.currentPlayersCount  = chosenRoom->getPlayerCount();
                        info.isStarted            = chosenRoom->isStarted();
                        info.mode                 = chosenRoom->getRoomMode();
            
                        int typeToSend  = MessageType::T_RoomInfo;
                        int sizeToSend  = 1;
            
                        sendAll(client.client_fd, &typeToSend, sizeof(typeToSend));
                        sendAll(client.client_fd, &sizeToSend, sizeof(sizeToSend));
                        sendAll(client.client_fd, &info,        sizeof(RoomInfo));
                    }
                }
            }
            
                
            
            else if (type == MessageType::T_Input) {
                KeyboardAction action;
                if (!recvAll(client.client_fd, &action, sizeof(KeyboardAction))) {
                    std::cout << "Client déconnecté : " << client.client_fd << std::endl;
                    server->udbm.removeSession(client.userId);
                    close(client.client_fd);
                    userIsOnline = false; // Stop the loop
                    break;
                }
                gameController.getAction(action);
            }
            else if (type == MessageType::T_MalusBonus) {
                MalBonus malusBobo;

                if (!recvAll(client.client_fd, &malusBobo, sizeof(MalBonus))){
                    std::cout << "Client déconnecté : " << client.client_fd << std::endl;
                    server->udbm.removeSession(client.userId);
                    close(client.client_fd);
                    userIsOnline = false; // Stop the loop
                    break;
                }

                eventQueue->addEvent(make_shared<MalusBonusEvent>(malusBobo.malusB, malusBobo.target));
            }
            

            else if (type == MessageType::T_ForcedExit){
                std::cout << "Client déconnecté : " << client.client_fd << std::endl;
                close(client.client_fd);
                    while(server->udbm.isLoggedIn(client.username.c_str())){
                  	server->udbm.logOut(client.userId);} //on marque la personne comme deconnecté
                    server->udbm.setNotInConversation(client.userId); //si la personne etait sur une conv cela permet qu'elle n'y soit plus marqué
                                                                                                         //comme telle
                  	while(server->udbm.isUserInSession(client.userId)){
                  		server->udbm.deleteSession(client.userId);}
                auto it = std::find_if(server->clients.begin(), server->clients.end(),
                                       [&client](const ClientSession& c) {
                                       return c.client_fd == client.client_fd;
                                       });
                if (it != server->clients.end()) {
                    server->clients.erase(it);
                }
                return nullptr;
            }
        }
    }
    else {
        std::cerr << "login failed";
    }
    return nullptr;
}

/**
 * @brief Routine avec Game() qui tourne
 * 
 * @param arg shared_ptr<Game>
 * @return void* 
 */
void* threadRoutineGame(void* arg){
    //Retreive arg
    auto* gamePtrWrapper = static_cast<std::shared_ptr<Game>*>(arg);
    std::shared_ptr<Game> game = *gamePtrWrapper;
    delete gamePtrWrapper;
    game->run();
    return nullptr;
}