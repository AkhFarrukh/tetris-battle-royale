//
// Created by farrukh on 22/11/24.
//

#include <cmath>
#include <cstdlib>
#include <numbers>
#include <ranges>
#include <string>
#include <vector>



class CurrentFigure {
    //todo rename to MovingFigure maybe?
    //todo this class doesnt account for setters and getters for other classes, but it will be chaged
private:
    Figure  tetromino_;
    Point   position_; //position of upper left corner of figure in the grid
    std::vector<Point> blocks_; //vector of actual coordinates of each block

    Point getSpawnPosition(TetrominoShape shape);
    void updateBlocks();

public:
    //Constructor
    CurrentFigure(Figure tetromino);

    Point getPosition();
    void setPosition(Point newPosition);
    Figure getFigure();
    void setFigure(Figure newFigure);
};

CurrentFigure::CurrentFigure(Figure tetromino)
    : tetromino_{tetromino}, {
    position_ = getSpawnPosition(tetromino_.shape);
    updateBlocks();
}

Point CurrentFigure::getSpawnPosition(FigureShape shape) {
    /* Method takes the shape of the figure from enum class and returns
     * a Point of the default position when spawning the figure on grid
     * for given shape. Spawning position is the position of top left corner
     * of the figure.
     *
     * Game mechanic reference: https://tetris.com/play-tetris
     * different from Electronika-60, Apple-IIGS and Nintendo GameBoy versions.
     * */
    switch (shape) {
        case FigureShape::I: return {Point(3, 1)};
        case FigureShape::O: return {Point(4, 0)};
        default:             return {Point(3, 0)};
    }
}

void CurrentFigure::updateBlocks() {
    /* This method takes offsets of each block of figure and calculates their
     * actual position
     * */
    bool firstInitialization = blocks_.empty();
    for(int index = 0; i < 4; i++){
        if(firstInitialization){
            blockRealPosition = position_ //todo + offset tetromino_.blocks[index]
            blocks_.push_back(tetromino_.blocks[index])
        } else {
            blocks_[index] = position_ //todo + offset tetromino_.blocks[index]
        }
    }
}

Point CurrentFigure::getPosition() {
    return position_;
}

void CurrentFigure::setPosition(int newPosition) {
    /* This method should be used to change position of figure
     *
     * */
    position_ = newPosition;
    updateBlocks();
}

Figure CurrentFigure::getPosition() {
    return tetromino_;
}

void CurrentFigure::setFigure(Figure newFigure) {
    /* This method should be used to change shape of figure relative to corner position
     * in case of rotation for example.
     * */
    tetromino_ = newFigure;
    updateBlocks();
}