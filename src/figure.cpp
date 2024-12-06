//
// Created by daris on 3/12/24.
//
#include "figure.hpp"

#include "color.cpp"
#include "tetrominoshape.cpp"
#include "coordinates.cpp"
#include "block.cpp"

#include <vector>
#include <cassert>

using namespace std;

const map<TetrominoShape, pair<Color, vector<Coordinates>>> Figure::figReferences = {
            {TetrominoShape::I, {Color::cyan, {Coordinates(-1,0), Coordinates(1,0), Coordinates(2,0)}}},
            {TetrominoShape::O, {Color::yellow, {Coordinates(0,-1), Coordinates(1,-1), Coordinates(1,0)}}},
            {TetrominoShape::T, {Color::purple, {Coordinates(-1,0), Coordinates(1,0), Coordinates(0,-1)}}},
            {TetrominoShape::S, {Color::green, {Coordinates(0,-1), Coordinates(1,-1), Coordinates(-1,0)}}},
            {TetrominoShape::Z, {Color::red, {Coordinates(0,-1), Coordinates(-1,-1), Coordinates(1,0)}}},
            {TetrominoShape::L, {Color::orange, {Coordinates(1,0), Coordinates(1,-1), Coordinates(-1,0)}}},
            {TetrominoShape::J, {Color::blue, {Coordinates(1,0), Coordinates(-1,-1), Coordinates(-1,0)}}}
        };


//Constructor
Figure::Figure(TetrominoShape _figShape): figShape{_figShape}{
    
    pair<Color, vector<Coordinates>> figData = figReferences.at(figShape);          //pair<Color, vector<Coordinates>> correspondant à la forme
    Color color = figData.first;
    figBlocks.push_back(Block(color, BLOCK_SIZE, Coordinates(0,0)));                //Block central en (0,0)
    for (auto& blockCoord : figData.second)                                         //Construit chaque block de la figure
    {
        figBlocks.push_back(Block(color, BLOCK_SIZE, blockCoord));
    }
    switch (figShape)
    {
    case TetrominoShape::I :
        center = Coordinates(BLOCK_SIZE/2, BLOCK_SIZE/2);     //For the I, the center isn't (0,0)
        break;
    case TetrominoShape::O :
        center = Coordinates(BLOCK_SIZE/2, -BLOCK_SIZE/2);     //For the I, the center isn't (0,0)
        break;
    default:
        break;
    }
};

Coordinates Figure::getBlockPos(int blockIndex){
    return figBlocks[blockIndex].getPosition();
}

void Figure::setBlockPos(int blockIndex, Coordinates blockPos){
    figBlocks[blockIndex].setPosition(blockPos);
}

void Figure::invariant() {
    assert(figBlocks.size() == 4 && "Error : Not 4 blocks in the figure ");
    for (Block _figBlock : figBlocks)
    {
        assert(_figBlock.getSize() != BLOCK_SIZE && "Error : A block has a wrong size ");
    }
}