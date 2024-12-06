//
// Created by daris on 11/24.
//
#ifndef FIGURE_HPP
#define FIGURE_HPP

#include "tetrominoshape.cpp"
#include "coordinates.cpp"
#include "block.cpp"

#include <vector>
#include <cassert>
#include <map>

constexpr int BLOCK_SIZE = 150;

using namespace std;


class Figure {
    private:
        vector<Block> figBlocks; //4 figure's blocks, THE FIRST is the PIVOT, others are OFFSETS
        Coordinates center = Coordinates(0,0);
        TetrominoShape figShape;
        static const map<TetrominoShape, pair<Color, vector<Coordinates>>> figReferences;
    public:
        //Constructor
        Figure(TetrominoShape _figShape);
        //Getters
        vector<Block> getFigBlocks () {return figBlocks;};
        Coordinates getCenter () {return center;};
        TetrominoShape getFigShape();
        Coordinates getBlockCoord(int block);

        //Setters
        void setFigBlocks (vector<Block> newFigBlocks) {newFigBlocks = figBlocks;};
        void setCenter (Coordinates newCenter) {center = newCenter;};
        //Other Methods

        //Errors handling
        void invariant ();        //assert (figBlocks.size() == 4)

};


#endif //FIGURE_HPP