//
// Created by farrukh on 22/11/24.
//

#include <cmath>
#include <cstdlib>
#include <numbers>
#include <ranges>
#include <string>
#include <vector>
#include <memory>

#include "tetrominoshape.cpp"
#include "coordinates.cpp"
#include "block.cpp"
#include "color.cpp"
#include "figure.cpp"



class MovingFigure {
private:
    Figure  tetromino_;
    Coordinates position_; //position of centerpiece of tetromino

    //vector of actual coordinates of each block
    std::vector<Coordinates> blockPositions_;
    //todo not sure if it should be vector of blocks

    //shared pointer to grid
    std::shared_ptr<Grid> grid_;

    unsigned int rotationState = 0;


    Coordinates getSpawnPosition(TetrominoShape shape, );

    void updateBlocks();
    void updateBlocks(
            const Coordinates& position,
            const shared_ptr<Figure>& tetromino,
            std::vector<Coordinates>& blockPositions
            );

    void checkBlockPositions();

    void initializeBlockPosition(
            int index,
            const Coordinates& position,
            const shared_ptr<Figure>& tetromino,
            std::vector<Coordinates>& blockPositions
            );

    void updateBlockPosition(
            int index,
            const Coordinates& position,
            const shared_ptr<Figure>& tetromino,
            std::vector<Coordinates>& blockPositions
            );

    bool allowBlock(Coordinates blockPosition);
    bool allowAction(
            const Coordinates newPosition,
            const shared_ptr<Figure>& newFigure
            );

    void rotateRight();
    void rotateLeft();


    /* todo need some kind of communication between grid or game to know
     * todo if a move can be made
     */
public:
    //Constructor
    MovingFigure(Figure tetromino);

    Coordinates getPosition();
    bool setPosition(const Coordinates newPosition);
    Figure getFigure();
    bool setFigure(const shared_ptr<Figure>& newFigure);

    bool setPosAndFig(
            const Coordinates newPosition,
            const shared_ptr<Figure>& newFigure
    );
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


/*****************************************************************************
 * Methods to work with blocks
 *****************************************************************************/


void MovingFigure::initializeBlockPosition(
        int index,
        const Coordinates& position,
        const shared_ptr<Figure>& tetromino,
        std::vector<Coordinates>& blockPositions
        ){
  /* Adds real coordinates of a block to a vector
   */
  Coordinates realPosition = position + tetromino->getFigBlocks()[index];
  blockPositions.push_back(realPosition);
}

void MovingFigure::updateBlockPosition(
        int index,
        const Coordinates& position,
        const shared_ptr<Figure>& tetromino,
        std::vector<Coordinates>& blockPositions
        ){
  /* Updates real coordinates of a block in a vector
   */
  blockPositions[index] = position + tetromino.getFigBlocks()[index];
}
void MovingFigure::updateBlocks() {
  updateBlocks(position_, tetromino_, blockPositions_);
}

void MovingFigure::updateBlocks(
        const Coordinates& position,
        const shared_ptr<Figure>& tetromino,
        std::vector<Coordinates>& blockPositions
        ) {
  /* This method takes offsets of each block of figure and calculates their
   * actual position
   */
  bool firstInitialization = blockPositions.empty(); //checks if vector is empty
  for(int index = 0; index < 4; index++){
    if(firstInitialization){
      //if vector is empty
      initializeBlockPosition(index, position, tetromino, blockPositions);
      //todo maybe ignore first block since it'll be (0,0)
    } else {
      updateBlockPosition(index);
    }
  }
}

bool MovingFigure::allowBlock(Coordinates blockPosition){
  return grid_->isBlockFree(blockPosition);
  //todo use real method from grid when it will be available
}


bool MovingFigure::allowAction(
        const Coordinates newPosition,
        const shared_ptr<Figure>& newFigure
        ){
  std::vector<Coordinates> newBlockPositions;
  updateBlocks(newPosition, newFigure, newBlockPositions);
  for(int index = 0; index < 4; index++){
    if(!allowBlock(newBlockPositions[index])){
      return false;
    }
  }
  return true;
}




Point MovingFigure::getPosition() {
  return position_;
}

bool MovingFigure::setPosition(const Coordinates newPosition) {
  /* This method should be used to change position of figure
   *
   */
  if(allowAction(newPosition, tetromino_)){
    position_ = newPosition;
    updateBlocks();
    return true
  }
  return false;
}

Figure MovingFigure::getFigure() {
    return tetromino_;
}

bool MovingFigure::setFigure(const shared_ptr<Figure>& newFigure) {
  /* This method should be used to change shape of figure
   * relative to center position in case of rotation for example.
   */
  if(allowAction(position_, newFigure)){
    tetromino_ = newFigure;
    updateBlocks();
    return true;
  }
  return false;
}

bool MovingFigure::setPosAndFig(
        const Coordinates newPosition,
        const shared_ptr<Figure>& newFigure
        ){
  /* Should be used if you need to do a translation at the same time in case
   * of wall kicks
   * https://tetris.wiki/Super_Rotation_System
   */
  if(allowAction(newPosition, newFigure)){
    position_ = newPosition;
    tetromino_ = newFigure;
    updateBlocks();
    return true;
  }
  return false;
}


unsigned int MovingFigure::getCurrentRotationState(){
  /* Returns the number of current rotation state that is 0 by default
   * consult the chart to understand which state corresponds to what position
   * of blocks
   * https://tetris.wiki/Super_Rotation_System
   */
  return rotationState;
}

void MovingFigure::rotateRight() {
  /* updates rotation state value when making a right rotation
   *
   */
  rotationState = (rotationState + 1) % 4;
}

void MovingFigure::rotateLeft(){
  /* updates rotation state value when making a left rotation
 *
 */
  rotationState = (rotationState - 1) % 4;
}