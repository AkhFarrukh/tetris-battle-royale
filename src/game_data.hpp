#ifndef GAME_DATA_HPP
#define GAME_DATA_HPP


#include <iostream>
#include <string>


class GameData{
    
    private:
    int score;
    int numberBrokenLines;
    std::string gameMode;    //pour la partie 1: Mode "endless" sans toutes ses fonctionnalités
    bool isGameOver;

    public:
    GameData(int score, int numberLinesBroken, bool isGameOver, const std::string& gameMode);
    
    int get_score() const;
    int get_numberBrokenLines() const;
    bool get_isGameOver() const;
    std::string get_gameMode() const;

    void set_score(int newScore);
    void set_numberBrokenLines(int new_numberBrokenLines);
    void set_isGameOver(bool gameStatus);
    void set_gameMode(const std::string& newGameMode);   // pas utile pour partie 1 

    void updateScore(int points);
    void showData() const;  

};

#endif // GAME_DATA_HPP


