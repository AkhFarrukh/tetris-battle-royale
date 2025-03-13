#ifndef PLAYERDATA_HPP
#define PLAYERDATA_HPP

#include "Mouvement.hpp"
#include "game_data.hpp"
//#include "figureGenerator.hpp"
//#include "GameController.hpp"
//#include "EventQueue.hpp"
//#include "MovementActionEvent.hpp"
//#include "TimerDropEvent.hpp"

class Grid;
class MovingFigure;
class Figure;
class EvenQueue;
enum class Color;

struct PlayerState {

    int                              playerId;

    std::shared_ptr<Grid>            grid;
    std::shared_ptr<MovingFigure>    activeFigure;
    Mouvement                        movement;
    GameData                         gameData;
    std::shared_ptr<Figure>          nextTetromino;
    std::shared_ptr<EventQueue> eventQueue;

    bool                             isGameOver = false;
    int                              fd;
    bool                             isInGame = true;
    MalusBonus                       malusBonus;

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

//struct ThreadGameData {
//    int client_fd;
//    shared_ptr<EventQueue> eventQueue;
//};

#endif 