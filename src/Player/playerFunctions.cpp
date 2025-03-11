#include "playerFunctions.hpp"

/**
 * @brief Routine capturing all the thing the server sends, and if it's a player state, show it
 * 
 * @param arg ThreadData, containing socket and bool for the loop capturing inputs
 * @return void* 
 */
void* Player::threadRoutineView (void* arg){

    std::cout<<"Starting threadRoutineView"<<std::endl;
    Player::ThreadData* threadData = static_cast<Player::ThreadData*>(arg);

    bool userIsInGame = threadData->userInGame;
    int sock = threadData->socket;


    //pr l'instant
    RenderEngineNcurses render;
    std::cout<<"Starting threadRoutineView loop"<<std::endl;
    while (userIsInGame) {
        //int bytesReceived = recv(client.client_fd, buffer, sizeof(buffer), 0);
        //To have info on the data received
        int type;
        size_t vectorNPDsize;
        int size;
        //Sending info to server
        /* PROBABLY NEED TO PUT A MUTEX ON SOCKET, SO WE DON'T READ AND WRITE AT THE SAME TIME */
        int bytesReceivedType = recv (sock, &type, sizeof(int), 0);
        int bytesReceivedSize = recv (sock, &size, sizeof(size), 0);
        //recv (sock, &size, sizeof(int), 0);
        std::cout<<"Received a message in threadRoutineView"<<std::endl;
        //Client disconnected or error
        if (bytesReceivedType <= 0 || bytesReceivedSize <= 0) {
            perror("Client disconnected : ");
            userIsInGame = false; // Stop the loop
        }

        else if(type == MessageType::T_PlayerData){
            vector<NetPlayerData> playersData;
            for (int i = 0; i < size; ++i) {
                NetPlayerData player;
            
                recvAll(sock, &player.playerId, sizeof(player.playerId));
                recvAll(sock, &player.isGameOver, sizeof(player.isGameOver));
                recvAll(sock, &player.linesCleared, sizeof(player.linesCleared));
                recvAll(sock, &player.score, sizeof(player.score));
            
                int gridSize;
                recvAll(sock, &gridSize, sizeof(gridSize));
            
                player.grid.resize(gridSize);
                if (gridSize > 0)
                    recvAll(sock, player.grid.data(), gridSize * sizeof(Color));
            
                playersData.push_back(player);
            }            
            //vector<NetPlayerData> playersData(size);
            //if (!recvAll(sock, playersData.data(), vectorNPDsize)) {
            //    userIsInGame = false;
            //    break;
            //}
            //std::cout<<"Done playersData, and size is : "<< playersData.size() <<std::endl;
            ////for (int i = 0; i < numPlayers; i++) {
            ////    shared_ptr<NetPlayerData> playerData;
            ////    if (!recvAll(sock, &playerData, sizeof(playerData))) {
            ////        userIsInGame = false;
            ////        break;
            ////    }
            ////    playersData.push_back(playerData);
            ////}
            if (playersData.size() <= 1){
                render.renderGames(playersData);
                std::cout<<"Rendered games"<<std::endl;
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
int Player::sendInput (std::shared_ptr<Player::ThreadData> threadData){
  printf("looofff\n");
    while(threadData->userInGame){
        /*
        *NEED TO CAPTURE INPUTS, AND SEND IT TO THE SERV (Input::start,   Input::getKeyboardInput() ?)
        */
        
    }
    
}