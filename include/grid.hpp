#ifndef GRID_HPP
#define GRID_HPP

#include <iostream>
#include  "Block.hpp"
// #include  "coordinates.cpp"
#include  <vector>
using namespace std;


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
    void lineHeight(int y, int x);
    void malusLines(int y, int x);
};

#endif