#include "playerFunctions.hpp"

#include <utility>


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
        int messageType;
        int messageSize;
        //Sending info to server
        /* PROBABLY NEED TO PUT A MUTEX ON SOCKET, SO WE DON'T READ AND WRITE AT THE SAME TIME */
        int bytesReceivedType = recvAll(sock, &messageType, sizeof(int));
        int bytesReceivedSize = recvAll(sock, &messageSize, sizeof(int));
        //Client disconnected or error
        if (bytesReceivedType <= 0 || bytesReceivedSize <= 0) {
            perror("Client disconnected : ");
            threadData->userInGame = false; // Stop the loop
            keepRunning = false;
            break;
        }
        else if(messageType == MessageType::T_Message){
            Message message;
            if (!recvAll(sock, &message, messageSize)) {
                perror("Error on receiving room");
                keepRunning = false;
                break;
            }
            affichageMessage(message);
        }
        else if(messageType == MessageType::T_Exit){
            threadData->userInGame = false;
            threadData->userInLobby = false;
            napms(300);
            continue;
        }
        else if(messageType == MessageType::T_PlayerData || messageType == MessageType::T_EndGame){
            if (!handlePlayerDataEndgameMessage(sock, threadData, messageSize, messageType, render)) {
                keepRunning = false;
                break;
            }
        }
        if (messageType == MessageType::T_ForcedExit){
            keepRunning = false;
            napms(300);
            break;
        }
        else if (messageType == MessageType::T_GameStart) {
            threadData->userInGame = true;
            threadData->userInLobby = true;
            continue;
        }
        else if (messageType == MessageType::T_RoomInfo || messageType == MessageType::T_RoomJoinInfo) {
            if (!roomJoinInfoHandler(sock, threadData, messageSize, messageType)) {
                break;
            }
         }
        else if (messageType == MessageType::T_RoomsInfo){
            if (!roomJoinHandler(sock, threadData, messageSize)) {
                break;
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
                    if (!handleStandardInput(threadData, currentAction)) {
                        break;
                    }
                }
            }
        }
    }
    return 0;
}


/**
 * @brief Handle standard input actions and send them to the server
 *
 * @param threadData ThreadData containing socket and player information
 * @param action KeyboardAction to handle
 * @return bool Success status of the operation
 */
bool Player::handleStandardInput(std::shared_ptr<Player::ThreadData> threadData, KeyboardAction action) {
    InputMessage inputMessage;
    inputMessage.input = action;
    if (!sendMessageWData(threadData->socket, MessageType::T_Input, inputMessage)) {
        perror("Error sending input message");
        keepRunning = false;
        return false;
    }
    return true;
}


/**
 * @brief Handle player data and endgame messages
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param messageSize Size of the message received
 * @param messageType Type of the message received
 * @param render RenderEngineNcurses object for rendering
 * @return bool Success status of the operation
 */
bool Player::handlePlayerDataEndgameMessage(int socket, ThreadData* threadData, int messageSize, int messageType, RenderEngineNcurses& render) {
    vector<NetPlayerData> playersData;
    if (!receivePlayersData(socket, messageSize, messageType, playersData)) {
        return false;
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

    return true;
}


/**
 * @brief Receive player data from the server
 *
 * @param socket Socket to communicate with the server
 * @param messageSize Size of the message received
 * @param messageType Type of the message received
 * @param playersData Vector to store the received player data
 * @return bool Success status of the operation
 */
bool Player::receivePlayersData(int socket, int messageSize, int messageType, std::vector<NetPlayerData>& playersData) {
    for (int i = 0; i < messageSize; ++i) {
        NetPlayerData player;
        if (!recvAll(socket, &player.playerId, sizeof(player.playerId))){return false;}
        if (!recvAll(socket, &player.isGameOver, sizeof(player.isGameOver))){return false;}
        if (!recvAll(socket, &player.linesCleared, sizeof(player.linesCleared))){return false;}
        if (!recvAll(socket, &player.score, sizeof(player.score))){return false;}
        if (!recvAll(socket, &player.nrg, sizeof(player.nrg))) {return false;}

        int gridSize;
        if (!recvAll(socket, &gridSize, sizeof(gridSize))){return false;}

        if (gridSize <= 0 || gridSize > 100000) {  // juste pour detecter quand gridSize devient trop grand
            std::cerr << "Error: Invalid gridSize received: " << gridSize << std::endl;
            return false;
        }
        player.grid.resize(gridSize);

        if (!recvAll(socket, player.grid.data(), gridSize * sizeof(Color))){return false;}
        if(messageType == MessageType::T_EndGame)player.endGame = true;
        playersData.push_back(player);
    }

    return true;
}


/**
 * @brief Handle the room join process and display available rooms
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param messageSize Size of the message received
 * @return bool Success status of the operation
 */
bool Player::roomJoinHandler(int socket, ThreadData* threadData, int messageSize) {
    RenderRooms renderR;
    std::vector<RoomInfo> roomsInfo;
    roomsInfo.resize(messageSize);
    if (!recvAll(socket, roomsInfo.data(), messageSize *sizeof(RoomInfo))) {
        keepRunning = false;
        return false;
    }

    Modes mode;
    int size;
    if(!recvAll(socket, &mode, sizeof(int))) {
        keepRunning = false;
        return false;
    }
    if(!recvAll(socket, &size, sizeof(int))) {
        keepRunning = false;
        return false;
    }

    clear();
    refresh();

    return handleRoomsSelection(socket, threadData, roomsInfo, renderR, mode);
}


/**
 * @brief Handle the selection of rooms and user actions
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param roomsInfo Vector of available rooms
 * @param renderR RenderRooms object for rendering
 * @param mode Game mode (Royal, Endless, etc.)
 * @return bool Success status of the operation
 */
bool Player::handleRoomsSelection(int socket, ThreadData* threadData,
                                  std::vector<RoomInfo>& roomsInfo,
                                  RenderRooms& renderR,
                                  Modes mode) {
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
            return handleJoinRoom(socket, threadData, roomsInfo, index);
        }
        if (index == -2) {
            return sendMessageToMenu(socket, mode);
        }
        if (index == -1) {
            threadData->userInLobby = false;
            return true;
        }
    }

    return true;
}



/**
 * @brief Handle the joining of a room and send the necessary information to the server
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param roomsInfo Vector of available rooms
 * @param index Index of the selected room
 * @return bool Success status of the operation
 */
bool Player::handleJoinRoom(int socket, ThreadData* threadData, std::vector<RoomInfo>& roomsInfo, int index) {
    size_t N = roomsInfo.size();
    bool isSpectator = index >= (int)N;
    int roomIdx = isSpectator ? index - N : index;
    int typeToSend = isSpectator ? MessageType::T_JoinAsSpectator : MessageType::T_Join;
    int sizeToSend = sizeof(int);
    int fd = roomsInfo[roomIdx].roomID;

    if (!sendAll(socket, &typeToSend, sizeof(typeToSend))) {
        keepRunning = false;
        return false;
    }
    if (!sendAll(socket, &sizeToSend, sizeof(sizeToSend))) {
        keepRunning = false;
        return false;
    }
    if (!sendAll(socket, &fd, sizeof(int))) {
        keepRunning = false;
        return false;
    }

    threadData->mux.lock();
    threadData->isSpectator = isSpectator;
    threadData->userInLobby = true;
    threadData->mux.unlock();
    return true;
}


/**
 * @brief Send a message to the menu with the specified mode
 *
 * @param socket Socket to communicate with the server
 * @param mode Game mode (Royal, Endless, etc.)
 * @return bool Success status of the operation
 */
bool Player::sendMessageToMenu(int socket, Modes mode) {
    int type = MessageType::T_Menu;
    int size = sizeof(MenuMessage);
    MenuMessage menuMessage;
    menuMessage.mode = mode;
    menuMessage.action = MenuActions::A_JoinRoom;

    if(!sendAll(socket, &type, sizeof(int))) {
        perror("Error sending MenuMessage info : ");
        keepRunning = false;
        return false;
    }
    if(!sendAll(socket, &size, sizeof(int))) {
        perror("Error sending MenuMessage info : ");
        keepRunning = false;
        return false;
    }
    if(!sendAll(socket, &menuMessage, sizeof(menuMessage))) {
        perror("Error sending MenuMessage info : ");
        keepRunning = false;
        return false;
    }
    return true;
}



/**
 * @brief Handle the room join information and process user actions
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param messageSize Size of the message received
 * @param messageType Type of the message received
 * @return bool Success status of the operation
 */
bool Player::roomJoinInfoHandler(int socket,
                        ThreadData* threadData,
                        int messageSize,
                        int messageType) {
    int flags; // todo idk what it does
    //Setup and receive room info
    if (!setupRoomSession(socket, threadData, messageSize, flags)) {
        return false;
    }

    // Process room interactions in while loop
    return processRoomInteractions(socket, threadData, messageType, flags);
}



/**
 * @brief Setup the room session and receive room information
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param messageSize Size of the message received

 * @param flags Socket flags to be set
 * @return bool Success status of the operation
 */
bool Player::setupRoomSession(int socket, ThreadData* threadData, int messageSize, int& flags) {
    RoomInfo info;
    if (!recvAll(socket, &info, messageSize)) {
        perror("Error on receiving room");
        keepRunning = false;
        return false;
    }
    threadData->roomInfo = info;
    flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
    return true;
}

/**
 * @brief Process room interactions and handle user actions
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param messageType Type of the message received
 * @param flags Socket flags to be set
 * @return bool Success status of the operation
 */
bool Player::processRoomInteractions(int socket, ThreadData* threadData, int messageType, int& flags) {
    while (keepRunning) {
        // Handle user actions in room
        if (!handleRoomUserActions(socket, threadData, messageType)) {
            break;
        }

        // Check for server messages
        if (!checkForServerMessages(socket, threadData)) {
            break;
        }

        usleep(500);
    }

    // Restore socket flags
    fcntl(socket, F_SETFL, flags);
    return true;
}

/**
 * @brief Handle user actions in the room based on the message type
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param messageType Type of the message received
 * @return bool Success status of the operation
 */
bool Player::handleRoomUserActions(int socket, ThreadData* threadData, int messageType) {
    RenderRooms renderR; //room renderer function
    int action; //todo rename and fix its magic values

    if (threadData->isSpectator) {
        // ─── Spectator Waiting Room ───────────────────────
        action = renderR.handleSpectatorRoom(threadData->roomInfo);
        return handleRoomSpectatorActions(socket, threadData, action);
    } else {
        // ─── Player Lobby ───────────────────────────────────
        action = renderR.handleMyRoom(threadData->roomInfo);
        return handleRoomPlayerActions(socket, threadData, action, messageType);
    }
}


/**
 * @brief Handle actions for spectators in the waiting room
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param action Action performed by the spectator
 * @return bool Success status of the operation
 */
bool Player::handleRoomSpectatorActions(int socket, ThreadData *threadData, int action){
    //todo actions are magic numbers
    if (action == -2) {
        // player refreshes
        if (!sendMessage(socket, MessageType::T_Refresh)) {
            //todo maybe keepRunning = false; break;
        }
    } else if (action == -1) {
        // Go back to room list
        if (!sendMessage(socket, MessageType::T_LeftRoom)) {
            keepRunning = true;
            return false;
        }
        threadData->isSpectator = false;
        return false;
    }
    return true;
}


/**
 * @brief Handle actions for players in the lobby
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @param action Action performed by the player
 * @param messageType Type of the message received
 * @return bool Success status of the operation
 */
bool Player::handleRoomPlayerActions(int socket, ThreadData* threadData, int action, int messageType) {
    if (action == 1 && messageType == MessageType::T_RoomInfo) {
        // force start
        MenuMessage menuMessage;
        menuMessage.action = MenuActions::A_ForceStart;
        if (!sendMessageWData(socket, MessageType::T_Menu, menuMessage)) {
            keepRunning = false;
            return false;
        }

    } else if (action == -2) {
        // refresh
        int msgType = (messageType == MessageType::T_RoomInfo) ?
                      MessageType::T_Refresh :
                      MessageType::T_RoomJoinRefresh;
        if (!sendMessage(socket, msgType)) {
            keepRunning = false;
            return false;
        }
        return false;
    } else if (action == -1) {
        // exit
        int msgType = (messageType == MessageType::T_RoomInfo) ?
                      MessageType::T_RoomDeleted :
                      MessageType::T_LeftRoom;
        if (!sendMessage(socket, msgType)) {
            keepRunning = false;
            return false;
        }
        threadData->userInLobby = false;
        return false;
    }
    return true;
}

/**
 * @brief Check for server messages about starting or exiting the game
 *
 * @param socket Socket to communicate with the server
 * @param threadData ThreadData containing socket and player information
 * @return bool Success status of the operation
 */
bool Player::checkForServerMessages(int socket, ThreadData* threadData) {
    int receivedMessageType;
    int receivedMessageSize;
    int bytesReceivedType = recv(socket, &receivedMessageType, sizeof(int), MSG_DONTWAIT);
    if (bytesReceivedType > 0) {
        recv(socket, &receivedMessageSize, sizeof(int), 0);

        if (receivedMessageType == MessageType::T_GameStart) {
            threadData->userInGame = true;
            return false;
        } else if (receivedMessageType == MessageType::T_ForcedExit) {
            threadData->userInGame = false;
            threadData->userInLobby = false;
            keepRunning = false;
            return false;
        } else if (receivedMessageType == MessageType::T_Exit) {
            threadData->userInGame = false;
            threadData->userInLobby = false;
            return false;
        }
    } else if (bytesReceivedType == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
        perror("Socket error");
        keepRunning = false;
        return false;
    }

    return true;
}


/**
 * @brief Generic function to send a message without data to the server
 *
 * @param socket Socket to send the message on
 * @param messageType Type of message (MessageType enum)
 * @return bool Success status of the send operation
 */
bool Player::sendMessage(int socket, int messageType) {
    int dataSize = sizeof(int);
    if (!sendAll(socket, &messageType, sizeof(int))) { return false;}
    if (!sendAll(socket, &dataSize, sizeof(int))) { return false;}
    return true;
}

/**
 * @brief Generic function to send a message with a data to the server
 *
 * @tparam T Type of message data to send
 * @param socket Socket to send the message on
 * @param messageType Type of message (MessageType enum)
 * @param messageData data of certain type or struct to send
 * @return bool Success status of the send operation
 */
template<typename T>
bool Player::sendMessageWData(int socket, int messageType, const T& messageData) {
    int dataSize = sizeof(T);
    if (!sendAll(socket, &messageType, sizeof(int))) { return false;}
    if (!sendAll(socket, &dataSize, sizeof(int))) { return false;}
    if (!sendAll(socket, &messageData, dataSize)) { return false;}
    return true;
}


/**
 * @brief Send exit message to the server
 *
 * @param threadData ThreadData containing socket and player information
 * @return bool Success status of the operation
 */
bool Player::sendExitMessage(shared_ptr<ThreadData> threadData) {
    return sendMessage(threadData->socket, MessageType::T_Exit);
}


/**
 * @brief Send malus or bonus message to the server
 *
 * @param chosenMalus Reference to store the chosen malus
 * @param chosenPlayer Reference to store the chosen player
 * @param socket Socket to send the message on
 * @return bool Success status of the operation
 */
bool Player::sendMalBonMessage(int& chosenMalus, int& chosenPlayer, int socket) {
    MalBonus currentMalBonus;
    currentMalBonus.malusB = intToMalusBonus(chosenMalus);
    currentMalBonus.target = chosenPlayer;// todo choose playerid and check  if it is not player himeself on server
    if (!sendMessageWData(socket, MessageType::T_MalusBonus, currentMalBonus)) {
        perror("Error sending malusbonus message");
        return false;
    }
    chosenMalus = 0; chosenPlayer = 0;
    return true;
}


/**
 * @brief Handle sending malus or bonus messages to the server
 *
 * @param threadData ThreadData containing socket and player information
 * @param numberKey The key pressed by the player (0-9)
 * @param chosenPlayer Reference to store the chosen player
 * @param chosenMalus Reference to store the chosen malus
 * @return bool Success status of the operation
 */
bool Player::handleMalBonMessageSending(shared_ptr<ThreadData> threadData,
                                                 int& numberKey,
                                                 int& chosenPlayer,
                                                 int& chosenMalus
    ) {
    if (threadData->targetPlayerMenu) {
        // if player choses target player for malus
        chosenPlayer = numberKey;
        threadData->targetPlayerMenu = false;
        //send a malus
        if (!sendMalBonMessage(chosenMalus, chosenPlayer, threadData->socket)) {
            return false;
        }
    } else {
        // if player choses malus or bonus
        //todo perform energy price check (on the server probably)
        chosenMalus = numberKey;
        if (chosenMalus == 6 or chosenMalus == 7) {
            //todo check if it is possible to change int to malusbonus earlier
            // if malus is bonus, no need for target player
            //send a bonus
            chosenPlayer = threadData->clinetPlayerID; // to himself
            if (!sendMalBonMessage(chosenMalus, chosenPlayer, threadData->socket)) {
                return false;
            }
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




/**
 * @brief Convert integer value to MalusBonus enum
 *
 * @param malus Integer value representing a malus or bonus
 * @return MalusBonus Corresponding MalusBonus enum value
 */
MalusBonus Player::intToMalusBonus(int malus) {
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