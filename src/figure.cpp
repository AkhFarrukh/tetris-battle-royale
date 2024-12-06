#include "figure.hpp"

#include "color.cpp"
#include "tetrominoshape.cpp"
#include "coordinates.cpp"
#include "block.cpp"

#include <vector>
#include <cassert>

using namespace std;


//Constructor
Figure::Figure(TetrominoShape _figShape): figShape{_figShape}{
    switch (figShape)
    {
    case TetrominoShape::I :
        Color color = Color::cyan;                  //Color
        Block pivot(color, BLOCK_SIZE, center);     //First block at (0,0), the others will be offset
        center = Coordinates(BLOCK_SIZE/2, BLOCK_SIZE/2);     //For the I, the center isn't (0,0)
        figBlocks = {pivot,
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY())),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 2 * BLOCK_SIZE, pivot.getY())),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() - 1 * BLOCK_SIZE, pivot.getY()))};
        break;
    case TetrominoShape::O :
        Color color = Color::yellow;
        Block pivot(color, BLOCK_SIZE, center);
        figBlocks = {pivot,
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY())),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY() + 1 * BLOCK_SIZE)),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() , pivot.getY() + 1 * BLOCK_SIZE))};
        break;
    case TetrominoShape::T :
        Color color = Color::purple;
        Block pivot(color, BLOCK_SIZE, center);
        figBlocks = {pivot,
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY())),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() , pivot.getY() + 1 * BLOCK_SIZE)),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() - 1 * BLOCK_SIZE, pivot.getY()))};
        break;
    case TetrominoShape::S :
        Color color = Color::green;
        Block pivot(color, BLOCK_SIZE, center);
        figBlocks = {pivot,
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX(), pivot.getY() + 1 * BLOCK_SIZE)),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() - 1 * BLOCK_SIZE, pivot.getY() + 1 * BLOCK_SIZE)),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY()))};
        break;
    case TetrominoShape::Z :
        Color color = Color::red;
        Block pivot(color, BLOCK_SIZE, center);
        figBlocks = {pivot,
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX(), pivot.getY() + 1 * BLOCK_SIZE)),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY() + 1 * BLOCK_SIZE)),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() - 1 * BLOCK_SIZE, pivot.getY()))};
        break;
    case TetrominoShape::L :
        Color color = Color::orange;
        Block pivot(color, BLOCK_SIZE, center);
        figBlocks = {pivot,
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY())),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY() - 1 * BLOCK_SIZE)),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() - 1 * BLOCK_SIZE, pivot.getY()))};
        break;
    case TetrominoShape::J :
        Color color = Color::blue;
        Block pivot(color, BLOCK_SIZE, center);
        figBlocks = {pivot,
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY())),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() + 1 * BLOCK_SIZE, pivot.getY() + 1 * BLOCK_SIZE)),
            Block(color, BLOCK_SIZE, Coordinates(pivot.getX() - 1 * BLOCK_SIZE, pivot.getY()))};
        break;
    default:
        break;
    }
};


void Figure::invariant() {
    assert(figBlocks.size() == 4 && "Error : Not 4 blocks in the figure ");
    for (Block _figBlock : figBlocks)
    {
        assert(_figBlock.getSize() != BLOCK_SIZE && "Error : A block has a wrong size ");
    }
}

