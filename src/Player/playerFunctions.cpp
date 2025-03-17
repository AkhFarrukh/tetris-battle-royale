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
        if(inGame){
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
            
            threadData->displayMut.lock();
            render.renderGames(playersData);
            threadData->displayMut.unlock();
            //std::cout<<"Rendered games"<<std::endl;
            
            /*
            }
            else {
                render.renderGames(playersData);
            }
            */

            /* todo add when royal mode is implemented
            if (gamemode == Modes::Royal_){
                //std::cout<<"Rendering royal games"<<std::endl;
                render.renderGamesRoyal(playersData, threadData->targetPlayerMenu);
            }
            */
        }
    }
    else{ //joueur n'est pas en game
        int type;
        int size;
        if(!recvAll(sock, &type, sizeof(int))){
            flag = true;
            break;
        }
        if(!recvAll(sock, &size, sizeof(int))){
            flag = true;
            break;
        }
        if (type == MessageType::T_ForcedExit){
            flag = true;
            break;
        }
        else if (type == MessageType::T_GameStart) {
            threadData->mux.lock();
            threadData->userInGame = true;
            threadData->userInLobby = true;
            threadData->mux.unlock();
            continue;
        }
        else if (type == MessageType::T_RoomInfo) {
            RenderRooms renderR;
            RoomInfo info;
            if (!recvAll(sock, &info, size)) {
                perror("Error on receiving room");
                break;
            }

            int flags = fcntl(sock, F_GETFL, 0);
            fcntl(sock, F_SETFL, flags | O_NONBLOCK);

            int act = 0;
            while (!flag) {
                threadData->displayMut.lock();
                act = renderR.handleMyRoom(info);
                threadData->displayMut.unlock();

                if (act == 1) {
                    int sendType = MessageType::T_Menu;
                    int sendSize = sizeof(Menu);
                    Menu menu;
                    menu.action = MenuActions::A_ForceStart;

                    if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                    if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                    if(!sendAll(sock, &menu, sizeof(Menu))){flag = true; break;}

                } else if (act == -2) {
                    int sendType = MessageType::T_Refresh;
                    int sendSize = sizeof(int);

                    if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                    if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                    break;
                } else if (act == -1) {
                    int sendType = MessageType::T_RoomDeleted;
                    int sendSize = sizeof(int);
                    if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                    if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                    threadData->mux.lock();
                    threadData->userInLobby = false;
                    threadData->mux.unlock();
                    break;
                }

                int messageType;
                int messageSize;

                int bytesReceivedType = recv(sock, &messageType, sizeof(int), MSG_DONTWAIT);

                if (bytesReceivedType > 0) {
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
                } else if (bytesReceivedType == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
                    perror("Socket error");
                    flag = true; break;
                }
                else if((errno == EAGAIN && errno == EWOULDBLOCK) && act == 1){printw("Not enough players !\n"); refresh(); napms(500);}
                usleep(500);
            }

            fcntl(sock, F_SETFL, flags);
         }

        else if (type == MessageType::T_RoomsInfo){
            RenderRooms renderR;
            std::vector<RoomInfo> roomsInfo;
            roomsInfo.resize(size);
            if (!recvAll(sock, roomsInfo.data(), size *sizeof(RoomInfo)))
            {
                break;
            }
            clear();
            printw("Got it !");
            refresh();
            int index = -3;
            while (true) {
                threadData->displayMut.lock();
                index = renderR.handleListRooms(roomsInfo);
                threadData->displayMut.unlock();
                if (index == -3) {
                    continue;
                }
                if ((index != -2) && (index != -1)) {
                    int chosenIndex = index;
                    int typeToSend = MessageType::T_Join;
                    int sizeToSend = sizeof(int);
                    int fd = roomsInfo[index].roomID;

                    if (!sendAll(sock, &typeToSend, sizeof(typeToSend))) {return nullptr;}
                    if (!sendAll(sock, &sizeToSend, sizeof(sizeToSend))) {return nullptr;}
                    if (!sendAll(sock, &fd, sizeof(int))) {return nullptr;}
                    break;
                }
                if (index == -2) {
                    int type = MessageType::T_Menu;
                    int size = sizeof(Menu);
                    Menu menu;
                    menu.mode = roomsInfo[0].mode;
                    menu.action = MenuActions::A_JoinRoom;


                    if(sendAll(sock, &type, sizeof(int)) == false){
                        perror("Error sending menu info : ");
                        return 0;
                    }
                    if(sendAll(sock, &size, sizeof(int)) == false){
                        perror("Error sending menu info : ");
                        return 0;
                    }
                    if(sendAll(sock, &menu, sizeof(Menu)) == false){
                        perror("Error sending menu info : ");
                        return 0;
                    }
                    break;
                }
                if (index == -1) {
                    return 0;
                }
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
            if (numberKey != -1) {
                if (threadData->targetPlayerMenu) {
                    chosenPlayer = numberKey;
                    threadData->mux.lock();
                    threadData->targetPlayerMenu = true;
                    threadData->mux.unlock();

                } else {
                    chosenMalus = numberKey;
                    threadData->mux.lock();
                    threadData->targetPlayerMenu = false;
                    threadData->mux.unlock();
                    //
                    //todo send chosenMalus and chosenPlayer to server
                    //
                    chosenMalus = 0; chosenMalus = 0;
                }
            }
            else {
                int type = MessageType::T_Input;
                int size = sizeof(InputMessage);
                InputMessage inputMessage;
                inputMessage.input = currentAction;
                if (!sendAll(threadData->socket, &type, sizeof(int))) {return -1;}
                if (!sendAll(threadData->socket, &size, sizeof(int))) {return -1;}
                if (!sendAll(threadData->socket, &inputMessage, sizeof(InputMessage))) {return -1;}
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