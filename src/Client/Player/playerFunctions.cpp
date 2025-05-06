#include "playerFunctions.hpp"


void Player::affichageMessage(Message& message){
    nodelay(stdscr, false);
    //interfaceMutex.lock(); //TEMPORARY SO IT COMPILES, DOESNT WORK BC I REMOVED THE GLOBAL VAR
    refresh();
    curs_set(0);
    int currentLine = LINES-1;
    while(mvinch(currentLine-1, 0) == ' ' || mvinch(currentLine-1, 0) == ERR){ //tant que ligne est vide
        currentLine--;
    }
    move(currentLine, 0);
    printw("%s : %s\n", message.expediteur, message.message);
    if (LINES-currentLine<5) {
        move(LINES-1, 0);
        clrtoeol();
        scrollok(stdscr, true);
        scroll(stdscr);
        scrollok(stdscr, false);
        move(3, 0);clrtoeol();
        move(2, 0);clrtoeol();
        move(1, 0);clrtoeol();
        move(0, 0);clrtoeol();
        printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
        printw("\nDiscussion with %s : \n", message.expediteur);
    }
    move(LINES- 1, 0);
    clrtoeol();
    printw("Enter your message : ");
    refresh();
    curs_set(1);
    //interfaceMutex.unlock(); //TEMPORARY SO IT COMPILES, DOESNT WORK BC I REMOVED THE GLOBAL VAR
}




/**
 * @brief Routine capturing all the thing the server sends, and if it's a player state, show it
 * 
 * @param arg ThreadData, containing socket and bool for the loop capturing inputs
 * @return void* 
 */
void* Player::threadRoutineView (void* arg){
    //std::cout<<"Starting threadRoutineView"<<std::endl;
    Player::ThreadData* threadData = static_cast<Player::ThreadData*>(arg);
    int sock = threadData->socket;
    RenderEngineNcurses render;
    while (keepRunning) {

        bool inGame = threadData->userInGame;

        //To have info on the data received
        int type;
        int size;
        //Sending info to server
        /* PROBABLY NEED TO PUT A MUTEX ON SOCKET, SO WE DON'T READ AND WRITE AT THE SAME TIME */
        int bytesReceivedType = recvAll(sock, &type, sizeof(int));
        int bytesReceivedSize = recvAll(sock, &size, sizeof(int));
        //Client disconnected or error
        if (bytesReceivedType <= 0 || bytesReceivedSize <= 0) {
            perror("Client disconnected : ");
            threadData->userInGame = false; // Stop the loop
            keepRunning = false;
            break;
        }
        else if(type == MessageType::T_Message){
            Message message;
            if (!recvAll(sock, &message, size)) {
                perror("Error on receiving room");
                keepRunning = false;
                break;
            }
            affichageMessage(message);
        }
        else if(type == MessageType::T_Exit){
            threadData->userInGame = false;
            threadData->userInLobby = false;
            napms(300);
            continue;
        }
        else if(type == MessageType::T_PlayerData || type == MessageType::T_EndGame){
            vector<NetPlayerData> playersData;
            for (int i = 0; i < size; ++i) {
                NetPlayerData player;
                /* NEED ERROR HANDLING */
                if (!recvAll(sock, &player.playerId, sizeof(player.playerId))){return nullptr;}
                if (!recvAll(sock, &player.isGameOver, sizeof(player.isGameOver))){return nullptr;}
                if (!recvAll(sock, &player.linesCleared, sizeof(player.linesCleared))){return nullptr;}
                if (!recvAll(sock, &player.score, sizeof(player.score))){return nullptr;}
                if (!recvAll(sock, &player.nrg, sizeof(player.nrg))) {return nullptr;}

                int gridSize;
           	    if (!recvAll(sock, &gridSize, sizeof(gridSize))){return nullptr;}

                if (gridSize <= 0 || gridSize > 100000) {  // juste pour detecter quand gridSize devient trop grand
                        std::cerr << "Error: Invalid gridSize received: " << gridSize << std::endl;
                        break;
                }
                player.grid.resize(gridSize);

                if (!recvAll(sock, player.grid.data(), gridSize * sizeof(Color))){return nullptr;}
                if(type == MessageType::T_EndGame)player.endGame = true;
                playersData.push_back(player);
            }


            if (threadData->gameMode == Modes::Royal_){
                threadData->displayMut.lock();
                render.renderGamesRoyal(playersData, threadData->targetPlayerMenu);
                threadData->displayMut.unlock();

                // Find which player is the client (ID == -1)
                int clientPlayerIndex = -1;
                for (size_t i = 0; i < playersData.size(); ++i) {
                    if (playersData.at(i).playerId == -1) {
                        clientPlayerIndex = i;
                        threadData->clinetPlayerID = i; // Store the index
                        break;
                    }
                }
            } else {
                // Render the regular games grid for endless, classic and duel modes
                threadData->displayMut.lock();
                render.renderGames(playersData);
                threadData->displayMut.unlock();
            }

        }
        if (type == MessageType::T_ForcedExit){
            keepRunning = false;
            napms(300);
            break;
        }
        else if (type == MessageType::T_GameStart) {

            threadData->userInGame = true;
            threadData->userInLobby = true;

            continue;
        }
        else if (type == MessageType::T_RoomInfo || type == MessageType::T_RoomJoinInfo) {
            RenderRooms renderR;
            RoomInfo info;
            if (!recvAll(sock, &info, size)) {
                perror("Error on receiving room");
                keepRunning = false;
                break;
            }
            int flags = fcntl(sock, F_GETFL, 0);
            fcntl(sock, F_SETFL, flags | O_NONBLOCK);

            int act = 0;
            while (keepRunning) {
                if (threadData->isSpectator) {
                    // ─── Spectator Waiting Room ───────────────────────
                    int act = renderR.handleSpectatorRoom(info);
                    if (act == -2) {
                        int msg = MessageType::T_Refresh, sz = sizeof(int);
                        sendAll(sock, &msg,  sizeof(msg));
                        sendAll(sock, &sz,   sizeof(sz));
                    }
                    else if (act == -1) {
                        // Go back to room list
                        int sendType = MessageType::T_LeftRoom;
                        int sendSize = sizeof(int);
                        if(!sendAll(sock, &sendType, sizeof(sendType))){keepRunning = true; break;}
                        if(!sendAll(sock, &sendSize, sizeof(sendSize))){keepRunning = true; break;}
        
                        threadData->mux.lock();
                        threadData->isSpectator = false;
                        threadData->mux.unlock();
                        break;
                    }
                }
                else {
                    // ─── Player Lobby ───────────────────────────────────
                    int act = renderR.handleMyRoom(info);

                    if (act == 1 && type == MessageType::T_RoomInfo) {
                        int sendType = MessageType::T_Menu;
                        int sendSize = sizeof(MenuMessage);
                        MenuMessage MenuMessage;
                        MenuMessage.action = MenuActions::A_ForceStart;

                        if(!sendAll(sock, &sendType, sizeof(sendType))){keepRunning = false; break;}
                        if(!sendAll(sock, &sendSize, sizeof(sendSize))){keepRunning = false; break;}
                        if(!sendAll(sock, &MenuMessage, sizeof(MenuMessage))){keepRunning = false; break;}

                    } else if (act == -2) {
                        if(type == MessageType::T_RoomInfo){
                            int sendType = MessageType::T_Refresh;
                            int sendSize = sizeof(int);

                            if(!sendAll(sock, &sendType, sizeof(sendType))){keepRunning = false; break;}
                            if(!sendAll(sock, &sendSize, sizeof(sendSize))){keepRunning = false; break;}
                            break;
                        }
                        else{
                            int sendType = MessageType::T_RoomJoinRefresh;
                            int sendSize = sizeof(int);

                            if(!sendAll(sock, &sendType, sizeof(sendType))){keepRunning = false; break;}
                            if(!sendAll(sock, &sendSize, sizeof(sendSize))){keepRunning = false; break;}
                            break;
                        }
                    } else if (act == -1) {
                        if(type == MessageType::T_RoomInfo){
                            int sendType = MessageType::T_RoomDeleted;
                            int sendSize = sizeof(int);
                            if(!sendAll(sock, &sendType, sizeof(sendType))){keepRunning = false; break;}
                            if(!sendAll(sock, &sendSize, sizeof(sendSize))){keepRunning = false; break;}
                            threadData->userInLobby = false;
                            break;
                        }
                        else{
                            int sendType = MessageType::T_LeftRoom;
                            int sendSize = sizeof(int);
                            if(!sendAll(sock, &sendType, sizeof(sendType))){keepRunning = false; break;}
                            if(!sendAll(sock, &sendSize, sizeof(sendSize))){keepRunning = false; break;}
                            threadData->userInLobby = false;

                            break;
                        }
                    }
                }
                int messageType;
                int messageSize;

                int bytesReceivedType = recv(sock, &messageType, sizeof(int), MSG_DONTWAIT);

                if (bytesReceivedType > 0) {
                    recv(sock, &messageSize, sizeof(int), 0);

                    if (messageType == MessageType::T_GameStart) {
                        threadData->userInGame = true;
                        break;
                    }
                    else if(messageType == MessageType::T_ForcedExit){
                        threadData->userInGame = false;
                        threadData->userInLobby = false;
                        keepRunning = false;
                        break;
                    }
                    else if(messageType == MessageType::T_Exit){
                        threadData->userInGame = false;
                        threadData->userInLobby = false;
                        break;
                    }
                } else if (bytesReceivedType == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
                    perror("Socket error");
                    keepRunning = false; break;
                }
                usleep(500);
            }

            fcntl(sock, F_SETFL, flags);
         }

        else if (type == MessageType::T_RoomsInfo){
            RenderRooms renderR;
            std::vector<RoomInfo> roomsInfo;
            roomsInfo.resize(size);
            if (!recvAll(sock, roomsInfo.data(), size *sizeof(RoomInfo)))
            {keepRunning = false; break;}
            Modes mode;
            int size;
            if(!recvAll(sock, &mode, sizeof(int))){keepRunning = false; break;}
            if(!recvAll(sock, &size, sizeof(int))){keepRunning = false;break;}
            clear();
            refresh();
            int index;
            int highlightIndex = 0;
            while (keepRunning) {
                threadData->displayMut.lock();
                index = renderR.handleListRooms(roomsInfo, highlightIndex);
                threadData->displayMut.unlock();
                if (index == -3) {
                    continue;
                }
                if (index >= 0) {
                    size_t N = roomsInfo.size();
                    bool isSpectator = index >= (int)N;
                    int roomIdx = isSpectator ? index - N : index;
                    int typeToSend = isSpectator
                        ? MessageType::T_JoinAsSpectator
                        : MessageType::T_Join;
                    int sizeToSend = sizeof(int);
                    int fd = roomsInfo[roomIdx].roomID;

                    if (!sendAll(sock, &typeToSend, sizeof(typeToSend))) {keepRunning = false; break;}
                    if (!sendAll(sock, &sizeToSend, sizeof(sizeToSend))) {keepRunning = false; break;}
                    if (!sendAll(sock, &fd, sizeof(int))) {keepRunning = false; break;}
                    threadData->mux.lock();
                    threadData->isSpectator = isSpectator;
                    threadData->userInLobby = true;
                    threadData->mux.unlock();
                    break;
                }
                if (index == -2) {
                    int type = MessageType::T_Menu;
                    int size = sizeof(MenuMessage);
                    MenuMessage MenuMessage;
                    MenuMessage.mode = mode;
                    MenuMessage.action = MenuActions::A_JoinRoom;


                    if(sendAll(sock, &type, sizeof(int)) == false){
                        perror("Error sending MenuMessage info : ");
                        keepRunning = false; break;
                    }
                    if(sendAll(sock, &size, sizeof(int)) == false){
                        perror("Error sending MenuMessage info : ");
                        keepRunning = false; break;
                    }
                    if(sendAll(sock, &MenuMessage, sizeof(MenuMessage)) == false){
                        perror("Error sending MenuMessage info : ");
                        keepRunning = false; break;
                    }
                    break;
                }
                if (index == -1) {

                    threadData->userInLobby = false;

                    break;
                }
            }

        }
    }

return 0;
}

/**
 * @brief Capture the inputs and send it to the server
 * 
 * @param threadData containing socket & bool for the loop
 * @return int 
 */

int Player::sendInput (std::shared_ptr<Player::ThreadData> threadData) {
    int chosenMalus = 0;
    int chosenPlayer = 0;
    bool inLobby = threadData->userInLobby;
    while (keepRunning && inLobby) {

        bool keepRunning = threadData->userInGame;
        inLobby = threadData->userInLobby;

        if (!keepRunning && !inLobby) {break;
        } else if(inLobby && !keepRunning) continue;
        else{
            threadData->displayMut.lock();
            KeyboardAction currentAction = Input::getKeyboardInput();
            threadData->displayMut.unlock();
            if(currentAction != KeyboardAction::NO_ACTION){
                if(currentAction == KeyboardAction::EXIT_KEY) {
                    //todo idk if it is possible to use sendMessage function here
                    if (!sendExitMessage(threadData)) {
                        keepRunning = false;
                        break;
                    }
                    threadData->userInLobby = false;
                    threadData->userInGame = false;
                    threadData->isSpectator = false;
                }

                int numberKey = keyboardActionToInt(currentAction);
                if (numberKey != -1 and threadData->gameMode == Modes::Royal_) {
                    if (!handleMalBonMessageSending(threadData, numberKey, chosenPlayer, chosenMalus)) {
                        keepRunning = false;
                        break;
                    }
                }
                else {
                    InputMessage inputMessage;
                    inputMessage.input = currentAction;
                    if (!sendMessage(threadData->socket, MessageType::T_Input, inputMessage)) {
                        perror("Error sending input message");
                        keepRunning = false;
                        break;
                    }
                }
            }
        }
    }
    return 0;
}




/**
 * @brief Generic function to send a message to the server
 *
 * @tparam T Type of message data to send
 * @param socket Socket to send the message on
 * @param messageType Type of message (MessageType enum)
 * @param messageData data of certain type or struct to send
 * @return bool Success status of the send operation
 */
template<typename T>
bool Player::sendMessage(int socket, int messageType, const T& messageData) {
    int dataSize = sizeof(T);
    if (!sendAll(socket, &messageType, sizeof(int))) { return false;}
    if (!sendAll(socket, &dataSize, sizeof(int))) { return false;}
    if (!sendAll(socket, &messageData, dataSize)) { return false;}
    return true;
}

bool Player::sendExitMessage(shared_ptr<ThreadData> threadData) {
    int msgType = MessageType::T_Exit;
    int msgSize = 0;
    if (!sendAll(threadData->socket, &msgType, sizeof(msgType))){return false;}
    if (!sendAll(threadData->socket, &msgSize, sizeof(msgSize))){return false;}
    return true;
}

bool Player::handleMalBonMessageSending(shared_ptr<ThreadData> threadData,
                                                 int numberKey,
                                                 int& chosenPlayer,
                                                 int& chosenMalus
    ) {
    if (threadData->targetPlayerMenu) {
        // if player choses target player for malus
        chosenPlayer = numberKey;
        threadData->targetPlayerMenu = false;
        //send a malus
        MalBonus currentMalBonus;
        currentMalBonus.malusB = intToMalusBonus(chosenMalus);
        currentMalBonus.target = chosenPlayer;// todo choose playerid and check  if it is not player himeself on server
        if (!sendMessage(threadData->socket, MessageType::T_MalusBonus, currentMalBonus)) {
            perror("Error sending malusbonus message");
            return false;
        }
        chosenMalus = 0; chosenPlayer = 0;
    } else {
        // if player choses malus or bonus
        //todo perform energy price check (on the server probably)
        chosenMalus = numberKey;
        if (chosenMalus == 6 or chosenMalus == 7) {
            //todo check if it is possible to change int to malusbonus earlier
            // if malus is bonus, no need for target player
            //send a bonus
            chosenPlayer = threadData->clinetPlayerID; // to himself
            MalBonus currentMalBonus;
            currentMalBonus.malusB = intToMalusBonus(chosenMalus);
            currentMalBonus.target = chosenPlayer;
            if (!sendMessage(threadData->socket, MessageType::T_MalusBonus, currentMalBonus)) {
                perror("Error sending malusbonus message");
                return false;
            }
            chosenMalus = 0; chosenPlayer = 0;
        }else {
            // if malus is malus, player needs to choose target player
            threadData->targetPlayerMenu = true;
        }
    }
    return true;
}


/**
 * @brief Convert keyboard action to integer value
 *
 * @param action KeyboardAction to convert
 * @return int Integer value for number keys (0-9), or -1 for other keys
 */
int Player::keyboardActionToInt(KeyboardAction action) {
    if (action >= NUMBER_KEY_0 && action <= NUMBER_KEY_9) {
        return action - NUMBER_KEY_0 ;  // Convert to 0-9
    } else {
        return -1;  // Not a number key
    }
}





MalusBonus Player::intToMalusBonus(int malus) {
    //todo reimagine this crap hardcoded thingy
    switch(malus) {
        case 1:
            return MalusIncreaseFallSpeed;
        case 2:
            return MalusReverseControls;
        case 3:
            return MalusFreezeControls;
        case 4:
            return MalusBlackOut;
        case 5:
            return MalusThunder;
        case 6:
            return Bonus_Block1X1;
        case 7: 
            return BonusReduceFallSpeed;
        default:
            return MalusNONE;
    }
}