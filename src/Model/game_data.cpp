#include "game_data.hpp"


GameData::GameData(int score, int numberLinesBroken, bool isGameOver, const std::string& gameMode) :
    score(score), numberBrokenLines(numberLinesBroken), isGameOver(isGameOver), gameMode(gameMode) {}

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

void GameData::updateLines(int lines){
    numberBrokenLines += lines;
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

void GameData::showData() const{
    
    std::cout<< "Mode:  "<< gameMode << "\n\n";
    std::cout<< "SCORE: "<< score << std::endl;
    std::cout<< "LINES: " << numberBrokenLines << std::endl;
    
}