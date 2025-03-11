#include "playerFunctions.hpp"

/**
 * @brief Routine capturing all the thing the server sends, and if it's a player state, show it
 * 
 * @param arg ThreadData, containing socket and bool for the loop capturing inputs
 * @return void* 
 */
void* Player::threadRoutineView (void* arg){

    std::shared_ptr<Player::ThreadData>* threadDataPtr = reinterpret_cast<std::shared_ptr<Player::ThreadData>*>(arg);
    std::shared_ptr<Player::ThreadData> threadData = *threadDataPtr;
    bool userIsInGame = threadData->userInGame;
    int sock = threadData->socket;

    printf("sock = %d\n", sock);
    printf("loioiiool4\n");
    //pr l'instant
    RenderEngineNcurses render;
    while (userIsInGame) {
        //int bytesReceived = recv(client.client_fd, buffer, sizeof(buffer), 0);
        //To have info on the data received
        int type;
        int size;
        //Sending info to server
        /* PROBABLY NEED TO PUT A MUTEX ON SOCKET, SO WE DON'T READ AND WRITE AT THE SAME TIME */
        int bytesReceivedType = recv (sock, &type, sizeof(int), 0);
        int bytesReceivedSize = recv (sock, &size, sizeof(int), 0);
        //Client disconnected or error
        if (bytesReceivedType <= 0 || bytesReceivedSize <= 0) {
            perror("Client disconnected : ");
            userIsInGame = false; // Stop the loop
        }
        
        else if(type == MessageType::T_PlayerData){
            int numPlayers;
            if (!recvAll(sock, &numPlayers, sizeof(numPlayers))) {
                userIsInGame = false;
                break;
            }
            vector<shared_ptr<NetPlayerData>> playersData;
            for (int i = 0; i < numPlayers; i++) {
                shared_ptr<NetPlayerData> playerData;
                if (!recvAll(sock, &playerData, sizeof(playerData))) {
                    userIsInGame = false;
                    break;
                }
                playersData.push_back(playerData);
            }
            if (numPlayers <= 1){
                render.renderGame(playersData[0]);
            }
            else {
                render.renderGames(playersData);
            }
        }
    }

}

/**
 * @brief Capture the inputs and send it to the server
 * 
 * @param threadData containing socket & bool for the loop
 * @return int 
 */
int Player::sendInput (std::shared_ptr<ThreadData> threadData){
  printf("looofff\n");
    while(threadData->userInGame){
        /*
        *NEED TO CAPTURE INPUTS, AND SEND IT TO THE SERV (Input::start,   Input::getKeyboardInput() ?)
        */
        
    }
    
}