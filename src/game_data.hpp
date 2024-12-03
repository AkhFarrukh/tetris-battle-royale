#ifndef GAME_DATA_HPP
#define GAME_DATA_HPP


#include <iostream>
#include <string>


class GameData{
    
    private:
    int score;
    int numberLinesBuilt;
    std::string gameMode;    //pour la partie 1: Mode "endless" sans toutes ses fonctionnalités
    bool isGameOver;

    public:
    GameData();
    
    int get_score() const;
    int get_numberLinesBuilt() const;
    bool get_isGameOver() const;
    std::string get_gameMode() const;

    void set_score(int new_score);
    void set_numberLinesBuilt(int new_numberLinesBuilt);
    void set_isGameOver(bool gameStatus);
    void set_gameMode(const std::string& new_gameMode);   // pas utile pour partie 1 

    void updateScore(int points);
    void showData();  

};

#endif // GAME_DATA_HPP


