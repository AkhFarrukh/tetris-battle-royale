#include <iostream>
#include "game_data.hpp"



GameData::GameData()
:score(score), numberBrokenLines(numberBrokenLines), isGameOver(isGameOver), gameMode(gameMode) {}

int GameData::get_score() const{
    return score;
}

int GameData::get_numberBrokenLines() const{
    return numberBrokenLines;
}

bool GameData::get_isGameOver() const{
    return isGameOver;
}

std::string GameData::get_gameMode() const{
    return gameMode;
}

void GameData::set_score(int newScore){
    score = newScore;
}

void GameData::set_numberBrokenLines(int new_numberBrokenLines ){
    numberBrokenLines = new_numberBrokenLines;
}

void GameData::set_isGameOver(bool gameStatus){
    isGameOver = gameStatus;
}

void GameData::set_gameMode(const std::string& newGameMode){
    gameMode = newGameMode;
}

void GameData::updateScore(int points){
     score = score + points;
    }

void GameData::showData(){
    /*
    std::cout<< "Mode:  "<< gameMode << "\n\n";
    std::cout<< "SCORE: "<< score << std::endl;
    std::cout<< "LINES: " << numberBrokenLines << std::endl;
    */
}


int main(){
    // un main pour que je puisse compiler 
}


