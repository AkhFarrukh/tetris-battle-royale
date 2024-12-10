#ifndef GRID_HPP
#define GRID_HPP

#pragma once
#include <iostream>
#include  "Block.hpp"
// #include  "coordinates.cpp"
#include  <vector>
using namespace std;


class Grid {
  vector<vector<Block>> matrix;
  public:
    Grid() {
        for (int y = 0; y < 20; ++y) {
            vector<Block> row;
            for (int x = 0; x < 10; ++x) {
                // Initialiser chaque Block avec une couleur par défaut, une taille, et ses coordonnées
                row.push_back(Block(Color::nullcolor, 1, Coordinates(x, y)));
                row.back().setEmpty(true); // Marquer le bloc comme vide
            }
            matrix.push_back(row);
        }
    }
    bool isLocationFree(Coordinates blocLocation);
    bool isFullLine(int row);
    void clearLine(int row);
    void applyDominoEffect(int clearedRow);
    void clearFullLines();
    void setBlock(Block newblock);
    Color getBlockColor(int x, int y);
};

#endif