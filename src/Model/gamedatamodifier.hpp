//
// Created by farrukh on 8/12/24.
//
#ifndef GAMEDATAMODIFIER_HPP
#define GAMEDATAMODIFIER_HPP


#include <memory>
#include <cmath>

#include "game_data.hpp"

#define LINES_FOR_LEVEL_UP 10
#define BASE_TIME_INTERVAL 800
#define LEVEL_DECREMENT_TIME 7


class GameDataModifier {
private:
    std::shared_ptr<GameData> gameData;
    int timeInterval = BASE_TIME_INTERVAL; //milliseconds
    int level = 1;

    void levelUp();
    void addScoreFromLines(int lineNumber);
    void updateTimeInterval();


public:
    GameDataModifier(const std::shared_ptr<GameData> gameData_);
    void registerLines(int lineNumber);
    int getTimeInterval();
};


#endif