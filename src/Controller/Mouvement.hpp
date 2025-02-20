#ifndef MOUVEMENT_HPP
#define MOUVEMENT_HPP

#include "input.hpp"
#include "movingfigure.hpp"
#include "ControllerEnum.hpp"
#include "figure.hpp"


class Mouvement{
    Action action_;
    // MovingFigure* movingFigure_;
    std::shared_ptr<MovingFigure> movingFigure_;
  public:
    // Mouvement(Action action, MovingFigure *movFigure):action_(action), movingFigure_{movFigure}{executeAction();}
    Mouvement(Action action, std::shared_ptr<MovingFigure> movFigure);
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