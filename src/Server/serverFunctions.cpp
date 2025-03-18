#include "serverFunctions.hpp"
#include <algorithm>


Server::Server(){
    roomsList = make_shared<vector<shared_ptr<Room>>>();
}


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
int Server::handleClient(int server_fd, struct sockaddr_in& address, size_t addressLen){
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
        new_session.inRoom = -1;
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
    GameController gameController;
    Server* server = threadData->serverInstance;
    ClientSession& client = threadData->client;

    server->lockThread = true;
    server->cv.notify_one();

    /***********************************************************************************
    *                               LOGIN HANDLING
    ***********************************************************************************/
    bool flagConnexion = true;
    bool successfulLogin = false;

    while(flagConnexion){
        int type;
        int size;
        int bytesReceivedType = recvAll(client.client_fd, &type, sizeof(int));
        int bytesReceivedSize = recvAll(client.client_fd, &size, sizeof(int));
        Connexion connexionData;
        if(type == MessageType::T_Connexion && size == sizeof(struct Connexion)){
            server->handleConnexion(server, client, connexionData, size, successfulLogin, flagConnexion);
        }
        else if(type == MessageType::T_Register && size == sizeof(struct Connexion)){
            server->handleRegister(server, client, connexionData, size, successfulLogin, flagConnexion);
        }

        else if(type == MessageType::T_ForcedExit || type == MessageType::T_Exit){
            server->handleForcedExit(server, client);
        }
        else{
            std::cout << "Mauvais type de confidentials" << std::endl;
            return 0;
        }

    }
    /***********************************************************************************
    *                     SUCCESSFUL LOGIN, STARTING LISTENING LOOP
    ***********************************************************************************/
    pthread_t threadGamePID;
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
            std::cout << "THANOSSSSSSSSSSSSSSS" << std::endl;
            //int bytesReceived = recv(client.client_fd, buffer, sizeof(buffer), 0);

            //To have info on the data received
            int type = {0};
            int size = {0};


            //Receiving info from client
            if (!recvAll(client.client_fd, &type, sizeof(int))){userIsOnline = false;}
            if (!recvAll(client.client_fd, &size, sizeof(int))){userIsOnline = false;}

            /************************************
            *MENU INFO
            */
            if(type == MessageType::T_Menu){
                std::cout << "Menu" << std::endl;
                Menu menu;
                if (!recvAll(client.client_fd, &menu, sizeof(Menu))){userIsOnline = false;}

                /************************************
                *PLAY ENDLESS INFO
                */
                else if (menu.action == MenuActions::A_Play) {
                    if (menu.mode == Modes::Endless_){
                        server->handlePlay(server, client, userIsOnline, eventQueue, threadGameData);
                    }   
                }
                /************************************
                *CREATE ROOM INFO
                */
                else if (menu.action == MenuActions::A_CreateRoom) {
                    server->handleCreateRoom(server, client, userIsOnline, menu, eventQueue);
                }
                /************************************
                *GET ROOM INFO
                */
                else if (menu.action == MenuActions::A_JoinRoom) {
                    server->handleGetRoom(server, client, userIsOnline, menu);
                }

                else if (menu.action == MenuActions::A_ForceStart) {
                    server->handleForceStart(server, client, userIsOnline, threadGamePID);
                }
            }
            /************************************
            *REFRESH INFO
            */
            else if (type == MessageType::T_Refresh || type == MessageType::T_RoomJoinRefresh){
                bool owner = (type == MessageType::T_Refresh);
                server->handleRefresh(server, client, userIsOnline, owner);
            }
            else if (type == MessageType::T_LeftRoom){
                server->handleLeftRoom(server, client);
            }
            else if (type == MessageType::T_RoomDeleted){
                server->handleDeleteRoom(server, client);
            }
            /************************************
            *JOIN ROOM
            */
            else if (type == MessageType::T_Join) {
                server->handleJoin(server, client, userIsOnline, size, eventQueue);
            }
            /************************************
            *INPUT INFO
            */
            else if (type == MessageType::T_Input) {
                server->handleInput(client, userIsOnline, eventQueue, gameController);
            }
            /************************************
            *MALUS BONUS INFO
            */
            else if (type == MessageType::T_MalusBonus) {
                cout<<"tamer"<<endl;
                server->handleMalusBonus(client, userIsOnline, eventQueue);
            }

            /************************************
            *FORCED EXIT INFO
            */
            else if (type == MessageType::T_ForcedExit){
                server->handleForcedExit(server, client);
            }
            if (!userIsOnline){
                /* EXIT PROPREMENT DU SERV SI DECO */
                server->handleDisconnect(server, client, threadGamePID);
            }
        }
    }
    else {
        std::cerr << "login failed";
    }
    return nullptr;
}



/************************************************************************************
 *                                                                                  *
 *                  METHODS HANDLING MESSAGES FROM THE CLIENT                       *
 *                                                                                  *
 ***********************************************************************************/

/**
 * @brief Routine avec Game() qui tourne
 * 
 * @param arg shared_ptr<Game>
 * @return void* 
 */
void* Server::threadRoutineGame(void* arg){
    //Retreive arg
    auto* gamePtrWrapper = static_cast<std::shared_ptr<Game>*>(arg);
    std::shared_ptr<Game> game = *gamePtrWrapper;
    delete gamePtrWrapper;
    cout<<"Going to run game"<<endl;
    game->run();

    return nullptr;
}

/**
 * @brief Will properly disconnect the player from the server and notifies it on the db
 * 
 * @param server ptr to server
 * @param client ClientSession containing all the client infos
 * @param threadGamePID PID of the thread containing the game of the player, if he hasn't ended yet
 */
void Server::handleDisconnect(Server* server, ClientSession& client, pthread_t& threadGamePID){
    std::cout << "Client déconnecté : " << client.client_fd << std::endl;
    close(client.client_fd);
    pthread_cancel(threadGamePID);
        while(server->udbm.isLoggedIn(client.username.c_str())){
      	server->udbm.logOut(client.userId);} //on marque la personne comme deconnecté
        server->udbm.setNotInConversation(client.userId); //si la personne etait sur une conv cela permet qu'elle n'y soit plus marqué
                                                                                             //comme telle
      	while(server->udbm.isUserInSession(client.userId)){
      		server->udbm.deleteSession(client.userId);}
    server->clientMutex.lock();
    auto it = std::find_if(server->clients.begin(), server->clients.end(),
                           [&client](const ClientSession& c) {
                           return c.client_fd == client.client_fd;
                           });
    if (it != server->clients.end()) {
        server->clients.erase(it);
    }
    server->clientMutex.unlock();
}

/**
 * @brief Will close the client session
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 */
void Server::handleForcedExit(Server* server, ClientSession& client){
    std::cout << "Client déconnecté : " << client.client_fd << std::endl;
    close(client.client_fd);
        while(server->udbm.isLoggedIn(client.username.c_str())){
      	server->udbm.logOut(client.userId);} //on marque la personne comme deconnecté
        server->udbm.setNotInConversation(client.userId); //si la personne etait sur une conv cela permet qu'elle n'y soit plus marqué
                                                                                             //comme telle
      	while(server->udbm.isUserInSession(client.userId)){
      		server->udbm.deleteSession(client.userId);}
    server->clientMutex.lock();
    auto it = std::find_if(server->clients.begin(), server->clients.end(),
                           [&client](const ClientSession& c) {
                           return c.client_fd == client.client_fd;
                           });
    if (it != server->clients.end()) {
        server->clients.erase(it);
    }
    server->clientMutex.unlock();
}

/**
 * @brief Will handle the message that the player wants to join a room
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param userIsOnline bool handling the listening loop, true if connected to the serv 
 * @param size size of the message to receive
 * @param eventQueue ptr for the player's eventQueue in Game
 */
void Server::handleJoin(Server* server, ClientSession& client, bool& userIsOnline, int& size, shared_ptr<EventQueue> eventQueue){
    cout<<"Join received"<<endl;
    int ownerFd;
    if (!recvAll(client.client_fd, &ownerFd, size)){userIsOnline = false;}

    std::shared_ptr<Room> chosenRoom = nullptr;
    for (auto &r : *server->roomsList) {
        cout<<"Listing rooms fd"<<r->getOwnerFd()<<endl;
        if (r->getOwnerFd() == ownerFd) {
            chosenRoom = r;
            break;
        }
    }

    if (!chosenRoom) {
        std::cout << "No matching room or invalid ownerFd = " << ownerFd << "\n";
    }
    else {
        if (!chosenRoom->addPlayer(client.client_fd, eventQueue)) {
            std::cout << "Failed to join room, maybe full or started\n";
        }
        else {
            std::cout << "Client joined room with hostFd = " << chosenRoom->getOwnerFd() << std::endl;

            RoomInfo info;
            info.roomID               = chosenRoom->getRoomID();
            info.currentPlayersCount  = chosenRoom->getPlayerCount();
            info.isStarted            = chosenRoom->isStarted();
            info.mode                 = chosenRoom->getRoomMode();

            client.inRoom = info.roomID;
            int typeToSend  = MessageType::T_RoomJoinInfo;
            int sizeToSend  = sizeof(struct RoomInfo);

            if (!sendAll(client.client_fd, &typeToSend, sizeof(typeToSend))){userIsOnline = false;}
            if (!sendAll(client.client_fd, &sizeToSend, sizeof(sizeToSend))){userIsOnline = false;}
            if (!sendAll(client.client_fd, &info,        sizeof(RoomInfo))){userIsOnline = false;}

            server->myRoom = chosenRoom;
        }
    }
}

void Server::handleLeftRoom(Server* server, ClientSession& client) {
  std::shared_ptr<Room> chosenRoom = nullptr;
  for (auto &r : *server->roomsList) {
        if (r->getOwnerFd() == client.inRoom) {
            chosenRoom = r;
            break;
        }
    }
    chosenRoom->removePlayer(client.client_fd);
    client.inRoom = -1;
}

/**
 * @brief Send again all the info about the room of & to the client
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param userIsOnline bool handling the listening loop, true if connected to the serv 
 */
void Server::handleRefresh(Server* server, ClientSession& client, bool& userIsOnline, bool& owner){
    cout<<"Refresh received"<<endl;
     std::shared_ptr<Room> chosenRoom = nullptr;
     for (auto &r : *server->roomsList) {
        if (r->getOwnerFd() == client.inRoom) {
            chosenRoom = r;
            break;
        }
     }
    if(!chosenRoom)userIsOnline = false;
    RoomInfo info;
    info.roomID = chosenRoom->getRoomID();
    info.currentPlayersCount = chosenRoom->getPlayerCount();
    info.isStarted = chosenRoom->isStarted();

    int typeToSend = MessageType::T_RoomInfo;
    if(!owner)typeToSend = MessageType::T_RoomJoinInfo;
    int sizeToSend = sizeof(struct RoomInfo);

    if (!sendAll(client.client_fd, &typeToSend, sizeof(typeToSend))){userIsOnline = false;}
    if (!sendAll(client.client_fd, &sizeToSend, sizeof(sizeToSend))){userIsOnline = false;}
    if (!sendAll(client.client_fd, &info, sizeof(RoomInfo))){userIsOnline = false;}
}

void Server::handleForceStart(Server* server, ClientSession& client, bool& userIsOnline, pthread_t& threadGamePID){
    std::shared_ptr<Room> room = nullptr;
     for (auto &r : *server->roomsList) {
        if (r->getOwnerFd() == client.inRoom) {
            room = r;
            break;
        }
     }
    if(!room)userIsOnline = false;
    if (room->checkGameReady()){
    room->broadcastGameStart();
    std::shared_ptr<Game> gameShPtr =  room->launchGame();
    auto* gamePtrWrapper = new shared_ptr<Game> (gameShPtr);
    cout <<"Creating game thread"<<endl;

    if (pthread_create(&threadGamePID, nullptr, threadRoutineGame, gamePtrWrapper) < 0) {
            perror("Error on thread creation for game : ");
            server->udbm.removeSession(client.userId);
            close(client.client_fd);
            userIsOnline = false; // Stop the loop
        }
    }
}

/**
 * @brief Will send to the client all the rooms available in the gameMode choosed
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param userIsOnline bool handling the listening loop, true if connected to the serv
 * @param menu Menu struct containing the gameMode choosed  by the client
 */
void Server::handleGetRoom(Server* server, ClientSession& client, bool& userIsOnline, Menu& menu){
    cout<<"JoinRoom received"<<endl;
    std::vector<RoomInfo> filteredRooms;
    auto allRooms = server->roomsList;
    for (auto& r : *allRooms) {
        if (r->getRoomMode() == menu.mode) {
            RoomInfo info;
            info.roomID = r->getRoomID();
            info.currentPlayersCount = r->getPlayerCount();
            info.isStarted = r->isStarted();
            filteredRooms.push_back(info);
        }
    }
    cout<<"Done filtered Room"<<endl;

    MessageType typeToSend = MessageType::T_RoomsInfo;
    int nbRooms = (int)filteredRooms.size();
    if (!sendAll(client.client_fd, &typeToSend, sizeof(typeToSend))){userIsOnline = false;}
    if (!sendAll(client.client_fd, &nbRooms, sizeof(nbRooms))){userIsOnline = false;}
    if (!sendAll(client.client_fd, filteredRooms.data(), nbRooms * sizeof(RoomInfo))){userIsOnline = false;}
    int type = menu.mode;
    int size = sizeof(int);
    sendAll(client.client_fd, &type,  sizeof(type));
    sendAll(client.client_fd, &size,  sizeof(size));
}

/**
 * @brief Will create a room and send it back to the client
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param userIsOnline bool handling the listening loop, true if connected to the serv
 * @param menu Menu struct containing the gameMode choosed  by the client
 * @param eventQueue ptr for the player's eventQueue in Game
 */
void Server::handleCreateRoom(Server* server, ClientSession& client, bool& userIsOnline, Menu& menu, shared_ptr<EventQueue> eventQueue){
    std::cout<<"Received CreateRoom"<<std::endl;
    auto r = std::make_shared<Room>(client.client_fd, menu.mode, eventQueue);
    auto allRoom = server->roomsList;
    server->myRoom = r;
    allRoom->push_back(r);
    std::cout << "Created new room with host fd=" << client.client_fd << std::endl;
    RoomInfo info;
    info.roomID = r->getRoomID();
    info.currentPlayersCount = r->getPlayerCount();
    info.isStarted = r->isStarted();
    client.inRoom = info.roomID;
    int typeToSend = MessageType::T_RoomInfo;
    int sizeToSend = sizeof(RoomInfo);
    if (!sendAll(client.client_fd, &typeToSend, sizeof(typeToSend))){userIsOnline = false;}
    if (!sendAll(client.client_fd, &sizeToSend, sizeof(sizeToSend))){userIsOnline = false;}
    if (!sendAll(client.client_fd, &info, sizeToSend)){userIsOnline = false;}
}

void Server::handleDeleteRoom(Server* server, ClientSession& client) {
    std::cout << "Received DeleteRoom" << std::endl;
    std::shared_ptr<Room> room = nullptr;
     for (auto &r : *server->roomsList) {
        if (r->getOwnerFd() == client.client_fd) {
            room = r;
            break;
        }
     }

    auto allRooms = server->roomsList;
    if(room){
        room->broadcastDeleteRoom();
        allRooms->erase(
            std::remove_if(allRooms->begin(), allRooms->end(),
                [&](const std::shared_ptr<Room>& r) {
                    return r == room;
                }),
            allRooms->end()
        );
        server->myRoom.reset();
    }
    client.inRoom = -1;
}

/**
 * @brief Will start a game of endless mode
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param userIsOnline bool handling the listening loop, true if connected to the serv
 * @param eventQueue ptr for the player's eventQueue in Game
 * @param threadGameData data that willbe passed to the game's thread, containing all player & game infos
 */
void Server::handlePlay(Server* server, ClientSession& client, bool& userIsOnline, shared_ptr<EventQueue> eventQueue, ThreadGameData& threadGameData){
    std::cout << "Client wants Endless mode\n";

    threadGameData.client_fd = client.client_fd;
    threadGameData.eventQueue = eventQueue;
    std::vector<ThreadGameData> data;
    data.push_back(threadGameData);

    int type = MessageType::T_GameStart;
    int size = sizeof(int);
    sendAll(client.client_fd, &type,  sizeof(type));
    sendAll(client.client_fd, &size,  sizeof(size));
    napms(355);
    std::shared_ptr<Game> soloGame = std::make_shared<Game>(data, 1, Modes::Endless_);
    auto* gamePtrWrapper = new shared_ptr<Game> (soloGame);
    pthread_t threadGamePID;
    if (pthread_create(&threadGamePID, nullptr, threadRoutineGame, gamePtrWrapper) < 0) {
        perror("Error on thread creation for game : ");
        server->udbm.removeSession(client.userId);
        close(client.client_fd);
        userIsOnline = false; // Stop the loop
    }
}

/**
 * @brief Will handle the reeiving of a malus bonus message and send it to the game
 * 
 * @param client ClientSession struct
 * @param userIsOnline bool handling the listening loop, true if connected to the serv
 * @param eventQueue ptr for the player's eventQueue in Game
 */
void Server::handleMalusBonus(ClientSession& client, bool& userIsOnline, shared_ptr<EventQueue> eventQueue){
    cout<<"MalusBonus received"<<endl;
    MalBonus malusBobo;

    if (!recvAll(client.client_fd, &malusBobo, sizeof(MalBonus))){userIsOnline = false;}

    eventQueue->addEvent(make_shared<MalusBonusEvent>(malusBobo.malusB, malusBobo.target));
}

/**
 * @brief Will handle the receiving of an input message, and add it to the game's eventQueue
 * @param client ClientSession struct
 * @param userIsOnline bool handling the listening loop, true if connected to the serv
 * @param eventQueue ptr for the player's eventQueue in Game
 * @param gameController 
 */
void Server::handleInput(ClientSession& client, bool& userIsOnline, shared_ptr<EventQueue> eventQueue, GameController& gameController){
    cout<<"Input received"<<endl;
    KeyboardAction action;
    if (!recvAll(client.client_fd, &action, sizeof(KeyboardAction))){userIsOnline = false;}
    eventQueue->addEvent(make_shared<MovementActionEvent>(gameController.getAction(action)));
}

/**
 * @brief Notify player if the login has been successful or not
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param connexionData Connexion struct, containing info bout connexion
 * @param username string containing the username entered
 * @param successfulLogin bool set to true or false depending on login issue
 * @param typeConnexion 
 * @param doubleSession 
 * @param flagConnexion 
 */
void Server::notifySuccessfulLogin(Server* server, ClientSession& client, Connexion& connexionData, string& username, 
                                        bool& successfulLogin, int& typeConnexion, int& doubleSession, bool& flagConnexion){
    typeConnexion = MessageType::T_AcceptConnexion;
    int user_id = server->udbm.getUserId(username.c_str());
    bool doubleSession_ = server->udbm.isUserInSession(user_id);
    if(doubleSession_){
        successfulLogin = false;
        doubleSession = MessageType::T_RefuseConnexion;
    }
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

/**
 * @brief Will handle the login of a client
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param connexionData Connexion struct, containing info bout connexion
 * @param size size of the struc to receive
 * @param successfulLogin bool set to true or false depending on login issue
 * @param flagConnexion 
 */
void Server::handleConnexion(Server* server, ClientSession& client, Connexion& connexionData, int& size, bool& successfulLogin, bool& flagConnexion){
    if(!recvAll(client.client_fd, &connexionData, size)){
        std::cout << "Erreur lors de l'envoie des confidentials" << std::endl;
        return;
    }
    std::cout<< "Initialised and got the args" << std::endl;
    // Check if both username and password are correct or not.
    std::string username = connexionData.username;
    std::string password = connexionData.password;
    successfulLogin = server->udbm.isCorrectLogin(username.c_str(), password.c_str());
    int typeConnexion;
    int doubleSession;
    if(successfulLogin){
        server->notifySuccessfulLogin(server, client, connexionData, username, successfulLogin, typeConnexion, doubleSession, flagConnexion);
    }
    else{
        server->notifyRefusedLogin(client);
    }
}

/**
 * @brief Will handle the registering of a client
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param connexionData Connexion struct, containing info bout connexion
 * @param size size of the struc to receive
 * @param successfulLogin bool set to true or false depending on login issue
 * @param flagConnexion 
 */
void Server::handleRegister(Server* server, ClientSession& client, Connexion& connexionData, int& size, bool& successfulLogin, bool& flagConnexion){
    if(!recvAll(client.client_fd, &connexionData, size)){
        std::cout << "Erreur lors de l'envoie des confidentials" << std::endl;
        return;
    }
    bool registered = server->udbm.newAccountCreated(connexionData.username, connexionData.password);
    if(registered){
        server->notifyAcceptConnexion(server, client, connexionData, successfulLogin);
        if(successfulLogin){
            server->notifyAcceptConnexion(server, client, connexionData, successfulLogin);
            client.userId = server->udbm.getUserId(connexionData.username);
            client.username = connexionData.username;
            client.password = connexionData.password;
            flagConnexion = false;
           }
        else{
            server->notifyRefusedLogin(client);
        }
    }
    else{
        server->notifyRefusedLogin(client);
    }
}

/************************************************************************************
 *                                                                                  *
 *                  METHODS HANDLING LOGIN OF THE CLIENT                            *
 *                                                                                  *
 ***********************************************************************************/

/**
 * @brief Will send to the client RefuseConnexion message
 * 
 * @param client ClientSession struct
 */
void Server::notifyRefusedLogin(ClientSession& client){
    int type_ = MessageType::T_RefuseConnexion;
    int size_ = sizeof(int);
    int bytesSentType = sendAll(client.client_fd, &type_, sizeof(int));
    int bytesSentSize = sendAll(client.client_fd, &size_, sizeof(int));
}

/**
 * @brief Will send to the client the AcceptConnexion message and update db
 * 
 * @param server ptr to server
 * @param client ClientSession struct
 * @param connexionData Connexion struct, containing info bout connexion
 * @param successfulLogin bool set to true or false depending on login issue
 */
void Server::notifyAcceptConnexion(Server* server, ClientSession& client, Connexion& connexionData, bool& successfulLogin){
    int typeRegister = MessageType::T_AcceptConnexion;
    int size_ = sizeof(int);
    int bytesSentType = sendAll(client.client_fd, &typeRegister, sizeof(int));
    int bytesSentSize = sendAll(client.client_fd, &size_, sizeof(int));
    successfulLogin = server->udbm.isCorrectLogin(connexionData.username, connexionData.password);
}

