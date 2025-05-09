//
// Created by farrukh on 26/02/25.
//

#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include "Input.hpp"
#include "ControllerEnum.hpp"
#include "Game.hpp"
#include "RenderEngineNcurses.hpp"
#include "EventQueue.hpp"
#include "MovementActionEvent.hpp"

using namespace std;


class Game; // foreward declaration

//todo remake with observer pattern maybe?

class GameController {
private:
  	bool controlsBlocked = false;
    bool controlsInverted = false;
    Input input;
    shared_ptr<Game> game;
    shared_ptr<EventQueue> eventQueue;
    std::mutex queueMutex;
public:
    GameController();
    ~GameController() noexcept __attribute__((noinline)) = default;
    void startGameInput() {input.start();}
    void stopGameInput() {input.stop();}
    Action getAction(KeyboardAction in);
    void applyMalusBonus(MalusBonus malBon, int target);

     // Getters
    bool areControlsBlocked() const { return controlsBlocked; }
    bool areControlsInverted() const { return controlsInverted; }

    // Setters
    void setControlsBlocked(bool isBlocked) { controlsBlocked = isBlocked; }
    void setControlsInverted(bool isInverted) { controlsInverted = isInverted; }
    void printGameOver();

};


#endif //GAMECONTROLLER_HPP
