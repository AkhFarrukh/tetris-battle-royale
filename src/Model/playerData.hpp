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
    MalusBonus                       malusBonus;
    
    bool                             isGameOver = false;
    int                              fd;
    int                              blackout = 0;
    bool                             slowed = false;
    bool                             blocked = false;
    bool                             inverted = false;
    bool                             sped_up = false;
    int                              countTetriminoGenerated = 0;
    bool                             isBonusCube = false;
    int                              energy = 0; //Energy pour acheter des malus/Bonus dans RC mode
    int                              completedLines = 0; //mode Classic et Duel

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