#ifndef FIGUREGENERATOR_HPP
#define FIGUREGENERATOR_HPP


#include "ComponentEnum.hpp"
#include "Figure.hpp"
#include <random>

class FigureGenerator {
private:
    TetrominoShape prevShape_;
    TetrominoShape shape;
    bool isBonusCube = false; //Si true, on aura des tetrimino 1X1

    static TetrominoShape getRandomShape();

public:
    // Constructor with specified initial shape
    FigureGenerator(TetrominoShape prevShape);

    // Constructor with random initial shape
    FigureGenerator();

    //Interrupteur pour bonus cube 1X1
    void switchBonusCube();

    // Working function
    Figure randomFigure(bool isBonusCube);
};


#endif