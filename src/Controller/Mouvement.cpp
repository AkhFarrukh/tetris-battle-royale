#include "Mouvement.hpp"


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

    if (figureShape == TetrominoShape::I || figureShape == TetrominoShape::O){
        rotateFigureSpecial(figure_);
    }
    else{
        rotateFigure(figure_);
    }
    Coordinates vect = (movingFigure_->getCenter()-(figure_->getCenter()));
    for (int i = 0; i < 4; i++){
        figure_->setBlockPos(i, figure_->getBlockPos(i)+vect);
    }
    figure_->setCenter(figure_->getCenter()+vect);
    return movingFigure_->setFigure(figure_);
}

bool Mouvement::rotateLeft(){
    TetrominoShape figureShape = movingFigure_->getFigShape();
    // std::shared_ptr<Figure> figure_ = std::make_shared(movingFigure_);
    std::shared_ptr<Figure> figure_ = movingFigure_->getFigure();

    if (figureShape == TetrominoShape::I || figureShape == TetrominoShape::O){
        rotateFigureSpecial(figure_, false);
    }
    else{
        rotateFigure(figure_, false);
    }
    Coordinates vect = (movingFigure_->getCenter()-(figure_->getCenter()));
    for (int i = 0; i < 4; i++){
      figure_->setBlockPos(i, figure_->getBlockPos(i)+vect);
    }
    figure_->setCenter(figure_->getCenter()+vect);
    return movingFigure_->setFigure(figure_);
}


void Mouvement::rotateFigure(shared_ptr<Figure> figure_, bool clockwiseRotation){
    if (clockwiseRotation){
        for (int i = 0; i < 4; i++){
            // figure_->setPosBlock(i, (-(figure_->getPosBlockY(i)), figure_->getPosBlockX(i)));
            figure_->setBlockPos(i, Coordinates(-figure_->getPosBlockY(i), figure_->getPosBlockX(i)));

        }
        // figure_->setCenter((-(figure_->getCenterY()), figure_->getCenterX()));
        figure_->setCenter(Coordinates(-figure_->getCenterY(), figure_->getCenterX()));
}
    else{
        for (int i = 0; i < 4; i++){
            // figure_->setPosBlock(i, (figure_->getPosBlockY(i), -(figure_->getPosBlockX(i))));
            figure_->setBlockPos(i, Coordinates(figure_->getPosBlockY(i), -(figure_->getPosBlockX(i))));
        }
        // figure_->setCenter((figure_->getCenterY(), -(figure_->getCenterX())));
        figure_->setCenter(Coordinates(figure_->getCenterY(), -(figure_->getCenterX())));

    }
}


void Mouvement::rotateFigureSpecial(shared_ptr<Figure> figure_, bool clockwiseRotation){
  if (clockwiseRotation){
    for (int i = 0; i < 4; i++){
    //    figure_->setPosBlock(i, (-(figure_->getPosBlockY(i)), figure_->getPosBlockX(i)+1));
       figure_->setBlockPos(i, Coordinates(-(figure_->getPosBlockY(i)), figure_->getPosBlockX(i)+1));
    }
    // figure_->setCenter((-(figure_.getCenterY()), figure_->getCenterX() + 1));
    figure_->setCenter(Coordinates(-(figure_->getCenterY()), figure_->getCenterX() + 1));

  }
  else{
      for (int i = 0; i < 4; i++){
        //   figure_->setPosBlock(i, (figure_->getPosBlockY(i) + 1, -(figure_->getPosBlockX(i))));
          figure_->setBlockPos(i, Coordinates(figure_->getPosBlockY(i) + 1, -(figure_->getPosBlockX(i))));
      }
    //   figure_->setCenter((figure_.getCenterY() + 1, -(figure_->getCenterX())));
      figure_->setCenter(Coordinates(figure_->getCenterY() + 1, -(figure_->getCenterX())));
  }
}
