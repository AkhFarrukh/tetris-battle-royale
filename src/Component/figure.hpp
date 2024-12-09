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
        vector<Block> figBlocks;        //4 figure's blocks, THE FIRST is the PIVOT, others are OFFSETS
        Coordinates center = Coordinates(0,0);
        TetrominoShape figShape;
        static const map<TetrominoShape, pair<Color, vector<Coordinates>>> figReferences;       //contient les différentes formes de figures
    public:
        //Constructor
        Figure(TetrominoShape _figShape);
        //Getters
        vector<Block> getFigBlocks () {return figBlocks;};      //getter Vector de la figure
        Coordinates getCenter () {return center;};  //getter center
        int getCenterX() {return center.x;}; //getter coordonnée x de center
        int getCenterY() {return center.y;}; //getter coordonnée y de center
        TetrominoShape getFigShape() {return figShape;};        //getter de la shape
        Coordinates getBlockPos(int blockIndex);            //getter des coordinates du bloc i
        int getPosBlockX(int blockIndex);
        int getPosBlockY(int blockIndex);


        //Setters
        void setFigBlocks (vector<Block> newFigBlocks) {newFigBlocks = figBlocks;};
        void setCenter (Coordinates newCenter) {center = newCenter;};
        void setBlockPos(int blockIndex, Coordinates blockPos);
        //Other Methods

        //Errors handling
        void invariant ();        //assert (figBlocks.size() == 4)

};


#endif //FIGURE_HPP