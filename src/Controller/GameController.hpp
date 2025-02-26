//
// Created by farrukh on 26/02/25.
//

#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include "input.hpp"
#include "ControllerEnum.hpp"

class GameController {
private:
    Input input;
public:
  void startGameInput() {input.start();}
  void stopGameInput() {input.stop();}
  Action getAction();
};



#endif //GAMECONTROLLER_HPP
