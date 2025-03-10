#include "playerFunctions.hpp"

/**
 * @brief Routine capturing all the thing the server sends, and if it's a player state, show it
 * 
 * @param arg ThreadData, containing socket and bool for the loop capturing inputs
 * @return void* 
 */
void* Player::threadRoutineView (void* arg){
    shared_ptr<ThreadData> threadData(reinterpret_cast<ThreadData*>(arg));
    bool userIsInGame = threadData->userInGame;
    int sock = threadData->socket;
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
        
        else if(type == MessageType::T_PlayerState){
            Game::PlayerState playerState;
            if (!recvAll(sock, &playerState, sizeof(Game::PlayerState))){
                //Repetition de code, pas ouf, need method
                perror("Client disconnected : ");
                userIsInGame = false; // Stop the loop
            }
            /*
            *NEED THE FONCTION SHOWING THE GAME WITH PLAYERSTATE AS AN ARG
            */
        }
    }
    
}

/**
 * @brief Capture the inputs and send it to the server
 * 
 * @param threadData containing socket & bool for the loop
 * @return int 
 */
int Player::sendInput (ThreadData& threadData){
    while(threadData.userInGame){
        /*
        *NEED TO CAPTURE INPUTS, AND SEND IT TO THE SERV (Input::start,   Input::getKeyboardInput() ?)
        */
        
    }
    
}