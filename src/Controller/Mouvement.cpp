#include "Mouvement.hpp"

#define WALL_KICK_TEST_START_INDEX 0
#define WALL_KICK_TEST_END_INDEX 4

/* Map for wall kick data for J, L, S, T, Z
 * key = {initialRotationState, targetRotationState}
 * value = vector of coordinates of offsets for wall kick
 * https://tetris.wiki/Super_Rotation_System
*/
const map<pair<RotationState, RotationState>, vector<Coordinates> > wallKickData = {
    // 0 -> R
    {
        {SPAWN, RIGHT},
        {
            Coordinates(0, 0),
            Coordinates(-1, 0),
            Coordinates(-1, 1),
            Coordinates(0, -2),
            Coordinates(-1, -2)
        }
    },

    // R -> 0
    {
        {RIGHT, SPAWN},
        {
            Coordinates(0, 0),
            Coordinates(1, 0),
            Coordinates(1, -1),
            Coordinates(0, 2),
            Coordinates(1, 2)
        }
    },

    // R -> 2
    {
        {RIGHT, TWO},
        {
            Coordinates(0, 0),
            Coordinates(1, 0),
            Coordinates(1, -1),
            Coordinates(0, 2),
            Coordinates(1, 2)
        }
    },

    // 2 -> R
    {
        {TWO, RIGHT},
        {
            Coordinates(0, 0),
            Coordinates(-1, 0),
            Coordinates(-1, 1),
            Coordinates(0, -2),
            Coordinates(-1, -2)
        }
    },

    // 2 -> L
    {
        {TWO, LEFT},
        {
            Coordinates(0, 0),
            Coordinates(1, 0),
            Coordinates(1, 1),
            Coordinates(0, -2),
            Coordinates(1, -2)
        }
    },

    // L -> 2
    {
        {LEFT, TWO},
        {
            Coordinates(0, 0),
            Coordinates(-1, 0),
            Coordinates(-1, -1),
            Coordinates(0, 2),
            Coordinates(-1, 2)
        }
    },

    // L -> 0
    {
        {LEFT, SPAWN},
        {
            Coordinates(0, 0),
            Coordinates(-1, 0),
            Coordinates(-1, -1),
            Coordinates(0, 2),
            Coordinates(-1, 2)
        }
    },

    // 0 -> L
    {
        {SPAWN, LEFT},
        {
            Coordinates(0, 0),
            Coordinates(1, 0),
            Coordinates(1, 1),
            Coordinates(0, -2),
            Coordinates(1, -2)
        }
    }
};


// I Tetromino Wall Kick Data
const map<pair<RotationState, RotationState>, vector<Coordinates> > wallKickDataI = {
    // 0 -> R
    {
        {SPAWN, RIGHT},
        {
            Coordinates(0, 0),
            Coordinates(-2, 0),
            Coordinates(1, 0),
            Coordinates(-2, -1),
            Coordinates(1, 2)
        }
    },

    // R -> 0
    {
        {RIGHT, SPAWN},
        {
            Coordinates(0, 0),
            Coordinates(2, 0),
            Coordinates(-1, 0),
            Coordinates(2, 1),
            Coordinates(-1, -2)
        }
    },

    // R -> 2
    {
        {RIGHT, TWO},
        {
            Coordinates(0, 0),
            Coordinates(-1, 0),
            Coordinates(2, 0),
            Coordinates(-1, 2),
            Coordinates(2, -1)
        }
    },

    // 2 -> R
    {
        {TWO, RIGHT},
        {
            Coordinates(0, 0),
            Coordinates(1, 0),
            Coordinates(-2, 0),
            Coordinates(1, -2),
            Coordinates(-2, 1)
        }
    },

    // 2 -> L
    {
        {TWO, LEFT},
        {
            Coordinates(0, 0),
            Coordinates(2, 0),
            Coordinates(-1, 0),
            Coordinates(2, 1),
            Coordinates(-1, -2)
        }
    },

    // L -> 2
    {
        {LEFT, TWO},
        {
            Coordinates(0, 0),
            Coordinates(-2, 0),
            Coordinates(1, 0),
            Coordinates(-2, -1),
            Coordinates(1, 2)
        }
    },

    // L -> 0
    {
        {LEFT, SPAWN},
        {
            Coordinates(0, 0),
            Coordinates(1, 0),
            Coordinates(-2, 0),
            Coordinates(1, -2),
            Coordinates(-2, 1)
        }
    },

    // 0 -> L
    {
        {SPAWN, LEFT},
        {
            Coordinates(0, 0),
            Coordinates(-1, 0),
            Coordinates(2, 0),
            Coordinates(-1, 2),
            Coordinates(2, -1)
        }
    }

};

Mouvement::Mouvement(Action action, std::shared_ptr<MovingFigure> movFigure): action_(action),
                                                                              movingFigure_{(movFigure)} {executeAction();}

/**
 * @brief Executes the action stored in the action_ field and returns a boolean
 * to indicate whether the action was successful or not.
 *
 * The action is executed by calling a member function of MovingFigure that
 * corresponds to the action. The result of the function call is stored in a
 * boolean variable that is returned at the end of the function.
 *
 * If the action is not recognized, the function does nothing and returns true.
 *
 * @return A boolean indicating whether the action was successful or not.
 */
bool Mouvement::executeAction(){
    bool s = true;
    switch (action_) {
        case MOVE_RIGHT:
            s = moveRight();
            break;
        case MOVE_LEFT:
            s = moveLeft();
            break;
        case SOFT_DROP:
            s =  moveDown();
            break;
        case ROTATE_RIGHT:
            s =  rotateRight();
            break;
        case ROTATE_LEFT:
            s = rotateLeft();
            break;
        default:
            break;
        }
    return s;
}


bool Mouvement::setMouvement(Action action){
  action_ = action;
  return executeAction();
}

/*
 *@brief Moves the figure by a given offset.
 *
 * This function moves the figure by the specified offset in the coordinates.
 * It calculates the new position by adding the offset to the current position
 * of the moving figure and then sets the new position.
 *
 * @param offset The offset by which to move the figure.
 * @return A boolean indicating whether the move was successful or not.
 */
bool Mouvement::moveFigure(const Coordinates& offset) {
    Coordinates newPos = movingFigure_->getPosition() + offset;
  return movingFigure_->setPosition(newPos);
}


bool Mouvement::moveRight() {
    return moveFigure(Coordinates{1, 0});
}

bool Mouvement::moveLeft(){
    return moveFigure(Coordinates{-1, 0});
}

bool Mouvement::moveDown(){
    return moveFigure(Coordinates{0, 1});
}


/**
 * @brief Rotates the figure by 90 degrees clockwise or counter-clockwise.
 *
 * This method rotates the figure by 90 degrees in the specified direction.
 * It first checks if the figure is an O tetromino, in which case it does
 * not rotate and returns false.
 * Then it rotates the figure in the specified direction (clockwise or
 * counter-clockwise) and moves it to the new position by applying the
 * wall kick offset if the rotation wasn't successful.
 *
 * @param clockwise Whether to rotate the figure clockwise or counter-clockwise.
 * @return A boolean indicating whether the rotation was successful or not.
 */
bool Mouvement::rotate(bool clockwise) {
    TetrominoShape figureShape = movingFigure_->getFigShape();
    std::shared_ptr<Figure> figure_ = movingFigure_->getFigure();

    if (figureShape == TetrominoShape::O) {
        // O tetromino does not rotate
        return false;
    }
    if (figureShape == TetrominoShape::I) {
        rotateFigureSpecial(figure_, clockwise);
    } else {
        rotateFigure(figure_, clockwise);
    }

    //adjust figure blocks after rotation
    Coordinates vect = (movingFigure_->getCenter() - (figure_->getCenter()));
    for (int i = 0; i < 4; i++) {
        figure_->setBlockPos(i, figure_->getBlockPos(i) + vect);
    }
    //adjust figure center after rotation
    figure_->setCenter(figure_->getCenter() + vect);

    RotationState rotationState = figure_->getRotationState();
    //tries to set the figure as a movingfigure, if it fails, it tries to wall kick and out it on a different offset.
    for (int i = WALL_KICK_TEST_START_INDEX; i <= WALL_KICK_TEST_END_INDEX; i++) {
        //todo this shit can be optimized by skipping test 0 (0,0) and setting figure before for loop
        Coordinates offset = performWallKick(figure_, rotationState, clockwise, i);
        Coordinates newPos = movingFigure_->getPosition() + offset;
        if (movingFigure_->setPosAndFig(newPos, figure_)) {
            return true;
        }
    }
    return false;
}

bool Mouvement::rotateLeft() {
    return rotate(false);
}

bool Mouvement::rotateRight() {
    return rotate(true);
}


/**
 * @brief Get a wall kick offset for a given figure state transition.
 *
 * Given a figure in a certain initial rotation state, this method returns a wall kick offset from the table
 * needed to transition to the target state
 *
 * It also changes the state saved within the figure
 *
 * @param figure_ The figure to transition.
 * @param testIndex Index of the offset to retrieve in the wall kick data.
 * @param current The current rotation state of the figure.
 * @param target The target rotation state of the figure.
 * @return The wall kick offset to apply to the figure's blocks.
 */
Coordinates Mouvement::getWallKickOffset(shared_ptr<Figure> figure_, int testIndex, RotationState current, RotationState target) {
    pair<RotationState, RotationState> key = make_pair(current, target);
    figure_->setRotationState(target);
    if (figure_->getFigShape() == TetrominoShape::I) {
        return wallKickDataI.at(key)[testIndex];
    } else {
        return wallKickData.at(key)[testIndex];
    }
    //todo maybe this was better but its less safe : Coordinates offset = wallKickData[key][testIndex];
}

/**
 * @brief Performs a wall kick transition.
 *
 * Given a figure in a certain initial rotation state, this method finds a target state and performs a transition.
 *
 * It also changes the state saved within the figure
 *
 * @param figure_ The figure to transition.
 * @param initialRotationState The initial rotation state of the figure.
 * @param clockwise Whether the rotation is clockwise or counter-clockwise.
 * @param testIndex Index of the offset to retrieve in the wall kick data.
 * @return The wall kick offset to apply to the figure's blocks.
 */
Coordinates Mouvement::performWallKick(
    shared_ptr<Figure> figure_, RotationState initialRotationState, bool clockwise, int testIndex) {
    //vector of next rotation state where index corresponds to int value on current state in its enum
    static const vector<RotationState> clockwiseOffsets = {RIGHT, TWO, SPAWN, LEFT};
    static const vector<RotationState> counterClockwiseOffsets = {LEFT, SPAWN, TWO, RIGHT};

    RotationState targetRotationState;
    if (clockwise) {
        targetRotationState = clockwiseOffsets[initialRotationState];
    } else {
        targetRotationState = counterClockwiseOffsets[initialRotationState];
    }

    return getWallKickOffset(figure_, testIndex, initialRotationState, targetRotationState);
}



/**
 * @brief Rotates a figure by 90 degrees.
 *
 * This function performs a 90-degree rotation on the given figure.
 * Depending on the direction specified by the clockwiseRotation parameter,
 * the rotation is executed either in a clockwise or counter-clockwise direction.
 * The function updates the positions of all blocks within the figure and
 * adjusts the center of the figure accordingly.
 *
 * @param figure_ A shared pointer to the Figure object to be rotated.
 * @param clockwiseRotation A boolean indicating the direction of rotation:
 *                          true for clockwise, false for counter-clockwise.
 */
void Mouvement::rotateFigure(shared_ptr<Figure> figure_, bool clockwiseRotation){
    if (clockwiseRotation){
        figure_->setRotationState(figure_->getRotationState());
        for (int i = 0; i < 4; i++){
            figure_->setBlockPos(i, Coordinates(-figure_->getPosBlockY(i), figure_->getPosBlockX(i)));

        }
        figure_->setCenter(Coordinates(-figure_->getCenterY(), figure_->getCenterX()));
}
    else{
        for (int i = 0; i < 4; i++){
            figure_->setBlockPos(i, Coordinates(figure_->getPosBlockY(i), -(figure_->getPosBlockX(i))));
        }
        figure_->setCenter(Coordinates(figure_->getCenterY(), -(figure_->getCenterX())));

    }
}


/**
 * @brief Special rotation function for I Tetrominoes.
 *
 * This method is used only for I Tetrominoes because they have a special rotation
 * behavior. The method takes a shared pointer to a Figure object and a boolean
 * indicating whether the rotation is clockwise or counter-clockwise. It then
 * modifies the positions of the blocks that make up the Tetromino to achieve
 * the desired rotation.
 *
 * @param figure_ The Figure object to be rotated.
 * @param clockwiseRotation A boolean indicating whether the rotation is clockwise
 *                          or counter-clockwise.
 */
void Mouvement::rotateFigureSpecial(shared_ptr<Figure> figure_, bool clockwiseRotation){
  if (clockwiseRotation){
    for (int i = 0; i < 4; i++){
          figure_->setBlockPos(i, Coordinates(-figure_->getPosBlockY(i), figure_->getPosBlockX(i)));

    }

  }
  else{
      for (int i = 0; i < 4; i++){
          figure_->setBlockPos(i, Coordinates(figure_->getPosBlockY(i), -figure_->getPosBlockX(i)));

      }
  }
}
