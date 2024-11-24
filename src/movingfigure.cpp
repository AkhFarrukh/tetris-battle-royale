//
// Created by farrukh on 22/11/24.
//

#include <cmath>
#include <cstdlib>
#include <numbers>
#include <ranges>
#include <string>
#include <vector>

#include "tetrominoshape.cpp"
#include "coordinates.cpp"
#include "block.cpp"
#include "color.cpp"
#include "figure.cpp"



class MovingFigure {
private:
    Figure  tetromino_;
    Coordinates position_; //position of centerpiece of tetromino
    std::vector<Coordinates> blockPositions_;
    //vector of actual coordinates of each block
    //todo not sure if it should be vector of blocks
    Coordinates getSpawnPosition(TetrominoShape shape);
    void updateBlocks();

    /* todo need some kind of communication between grid or game to know
     * todo if a move can be made
     */
public:
    //Constructor
    MovingFigure(Figure tetromino);

    Coordinates getPosition();
    void setPosition(Point newPosition);
    Figure getFigure();
    void setFigure(Figure newFigure);
};

MovingFigure::MovingFigure(Figure tetromino)
    : tetromino_{tetromino}, {
    position_ = getSpawnPosition(tetromino_->getShape);
    //todo i need to know the shape
    updateBlocks();
}

Point MovingFigure::getSpawnPosition(FigureShape shape) {
    /* Method takes the shape of the figure from enum class and returns
     * a Point of the default position when spawning the figure on grid
     * for given shape. Spawning position is the position of top left corner
     * of the figure.
     *
     * Game mechanic reference: https://tetris.com/play-tetris
     * different from Electronika-60, Apple-IIGS and Nintendo GameBoy versions.
     */
    switch (shape) {
        //todo change based on figure class implementation of each figure
        case FigureShape::I: return {Coordinates(3, 1)};
        case FigureShape::O: return {Coordinates(4, 0)};
        default:             return {Coordinates(3, 0)};
    }
}

void MovingFigure::updateBlocks() {
    /* This method takes offsets of each block of figure and calculates their
     * actual position
     */
    bool firstInitialization = blocks_.empty(); //checks if vector is empty
    for(int index = 0; i < 4; i++){
        if(firstInitialization){
            //if vector is empty
            x = position_->getX() + tetromino_->getFigBlocks()[i]->getX();
            y = position_->getY() + tetromino_->getFigBlocks()[i]->getY();
            //todo need a method to get individual blocks
            //todo maybe ignore first block since it'll be (0,0)
            blockRealPosition = Coordinates(x, y);
            blockPositions_.push_back(blockRealPosition);
        } else {
            blocks_[index]->getX() = position_->getX() +
                                     tetromino_->getFigBlocks()[i]->getX();
            blocks_[index]->getY() = position_->getY() +
                                     tetromino_->getFigBlocks()[i]->getY();
        }
    }
}

Point MovingFigure::getPosition() {
    return position_;
}

void MovingFigure::setPosition(int newPosition) {
    /* This method should be used to change position of figure
     *
     */
    position_ = newPosition;
    updateBlocks();
}

Figure MovingFigure::getPosition() {
    return tetromino_;
}

void MovingFigure::setFigure(Figure newFigure) {
    /* This method should be used to change shape of figure
     * relative to center position in case of rotation for example.
     */
    tetromino_ = newFigure;
    updateBlocks();
}