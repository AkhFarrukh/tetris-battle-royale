#ifndef GRID_HPP
#define GRID_HPP

#include <iostream>
#include  "Block.hpp"
// #include  "coordinates.cpp"
#include  <vector>
using namespace std;

/**
 * @brief Represents the game grid, which consists of a matrix of blocks.
 *
 * The Grid class is responsible for maintaining the state of the game grid,
 * including the positions and colors of blocks. It provides methods to check
 * for free locations, manage and clear full lines, apply effects, and set or
 * retrieve block information.
 */
class Grid {
  vector<vector<Block>> matrix;
  public:
    Grid();
    bool isLocationFree(Coordinates blocLocation);
    bool isFullLine(int row);
    void clearLine(int row);
    void applyDominoEffect(int clearedRow);
    void clearFullLines();
    void setBlock(Block newblock);
    Color getBlockColor(int x, int y);
    void malusLines(int y, int x);
};

#endif