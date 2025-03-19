#include "playerFunctions.hpp"
#include "input.hpp"
#include <fcntl.h>

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
    while (!flag) {
        threadData->mux.lock();
        bool inGame = threadData->userInGame;
        threadData->mux.unlock();
        //int bytesReceived = recv(client.client_fd, buffer, sizeof(buffer), 0);
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
            threadData->mux.lock();
            threadData->userInGame = false; // Stop the loop
            threadData->mux.unlock();
            flag = true;
            break;
        }
        else if(type == MessageType::T_Exit){
            threadData->mux.lock();
            threadData->userInGame = false;
            threadData->userInLobby = false;
            threadData->mux.unlock();
            napms(300);
            continue;
        }
        else if(type == MessageType::T_PlayerData){
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

                playersData.push_back(player);
            }
            

            //std::cout<<"Rendered games"<<std::endl;
            
            /*
            }
            else {
                render.renderGames(playersData);
            }
            */


            if (threadData->gameMode == Modes::Royal_){
                //std::cout<<"Rendering royal games"<<std::endl;
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
            flag = true;
            napms(300);
            break;
        }
        else if (type == MessageType::T_GameStart) {
            threadData->mux.lock();
            threadData->userInGame = true;
            threadData->userInLobby = true;
            threadData->mux.unlock();
            continue;
        }
        else if (type == MessageType::T_RoomInfo || type == MessageType::T_RoomJoinInfo) {
            RenderRooms renderR;
            RoomInfo info;
            if (!recvAll(sock, &info, size)) {
                perror("Error on receiving room");
                flag = true;
                break;
            }
            int flags = fcntl(sock, F_GETFL, 0);
            fcntl(sock, F_SETFL, flags | O_NONBLOCK);

            int act = 0;
            while (!flag) {
                threadData->displayMut.lock();
                act = renderR.handleMyRoom(info);
                threadData->displayMut.unlock();

                if (act == 1 && type == MessageType::T_RoomInfo) {
                    int sendType = MessageType::T_Menu;
                    int sendSize = sizeof(Menu);
                    Menu menu;
                    menu.action = MenuActions::A_ForceStart;

                    if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                    if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                    if(!sendAll(sock, &menu, sizeof(Menu))){flag = true; break;}

                } else if (act == -2) {
                    if(type == MessageType::T_RoomInfo){
                        int sendType = MessageType::T_Refresh;
                        int sendSize = sizeof(int);

                        if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                        if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                        break;
                    }
                    else{
                        int sendType = MessageType::T_RoomJoinRefresh;
                        int sendSize = sizeof(int);

                        if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                        if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                        break;
                    }
                } else if (act == -1) {
                    if(type == MessageType::T_RoomInfo){
                        int sendType = MessageType::T_RoomDeleted;
                        int sendSize = sizeof(int);
                        if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                        if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                        threadData->mux.lock();
                        threadData->userInLobby = false;
                        threadData->mux.unlock();
                        break;
                    }
                    else{
                        int sendType = MessageType::T_LeftRoom;
                        int sendSize = sizeof(int);
                        if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                        if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                        threadData->mux.lock();
                        threadData->userInLobby = false;
                        threadData->mux.unlock();
                        break;
                    }
                }

                int messageType;
                int messageSize;

                int bytesReceivedType = recv(sock, &messageType, sizeof(int), MSG_DONTWAIT);

                if (bytesReceivedType > 0) {
                    //printf("yuyyuyuu\n");
                    recv(sock, &messageSize, sizeof(int), 0);

                    if (messageType == MessageType::T_GameStart) {
                        threadData->mux.lock();
                        threadData->userInGame = true;
                        threadData->mux.unlock();
                        break;
                    }
                    else if(messageType == MessageType::T_ForcedExit){
                        threadData->mux.lock();
                        threadData->userInGame = false;
                        threadData->userInLobby = false;
                        threadData->mux.unlock();
                        flag = true;
                        break;
                    }
                    else if(messageType == MessageType::T_Exit){
                        threadData->mux.lock();
                        threadData->userInGame = false;
                        threadData->userInLobby = false;
                        threadData->mux.unlock();
                        break;
                    }
                } else if (bytesReceivedType == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
                    perror("Socket error");
                    flag = true; break;
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
            {flag = true; break;}
            Modes mode;
            int size;
            if(!recvAll(sock, &mode, sizeof(int))){flag = true; break;}
            if(!recvAll(sock, &size, sizeof(int))){flag = true;break;}
            clear();
            refresh();
            int index;
            int highlightIndex = 0;
            while (!flag) {
                threadData->displayMut.lock();
                index = renderR.handleListRooms(roomsInfo, highlightIndex);
                threadData->displayMut.unlock();
                if (index == -3) {
                    continue;
                }
                if ((index != -2) && (index != -1)) {
                    int chosenIndex = index;
                    int typeToSend = MessageType::T_Join;
                    int sizeToSend = sizeof(int);
                    int fd = roomsInfo[index].roomID;

                    if (!sendAll(sock, &typeToSend, sizeof(typeToSend))) {flag = true; break;}
                    if (!sendAll(sock, &sizeToSend, sizeof(sizeToSend))) {flag = true; break;}
                    if (!sendAll(sock, &fd, sizeof(int))) {flag = true; break;}
                    threadData->mux.lock();
                    threadData->userInLobby = true;
                    threadData->mux.unlock();
                    break;
                }
                if (index == -2) {
                    int type = MessageType::T_Menu;
                    int size = sizeof(Menu);
                    Menu menu;
                    menu.mode = mode;
                    menu.action = MenuActions::A_JoinRoom;


                    if(sendAll(sock, &type, sizeof(int)) == false){
                        perror("Error sending menu info : ");
                        flag = true; break;
                    }
                    if(sendAll(sock, &size, sizeof(int)) == false){
                        perror("Error sending menu info : ");
                        flag = true; break;
                    }
                    if(sendAll(sock, &menu, sizeof(Menu)) == false){
                        perror("Error sending menu info : ");
                        flag = true; break;
                    }
                    break;
                }
                if (index == -1) {
                    threadData->mux.lock();
                    threadData->userInLobby = false;
                    threadData->mux.unlock();
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
    threadData->mux.lock();
    bool inLobby = threadData->userInLobby;
    threadData->mux.unlock();
    while (!flag && inLobby) {
        threadData->mux.lock();
        bool keepRunning = threadData->userInGame;
        inLobby = threadData->userInLobby;
        threadData->mux.unlock();
        if (!keepRunning && !inLobby) {
            break;
        }
        else if(inLobby && !keepRunning)continue;
        else{
          threadData->displayMut.lock();
        KeyboardAction currentAction = Input::getKeyboardInput();
        threadData->displayMut.unlock();
        if(currentAction != KeyboardAction::NO_ACTION){
            if(currentAction == KeyboardAction::EXIT_KEY) {
                threadData->mux.lock();
                threadData->userInLobby = false;
                threadData->userInGame = false;
                threadData->mux.unlock();
            }

            int numberKey = keyboardActionToInt(currentAction);
            if (numberKey != -1 and threadData->gameMode == Modes::Royal_) {
                if (threadData->targetPlayerMenu) {
                    // if player choses target player for malus
                    chosenPlayer = numberKey;
                    threadData->mux.lock();
                    threadData->targetPlayerMenu = false;
                    threadData->mux.unlock();
                    //send a malus
                    int type = MessageType::T_MalusBonus;
                    int size = sizeof(MalBonus);
                    MalBonus currentMalBonus;
                    currentMalBonus.malusB = intToMalusBonus(chosenMalus);
                    currentMalBonus.target = chosenPlayer;// todo choose playerid and check  if it is not player himeself on server
                    if (!sendAll(threadData->socket, &type, sizeof(int))) { flag = true; break;}
                    if (!sendAll(threadData->socket, &size, sizeof(int))) { flag = true; break;}
                    if (!sendAll(threadData->socket, &currentMalBonus, sizeof(MalBonus))) { flag = true; break;}
                    chosenMalus = 0; chosenPlayer = 0;
                } else {
                    // if player choses malus or bonus
                    //todo perform energy price check (on the server probably)
                    chosenMalus = numberKey;
                    if (chosenMalus == 6 or chosenMalus == 7) {
                        // if malus is bonus, no need for target player
                        //send a bonus
                        chosenPlayer = threadData->clinetPlayerID; // to himself
                        int type = MessageType::T_MalusBonus;
                        int size = sizeof(MalBonus);
                        MalBonus currentMalBonus;
                        currentMalBonus.malusB = intToMalusBonus(chosenMalus);
                        currentMalBonus.target = chosenPlayer;
                        if (!sendAll(threadData->socket, &type, sizeof(int))) {flag = true; break;}
                        if (!sendAll(threadData->socket, &size, sizeof(int))) {flag = true; break;}
                        if (!sendAll(threadData->socket, &currentMalBonus, sizeof(MalBonus))) {flag = true; break;}
                        chosenMalus = 0; chosenPlayer = 0;
                    }else {
                        // if malus is malus, player needs to choose target player
                        threadData->mux.lock();
                        threadData->targetPlayerMenu = true;
                        threadData->mux.unlock();
                    }
                }
            }
            else {
                int type = MessageType::T_Input;
                int size = sizeof(InputMessage);
                InputMessage inputMessage;
                inputMessage.input = currentAction;
                if (!sendAll(threadData->socket, &type, sizeof(int))) { flag = true; break;}
                if (!sendAll(threadData->socket, &size, sizeof(int))) { flag = true; break;}
                if (!sendAll(threadData->socket, &inputMessage, sizeof(InputMessage))) { flag = true; break;}
            }
        }}
    }
    return 0;
}



/**
 * @brief Convert keyboard action to integer value
 *
 * @param action KeyboardAction to convert
 * @return int Integer value for number keys (0-9), or -1 for other keys
 */
int Player::keyboardActionToInt(KeyboardAction action) {
    switch(action) {
        case NUMBER_KEY_0:
            return 0;
        case NUMBER_KEY_1:
            return 1;
        case NUMBER_KEY_2:
            return 2;
        case NUMBER_KEY_3:
            return 3;
        case NUMBER_KEY_4:
            return 4;
        case NUMBER_KEY_5:
            return 5;
        case NUMBER_KEY_6:
            return 6;
        case NUMBER_KEY_7:
            return 7;
        case NUMBER_KEY_8:
            return 8;
        case NUMBER_KEY_9:
            return 9;
        default:
            return -1;
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
        default:
            return MalusNONE;
    }
}