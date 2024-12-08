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
#include "figure.hpp"
#include "grid.cpp"



class MovingFigure {
private:
    Figure  tetromino_;
    Coordinates position_; //position of centerpiece of tetromino

    //vector of actual coordinates of each block
    std::vector<Coordinates> blockPositions_;

    //shared pointer to grid
    std::shared_ptr<Grid> grid_;

    unsigned int rotationState = 0; //todo this attribute is no longer needed


    Coordinates getSpawnPosition(TetrominoShape shape);

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

    bool spawnSucceeded();

    //todo these methods are no longer needed
    unsigned int getCurrentRotationState();
    void rotateRight();
    void rotateLeft();
};



MovingFigure::MovingFigure(Figure tetromino)
    : tetromino_{tetromino}, {
    position_ = getSpawnPosition(tetromino_->getFigShape());
    updateBlocks();
}

Coordinates MovingFigure::getSpawnPosition(TetrominoShape shape) {
    /* Method takes the shape of the figure from enum class and returns
     * a Point of the default position when spawning the figure on grid
     * for given shape. Spawning position is the position of top left corner
     * of the figure.
     *
     * Game mechanic reference: https://tetris.com/play-tetris
     * different from Electronika-60, Apple-IIGS and Nintendo GameBoy versions.
     */
    return {Coordinates(4, 1)}
    // Implementation in class figure resulted in each figure conveniently
    // having a center in the same spawn coordinates
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
  Coordinates realPosition = position + tetromino->getBlockPos(index);
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
  blockPositions[index] = position + tetromino->getBlockPos(index);
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
    } else {
      updateBlockPosition(index);
    }
  }
}


/*****************************************************************************
 * Checkers and Verifiers
 *****************************************************************************/

bool MovingFigure::allowBlock(Coordinates blockPosition){
  return grid_->isLocationFree(blockPosition);
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

bool MovingFigure::spawnSucceeded(){
  /* Tells if newly spawned figure appeared on top of en existing one.
   * Should be used by game to check for gameover.
   * Returns true is figure spawned on a free space
   * Returns false if figure spawned on occupied space
   */
  return allowAction(position_,Figure);
}

/*****************************************************************************
 * Getters and setters
 *****************************************************************************/

Coordinates MovingFigure::getPosition() {
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


//todo this method is no longer needed
unsigned int MovingFigure::getCurrentRotationState(){
  /* Returns the number of current rotation state that is 0 by default
   * consult the chart to understand which state corresponds to what position
   * of blocks
   * https://tetris.wiki/Super_Rotation_System
   */
  return rotationState;
}

//todo this method is no longer needed
void MovingFigure::rotateRight() {
  /* updates rotation state value when making a right rotation
   *
   */
  rotationState = (rotationState + 1) % 4;
}

//todo this method is no longer needed
void MovingFigure::rotateLeft(){
  /* updates rotation state value when making a left rotation
 *
 */
  rotationState = (rotationState - 1) % 4;
}