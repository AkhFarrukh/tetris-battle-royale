//
// Created by daris on 11/24.
//
#ifndef FIGURE_HPP
#define FIGURE_HPP

#include "ComponentEnum.hpp"
#include "Block.hpp"
#include "RotationState.hpp"

#include <vector>
#include <cassert>
#include <map>

constexpr int BLOCK_SIZE = 150;

using namespace std;

/**
 * @brief Represents a tetromino figure in the game.
 *
 * The Figure class encapsulates the properties and behavior of a tetromino
 * figure, which consists of four blocks. Each figure has a specific shape,
 * color, and rotation state. The class provides methods to manage the blocks
 * that make up the figure and to interact with the game grid.
 *
 * Responsibilities include:
 * - Storing 4 blocks that form the tetromino, with the first block as the pivot.
 * - Maintaining the current shape and color of the tetromino.
 * - Keeping track of the rotation state of the tetromino.
 * - Providing access to the figure's blocks and properties.
 */
class Figure {
    private:
        vector<Block> figBlocks;        //4 figure's blocks, THE FIRST is the PIVOT, others are OFFSETS
        Coordinates center = Coordinates(0,0);
        TetrominoShape figShape;
        static const map<TetrominoShape, pair<Color, vector<Coordinates>>> figReferences;       //contient les différentes formes de figures
        Color color;
        RotationState rotationState = SPAWN;
    public:
        //Constructor
        Figure(TetrominoShape _figShape);
        //Getters
        vector<Block> getFigBlocks ();      //getter Vector de la figure
        Coordinates getCenter ();  //getter center
        int getCenterX(); //getter coordonnée x de center
        int getCenterY(); //getter coordonnée y de center
        TetrominoShape getFigShape();        //getter de la shape
        Coordinates getBlockPos(int blockIndex);            //getter des coordinates du bloc i
        int getPosBlockX(int blockIndex);
        int getPosBlockY(int blockIndex);
        Color getColor();
        RotationState getRotationState() {return rotationState;}

        //Setters
        void setFigBlocks (vector<Block> newFigBlocks);
        void setCenter (Coordinates newCenter);
        void setBlockPos(int blockIndex, Coordinates blockPos);
        void setRotationState(RotationState newRotationState) {rotationState = newRotationState;}



        //Other Methods

        //Errors handling
        void invariant ();        //assert (figBlocks.size() == 4)


};


#endif //FIGURE_HPP