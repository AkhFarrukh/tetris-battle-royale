#include "playerFunctions.hpp"
#include "input.hpp"

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


    //pr l'instant
    RenderEngineNcurses render;
    //std::cout<<"Starting threadRoutineView loop"<<std::endl;
    //clear();
    //refresh();
    while (threadData->userInGame && !flag) {
        //int bytesReceived = recv(client.client_fd, buffer, sizeof(buffer), 0);
        //To have info on the data received
        int type;
        int size;
        //Sending info to server
        /* PROBABLY NEED TO PUT A MUTEX ON SOCKET, SO WE DON'T READ AND WRITE AT THE SAME TIME */
        int bytesReceivedType = recvAll(sock, &type, sizeof(int));
        int bytesReceivedSize = recvAll(sock, &size, sizeof(int));
        //recv (sock, &size, sizeof(int), 0);
        //std::cout<<"Received a message in threadRoutineView"<<std::endl;
        //Client disconnected or error
        if (bytesReceivedType <= 0 || bytesReceivedSize <= 0) {
            perror("Client disconnected : ");
            threadData->mux.lock();
            threadData->userInGame = false; // Stop the loop
            threadData->mux.unlock();
        }
        else if(type == MessageType::T_Exit){
            threadData->mux.lock();
            threadData->userInGame = false;
            threadData->mux.unlock();
        }
        else if(type == MessageType::T_PlayerData){
            vector<NetPlayerData> playersData;
            for (int i = 0; i < size; ++i) {
                NetPlayerData player;
                /* NEED ERROR HANDLING */
                recvAll(sock, &player.playerId, sizeof(player.playerId));
                recvAll(sock, &player.isGameOver, sizeof(player.isGameOver));
                recvAll(sock, &player.linesCleared, sizeof(player.linesCleared));
                recvAll(sock, &player.score, sizeof(player.score));
            
                int gridSize;
           	recvAll(sock, &gridSize, sizeof(gridSize));

		if (gridSize <= 0 || gridSize > 100000) {  // juste pour detecter quand gridSize devient trop grand
    			//std::cerr << "Error: Invalid gridSize received: " << gridSize << std::endl;
    			break;
		}

                player.grid.resize(gridSize);

                recvAll(sock, player.grid.data(), gridSize * sizeof(Color));
            
                playersData.push_back(player);
            }            
            if (playersData.size() <= 1){
                threadData->displayMut.lock();
                render.renderGames(playersData);
                threadData->displayMut.unlock();
                //std::cout<<"Rendered games"<<std::endl;
            }
            else {
                render.renderGames(playersData);
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

int Player::sendInput (std::shared_ptr<Player::ThreadData> threadData){
    while (!flag) {
        bool keepRunning = threadData->userInGame;
        if (!keepRunning) {
            break;
        }
        threadData->displayMut.lock();
        KeyboardAction currentAction = Input::getKeyboardInput();
        threadData->displayMut.unlock();
        if(currentAction != KeyboardAction::NO_ACTION){
            if(currentAction == KeyboardAction::EXIT_KEY) {
                threadData->mux.lock();
                threadData->userInGame = false;
                threadData->mux.unlock();
            }
            int type = MessageType::T_Input;
            int size = sizeof(InputMessage);
            InputMessage inputMessage;
            inputMessage.input = currentAction;
	        sendAll(threadData->socket, &type, sizeof(int));
            sendAll(threadData->socket, &size, sizeof(int));
            sendAll(threadData->socket, &inputMessage, sizeof(InputMessage));
        }
    }
    return 0;
}
