
#include "input.cpp"
#include "movingfigure.cpp"


class Mouvement{
    Input::Action action_;
    Figure* movingFigure_;
  public:
    Mouvement(Input::Action action, MovingFigure *movFigure):action_(action), movingFigure_{movFigure}{
      executeAction();
      for (int i = 0; i < 7; i++){

      }
    }
  private:
    void executeAction();
    void moveRight();
    void moveLeft();
    void moveDown();
    void rotateRight();
    void rotateLeft();
    Figure rotateFigure(bool clockwiseRotation=true);
    Figure rotateFigureSpecial(bool clockwiseRotation=true);
};

void Mouvement::executeAction(){
    switch (action_) {
        case Input::MOVE_RIGHT:
            moveRight();
            break;
        case Input::MOVE_LEFT:
            moveLeft();
            break;
        case Input::DROP:
            moveDown();
            break;
        case Input::ROTATE_RIGHT:
            rotateRight();
            break;
        case Input::ROTATE_LEFT:
            rotateLeft();
            break;
        default:
            break;
        }
}

void Mouvement::moveRight(){
  Coordinates newPos = movingFigure_->getPosition() + Coordinates {1,0};
  movingFigure_->setPosition(newPos);
}

void Mouvement::moveLeft(){
    Coordinates newPos = movingFigure_->getPosition() + Coordinates {-1,0};
    movingFigure_->setPosition(newPos);
}

void Mouvement::moveDown(){
    Coordinates newPos = movingFigure_->getPosition() + Coordinates {0,1};
    movingFigure_->setPosition(newPos);
}

void Mouvement::rotateRight(){
    TetrominoShape figureShape = movingFigure_->getFigShape();
    std::shared_ptr<Figure> figure_;
    if (figureShape == TetrominoShape::I || figureShape == TetrominoShape::O){
        figure_ = std::make_shared<Figure>(rotateFigureSpecial());
    }
    else{
        figure_ = std::make_shared<Figure>(rotateFigure());
    }
    Coordinates vect = ((movingFigure_->getFigure())->getCenter()-(figure_->getCenter()));
    for (int i = 0; i < 4; i++){
        figure_->setPosition(i, figure_->getPosition()+vect);
    }
    figure_->setCenter(figure_->getCenter()+vect);
    movingFigure_->setFigure(figure_);
}

void Mouvement::rotateLeft(){
    TetrominoShape figureShape = movingFigure_->getFigShape();
    std::shared_ptr<Figure> figure_;
    if (figureShape == TetrominoShape::I || figureShape == TetrominoShape::O){
        figure_ = std::make_shared<Figure>(rotateFigureSpecial(false));
    }
    else{
        figure_ = std::make_shared<Figure>(rotateFigure(false));
    }
    Coordinates vect = ((movingFigure_->getFigure())->getCenter()-(figure_->getCenter()));
    for (int i = 0; i < 4; i++){
      figure_->setPosition(i, figure_->getPosition()+vect);
    }
    figure_->setCenter(figure_->getCenter()+vect);
    movingFigure_->setFigure(figure_);
}

Figure Mouvement::rotateFigure(bool clockwiseRotation){
    Figure figure_ = movingFigure_->getFigure();
    if (clockwiseRotation){
        for (int i = 0; i < 4; i++){
            figure_.setPosBlock(i, (-(figure_->getPosBlockY(i)), figure_->getPosBlockX(i)));
        }
        figure_->setCenter((-(figure_.getCenterY()), figure_->getCenterX()));
    }
    else{
        for (int i = 0; i < 4; i++){
            figure_.setPosBlock(i, (figure_->getPosBlockY(i), -(figure_->getPosBlockX(i))));
        }
        figure_->setCenter((figure_.getCenterY(), -(figure_->getCenterX())));
    }
    return figure_;
}

Figure Mouvement::rotateFigureSpecial(bool clockwiseRotation){
  Figure figure_ = movingFigure_->getFigure();
  if (clockwiseRotation){
    for (int i = 0; i < 4; i++){
       figure_.setPosBlock(i, (-(figure_->getPosBlockY(i)), figure_->getPosBlockX(i)+1));
    }
    figure_->setCenter((-(figure_.getCenterY()), figure_->getCenterX() + 1));
  }
  else{
      for (int i = 0; i < 4; i++){
          figure_.setPosBlock(i, (figure_->getPosBlockY(i) + 1, -(figure_->getPosBlockX(i))));
      }
      figure_->setCenter((figure_.getCenterY() + 1, -(figure_->getCenterX())));
  }
  return figure_;
}
