#include "input.cpp"
#include "movingfigure.cpp"
#include "action.cpp"
#include "figure.hpp"


class Mouvement{
    Action action_;
    Figure* movingFigure_;
  public:
    Mouvement(Action action, MovingFigure *movFigure):action_(action), movingFigure_{movFigure}{executeAction();}
    void setMouvement(Action action);
  private:
    void executeAction();
    void moveRight();
    void moveLeft();
    void moveDown();
    void rotateRight();
    void rotateLeft();
    void rotateFigure(shared_ptr<Figure> figure_, bool clockwiseRotation=true);
    void rotateFigureSpecial(shared_ptr<Figure> figure_, bool clockwiseRotation=true);
};

void Mouvement::executeAction(){
    switch (action_) {
        case MOVE_RIGHT:
            moveRight();
            break;
        case MOVE_LEFT:
            moveLeft();
            break;
        case SOFT_DROP:
            moveDown();
            break;
        case ROTATE_RIGHT:
            rotateRight();
            break;
        case ROTATE_LEFT:
            rotateLeft();
            break;
        default:
            break;
        }
}

void Mouvement::setMouvement(Action action){
  action_ = action;
  executeAction();
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
    std::shared_ptr<Figure> figure_ = std::make_shared(movingFigure_->getFigure);
    if (figureShape == TetrominoShape::I || figureShape == TetrominoShape::O){
        rotateFigureSpecial(figure_)
    }
    else{
        rotateFigure(figure_);
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
    std::shared_ptr<Figure> figure_ = std::make_shared(movingFigure_->getFigure);
    if (figureShape == TetrominoShape::I || figureShape == TetrominoShape::O){
        rotateFigureSpecial(figure_, false);
    }
    else{
        rotateFigure(figure_, false);
    }
    Coordinates vect = ((movingFigure_->getFigure())->getCenter()-(figure_->getCenter()));
    for (int i = 0; i < 4; i++){
      figure_->setPosition(i, figure_->getPosition()+vect);
    }
    figure_->setCenter(figure_->getCenter()+vect);
    movingFigure_->setFigure(figure_);
}


void Mouvement::rotateFigure(shared_ptr<Figure> figure_, bool clockwiseRotation){
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
}


void Mouvement::rotateFigureSpecial(shared_ptr<Figure> figure_, bool clockwiseRotation){
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
}
