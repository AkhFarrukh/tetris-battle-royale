#ifndef FIGUREGENERATOR_HPP
#define FIGUREGENERATOR_HPP


#include "ComponentEnum.hpp"
#include "figure.hpp"
// #include "../Component/coordinates.cpp"
#include <random>

class FigureGenerator {
private:
    TetrominoShape prevShape_;
    TetrominoShape shape;

    static TetrominoShape getRandomShape();

public:
    // Constructor with specified initial shape
    FigureGenerator(TetrominoShape prevShape);

    // Constructor with random initial shape
    FigureGenerator();

    // Working function
    Figure randomFigure();
};


#endif