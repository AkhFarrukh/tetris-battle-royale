#ifndef MOUVEMENT_HPP
#define MOUVEMENT_HPP

#include "Input.hpp"
#include "MovingFigure.hpp"
#include "ControllerEnum.hpp"
#include "Figure.hpp"
#include "Coordinates.hpp"
#include "RotationState.hpp"

#include <memory>
#include <vector>
#include <map>
#include <utility>


/**
 * @brief Class that handles the actions movement of a movingfigure.
 *
 * The Mouvement class is responsible for executing various actions on a
 * MovingFigure object, such as moving it left, right, down, or rotating it.
 *
 */
class Mouvement{
    Action action_;
    // MovingFigure* movingFigure_;
    std::shared_ptr<MovingFigure> movingFigure_;
  public:
    // Mouvement(Action action, MovingFigure *movFigure):action_(action), movingFigure_{movFigure}{executeAction();}
    Mouvement(Action action, std::shared_ptr<MovingFigure> movFigure);
    bool setMouvement(Action action);

    bool moveFigure(const Coordinates &offset);

private:
    bool executeAction();
    bool moveRight();
    bool moveLeft();
    bool moveDown();

    bool rotate(bool clockwise);

    bool rotateRight();
    bool rotateLeft();
    Coordinates performWallKick(shared_ptr<Figure> figure_, RotationState initialRotationState, bool clockwise, int testIndex);
    Coordinates getWallKickOffset(shared_ptr<Figure> figure_, int testIndex, RotationState current, RotationState target);
    void rotateFigure(shared_ptr<Figure> figure_, bool clockwiseRotation=true);
    void rotateFigureSpecial(shared_ptr<Figure> figure_, bool clockwiseRotation=true);
};

#endif