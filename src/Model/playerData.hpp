#ifndef PLAYERDATA_HPP
#define PLAYERDATA_HPP

#include "Mouvement.hpp"
#include "game_data.hpp"
#include "figureGenerator.hpp"
#include "GameController.hpp"
#include "EventQueue.hpp"
#include "MovementActionEvent.hpp"
#include "TimerDropEvent.hpp"
#include "serverFunctions.hpp"

struct PlayerState {

    std::shared_ptr<Grid>            grid;
    std::shared_ptr<MovingFigure>    activeFigure;
    Mouvement                        movement;
    GameData                         gameData;
    std::shared_ptr<Figure>          nextTetromino;
    std::shared_ptr<EventQueue> eventQueue;

    int                              playerId;
    bool                             isGameOver = false;
    int                              fd;

    PlayerState(int pid)
        : playerId(pid),
          movement(Action::NONE, nullptr),
          gameData(0, 0, false, "Royal"){}  
};

struct NetPlayerData {
    int playerId;
    bool isGameOver;
    int linesCleared;
    int score;

    vector<Color> grid;
};

#endif 