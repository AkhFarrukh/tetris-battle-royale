//
// Created by farrukh on 22/11/24.
//

// #include <cmath>
// #include <cstdlib>
// #include <numbers>
// #include <ranges>
// #include <string>
// #include <vector>
// #include <memory>
// #include <utility>

#include "movingfigure.hpp"

// #include "tetrominoshape.cpp"
// #include "coordinates.cpp"
// #include "block.cpp"
// #include "color.cpp"
// #include "figure.hpp"
// #include "grid.cpp"



// MovingFigure::MovingFigure(std::shared_ptr<Figure> tetromino, std::shared_ptr<Grid> grid)
//         : tetromino_{std::move(tetromino)}, grid_{std::move(grid)} {
//     position_ = getSpawnPosition();
//     updateBlocks();
// }

MovingFigure::MovingFigure(std::shared_ptr<Figure> tetromino, std::shared_ptr<Grid> grid)
    : Figure(tetromino->getFigShape()),
      tetromino_{std::move(tetromino)},
      position_{getSpawnPosition()},
      grid_{std::move(grid)}{
    updateBlocks();
}

/**
 * @brief Method takes the shape of the figure from enum class and returns
 * a Point of the default position when spawning the figure on grid
 * for given shape. Spawning position is the position of top left corner
 * of the figure.
 *
 * Game mechanic reference: https://tetris.com/play-tetris
 * different from Electronika-60, Apple-IIGS and Nintendo GameBoy versions.
 *
 * @return Default spawn position of the figure given its shape
 */
Coordinates MovingFigure::getSpawnPosition() {
    return {Coordinates(4, 1)};
    // Implementation in class figure resulted in each figure conveniently
    // having a center in the same spawn coordinates
}


/*****************************************************************************
 * Methods to work with blocks
 *****************************************************************************/


/**
 * @brief Method adds real coordinates of a block to a vector.
 *
 * The method takes an index of a block in the figure, a position of the figure,
 * a shared pointer to the figure, and a vector of block positions. It
 * calculates the real position of the block by adding the block's offset to the
 * position of the figure, and then adds this real position to the vector of
 * block positions.
 *
 * @param index Index of the block in the figure.
 * @param position Position of the figure.
 * @param tetromino Shared pointer to the figure.
 * @param blockPositions Vector of block positions.
 */
void MovingFigure::initializeBlockPosition(
        int index,
        const Coordinates& position,
        const shared_ptr<Figure>& tetromino,
        std::vector<Coordinates>& blockPositions
        ){
  Coordinates realPosition = position + tetromino->getBlockPos(index);
  blockPositions.push_back(realPosition);
}


/**
 * @brief Updates the real coordinates of a block in a vector.
 *
 * This method calculates and updates the actual position of a block
 * in the tetromino based on its index. It adds the block's offset
 * position to the given figure's position and updates the block's
 * position in the blockPositions vector at the specified index.
 *
 * @param index Index of the block in the figure.
 * @param position Current position of the figure.
 * @param tetromino Shared pointer to the figure containing the block.
 * @param blockPositions Vector of block positions to be updated.
 */
void MovingFigure::updateBlockPosition(
        int index,
        const Coordinates& position,
        const shared_ptr<Figure>& tetromino,
        std::vector<Coordinates>& blockPositions
        ){
  blockPositions[index] = position + tetromino->getBlockPos(index);
}


/**
 * @brief Method updates the real coordinates of all blocks in a vector.
 *
 * The method takes the position of the figure, a shared pointer to the figure,
 * and a vector of block positions. It loops through all blocks in the figure,
 * calculates their actual position by adding the block's offset to the given
 * figure's position, and updates the block's position in the blockPositions
 * vector. If the blockPositions vector is empty, it initializes it with the
 * real positions of the blocks. Otherwise, it updates the positions of the
 * blocks in the vector.
 *
 * @param position Position of the figure.
 * @param tetromino Shared pointer to the figure containing the blocks.
 * @param blockPositions Vector of block positions to be updated.
 */
void MovingFigure::updateBlocks(
        const Coordinates& position,
        const shared_ptr<Figure>& tetromino,
        std::vector<Coordinates>& blockPositions
        ) {
  bool firstInitialization = blockPositions.empty(); //checks if vector is empty
  for(int index = 0; index < 4; index++){
    if(firstInitialization){
      //if vector is empty
      initializeBlockPosition(index, position, tetromino, blockPositions);
    } else {
      updateBlockPosition(index, position, tetromino, blockPositions);
    }
  }
}


/**
 * @brief Calls updateBlocks() with the MovingFigure's position and tetromino.
 *
 * This is a convenience function that calls updateBlocks() with the MovingFigure's
 * own position and tetromino. It is used to update the MovingFigure's block
 * positions when the figure is moved or rotated.
 */
void MovingFigure::updateBlocks() {
  updateBlocks(position_, tetromino_, blockPositions_);
}

/*****************************************************************************
 * Checkers and Verifiers
 *****************************************************************************/

/**
 * @brief Checks if a block is allowed to be placed at the given position.
 *
 * This method takes a block position and returns true if the block can be placed
 * at that position, and false otherwise. It checks if the position is free on
 * the grid by calling Grid::isLocationFree() on the grid contained in the
 * MovingFigure.
 *
 * @param blockPosition The position to check.
 * @return True if the block can be placed at the given position, false otherwise.
 */
bool MovingFigure::allowBlock(Coordinates blockPosition){
  return grid_->isLocationFree(blockPosition);
}


/**
 * @brief Checks if a given figure can be placed at a given position.
 *
 * This method takes a position and a shared pointer to a figure and
 * returns true if the figure can be placed at that position, and false
 * otherwise. It checks by updating the block positions of the figure
 * with the given position and then checks each block position to see
 * if it is free on the grid by calling allowBlock() on each block.
 *
 * @param newPosition The position to check.
 * @param newFigure The figure to check.
 * @return True if the figure can be placed at the given position, false otherwise.
 */
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

/**
 * @brief Checks if a newly spawned figure has successfully appeared on the grid.
 *
 * This method determines whether the newly spawned figure can be placed on the grid
 * without overlapping any existing blocks. It should be used by the game to check
 * for a game over condition. The method returns true if the figure spawned on a free
 * space and false if the figure spawned on an occupied space.
 *
 * @return True if the figure spawned on a free space, false if on an occupied space.
 */
bool MovingFigure::spawnSucceeded(){
  return allowAction(position_,tetromino_);
}

/*****************************************************************************
 * Getters and setters
 *****************************************************************************/

Coordinates MovingFigure::getPosition() {
  return position_;
}


/**
 * @brief Changes the position of the figure.
 *
 * This method takes a new position as an argument and checks if the figure can be
 * placed at the given position. If the move is valid, it updates the position of
 * the figure and returns true. If the move is invalid, it returns false.
 *
 * @param newPosition The new position of the figure.
 * @return True if the position was changed, false otherwise.
 */
bool MovingFigure::setPosition(const Coordinates newPosition) {
  if(allowAction(newPosition, tetromino_)){
    position_ = newPosition;
    updateBlocks();
    return true;
  }
  return false;
}


shared_ptr<Figure> MovingFigure::getFigure() {
    return tetromino_;
}


/**
 * @brief Changes the shape of the figure controlled by the MovingFigure after rotation.
 *
 * This method takes a new shared pointer to a figure as an argument and checks
 * if the new figure can be placed at the same position as the current figure.
 * If the move is valid, it updates the shape of the figure and returns true.
 * If the move is invalid, it checks for valid alternative positions and if
 * one is found, it updates the position and shape of the figure and returns true.
 * If no valid position is found, it returns false.
 *
 * @param newFigure The new rotated figure.
 * @return True if the shape was changed, false otherwise.
 */
bool MovingFigure::setFigure(const std::shared_ptr<Figure>& newFigure) {
  if(allowAction(position_, newFigure)){
    tetromino_ = newFigure;
    updateBlocks();
    return true;
  }
  std::vector<Coordinates> offsets;
    if (tetromino_->getFigShape() == TetrominoShape::I) {
        offsets = {{-2, 0}, {2, 0}, {0, -1}, {0, 1}};
    } else {
        offsets = {{-1, 0}, {1, 0}, {0, -1}};
    }
  for (const auto& offset : offsets) {
      Coordinates newPosition = position_ + offset;
      if (allowAction(newPosition, newFigure)) {
          setPosAndFig(newPosition, newFigure);
          return true;
      }
  }
  return false;
}

/**
 * @brief Changes the position and shape of the figure controlled by the MovingFigure.
 *
 * This method combines the setPosition() and setFigure() methods in one action.
 *
 * This method should be used if you need to do a translation with a rotation at the same time
 * in case of wall kicks. For example, when rotating a figure, you may need to
 * move it to a different position to avoid collision with the wall.
 * https://tetris.wiki/Super_Rotation_System
 *
 * @param newPosition The new position of the figure.
 * @param newFigure The new rotated figure.
 * @return True if the position and shape were changed, false otherwise.
 */

bool MovingFigure::setPosAndFig(
        const Coordinates newPosition,
        const shared_ptr<Figure>& newFigure
        ){

  if(allowAction(newPosition, newFigure)){
    position_ = newPosition;
    tetromino_ = newFigure;
    updateBlocks();
    return true;
  }
  return false;
}


/**
 * @brief Returns the vector of blocks of the figure with their actual positions.
 *
 * This method is used to get the blocks of the figure with their actual positions
 * on the grid. It creates a temporary figure with the same shape as the current
 * figure, but with the actual positions of the blocks.
 *
 * @return A vector of blocks of the figure with their actual positions.
 */
std::vector<Block> MovingFigure::getBlocksRealPositions() {
  std::shared_ptr<Figure> realTetro = tetromino_;
  for (int index = 0; index<4; ++index)
  {
    realTetro->setBlockPos(index, blockPositions_[index]);
  }
  vector<Block> realBlocks = realTetro->getFigBlocks();
  return realBlocks;
}


/**
 * @brief Returns the color of the moving figure at a given position.
 *
 * This method takes a position as an argument and checks if the position is one of
 * the actual positions of the blocks of the moving figure. If the position is from
 * the moving figure, it returns the color of the moving figure. If the position is
 * not from the moving figure, it returns a null color.
 *
 * @param coordinates The position to check.
 * @return The color of the moving figure if the position is from the figure, a null
 * color otherwise.
 */
Color MovingFigure::getMovingFigureBlockColor(Coordinates coordinates) {
  /*Returns a color of current figure if coordinate is one of movingfigure's
   *Return a nullcolor if coordinates are not from the figure
   */
  for(Coordinates pos: blockPositions_){
    if(pos == coordinates){
      return tetromino_->getColor();
    }
  }
  return Color::nullcolor;
}



unsigned int MovingFigure::getCurrentRotationState(){
  //todo remove because rotation state is now in figure

  /* Returns the number of current rotation state that is 0 by default
   * consult the chart to understand which state corresponds to what position
   * of blocks
   * https://tetris.wiki/Super_Rotation_System
   */
  return rotationState;
}

void MovingFigure::rotateRight() {
  //todo remove because rotation state is now in figure

  /* updates rotation state value when making a right rotation
   *
   */
  rotationState = (rotationState + 1) % 4;
}


void MovingFigure::rotateLeft(){
  //todo remove because rotation state is now in figure

  /* updates rotation state value when making a left rotation
 *
 */
  rotationState = (rotationState - 1) % 4;
}