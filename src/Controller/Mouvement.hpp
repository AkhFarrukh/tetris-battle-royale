#ifndef MOUVEMENT_HPP
#define MOUVEMENT_HPP

#include "input.cpp"
#include "movingfigure.hpp"
#include "action.cpp"
// #include "figure.hpp"

#include "../Component/figure.hpp"


class Mouvement{
    Action action_;
    // MovingFigure* movingFigure_;
    std::shared_ptr<MovingFigure> movingFigure_;
  public:
    // Mouvement(Action action, MovingFigure *movFigure):action_(action), movingFigure_{movFigure}{executeAction();}
    Mouvement(Action action, std::shared_ptr<MovingFigure> movFigure): action_(action), movingFigure_{(movFigure)} {executeAction();}
    bool setMouvement(Action action);
  private:
    bool executeAction();
    bool moveRight();
    bool moveLeft();
    bool moveDown();
    bool rotateRight();
    bool rotateLeft();
    void rotateFigure(shared_ptr<Figure> figure_, bool clockwiseRotation=true);
    void rotateFigureSpecial(shared_ptr<Figure> figure_, bool clockwiseRotation=true);
};

#endif