#include "Mouvement.hpp"

#define WALL_KICK_TEST_START_INDEX 0
#define WALL_KICK_TEST_END_INDEX 4

// Map for wall kick data
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


Mouvement::Mouvement(Action action, std::shared_ptr<MovingFigure> movFigure): action_(action),
                                                                              movingFigure_{(movFigure)} {executeAction();}

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

bool Mouvement::moveRight(){
  Coordinates newPos = movingFigure_->getPosition() + Coordinates {1,0};
  return movingFigure_->setPosition(newPos);
}

bool Mouvement::moveLeft(){
    Coordinates newPos = movingFigure_->getPosition() + Coordinates {-1,0};
    return movingFigure_->setPosition(newPos);
}

bool Mouvement::moveDown(){
    Coordinates newPos = movingFigure_->getPosition() + Coordinates {0,1};
    return movingFigure_->setPosition(newPos);
}




bool Mouvement::rotateRight(){
    TetrominoShape figureShape = movingFigure_->getFigShape();
    // std::shared_ptr<Figure> figure_ = std::make_shared(movingFigure_);
    std::shared_ptr<Figure> figure_ = movingFigure_->getFigure();

    // if (figureShape == TetrominoShape::I || figureShape == TetrominoShape::O){
    //     rotateFigureSpecial(figure_);
    // }
    // else{
        // rotateFigure(figure_);
    // }
    if (figureShape == TetrominoShape::O){ 
    }else if(figureShape == TetrominoShape::I){
        rotateFigureSpecial(figure_);
    } else{
        rotateFigure(figure_);
    }
    
    Coordinates vect = (movingFigure_->getCenter()-(figure_->getCenter()));
    for (int i = 0; i < 4; i++){
        figure_->setBlockPos(i, figure_->getBlockPos(i)+vect);
    }
    figure_->setCenter(figure_->getCenter()+vect);
    //todo while loop here
    //todo use setfigpos
    //todo dont forget to change the rotation state
    // two functions are repeating
    RotationState rotationState = figure_->getRotationState();
    for (int i = WALL_KICK_TEST_START_INDEX; i <= WALL_KICK_TEST_END_INDEX; i++) {
        //todo this shit can be optimized by skipping test 0 (0,0) and setting figure before for loop
        Coordinates offset = performWallKick(figure_, rotationState, true, i);
        Coordinates newPos = movingFigure_->getPosition() + offset;
        if (movingFigure_->setPosAndFig(newPos, figure_)) {
            movingFigure_->setFigure(figure_);
            return true;
        }
    }
    return false;
}

bool Mouvement::rotateLeft(){
    TetrominoShape figureShape = movingFigure_->getFigShape();
    // std::shared_ptr<Figure> figure_ = std::make_shared(movingFigure_);
    std::shared_ptr<Figure> figure_ = movingFigure_->getFigure();

    if (figureShape == TetrominoShape::O){
    }else if(figureShape == TetrominoShape::I){
        rotateFigureSpecial(figure_, false);
    } else{
        rotateFigure(figure_, false);
    }

    Coordinates vect = (movingFigure_->getCenter()-(figure_->getCenter()));
    for (int i = 0; i < 4; i++){
      figure_->setBlockPos(i, figure_->getBlockPos(i)+vect);
    }
    figure_->setCenter(figure_->getCenter()+vect);
    RotationState rotationState = figure_->getRotationState();
    for (int i = WALL_KICK_TEST_START_INDEX; i <= WALL_KICK_TEST_END_INDEX; i++) {
        //todo this shit can be optimized by skipping test 0 (0,0) and setting figure before for loop
        Coordinates offset = performWallKick(figure_, rotationState, false, i);
        Coordinates newPos = movingFigure_->getPosition() + offset;
        if (movingFigure_->setPosAndFig(newPos, figure_)) {
            movingFigure_->setFigure(figure_);
            return true;
        }
    }
    return false;
}


Coordinates Mouvement::getWallKickOffset(shared_ptr<Figure> figure_, int testIndex, RotationState current, RotationState target) {
    pair<RotationState, RotationState> key = make_pair(current, target);
    Coordinates offset = wallKickData.at(key)[testIndex];
    figure_->setRotationState(target);
    //todo maybe this was better but its less safe : Coordinates offset = wallKickData[key][testIndex];
    return offset;
}

Coordinates Mouvement::performWallKick(shared_ptr<Figure> figure_, RotationState initialRotationState, bool clockwise, int testIndex) {
    //todo figure_ is useless here
    Coordinates offset = Coordinates(0, 0);
    if (clockwise) {
        switch (initialRotationState) {
            case SPAWN:
                offset = getWallKickOffset(figure_, testIndex, initialRotationState, RIGHT);
                break;
            case RIGHT:
                offset = getWallKickOffset(figure_, testIndex, initialRotationState, TWO);
                break;
            case LEFT:
                offset = getWallKickOffset(figure_, testIndex, initialRotationState, SPAWN);
                break;
            case TWO:
                offset = getWallKickOffset(figure_, testIndex, initialRotationState, LEFT);
                break;
            default: ;
        }
    } else {
        switch (initialRotationState) {
            case SPAWN:
                offset = getWallKickOffset(figure_, testIndex, initialRotationState, LEFT);
                break;
            case LEFT:
                offset = getWallKickOffset(figure_, testIndex, initialRotationState, TWO);
                break;
            case RIGHT:
                offset = getWallKickOffset(figure_, testIndex, initialRotationState, SPAWN);
                break;
            case TWO:
                offset = getWallKickOffset(figure_, testIndex, initialRotationState, RIGHT);
                break;
            default: ;
        }
    }
    return offset;
}

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
