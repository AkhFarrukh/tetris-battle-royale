#ifndef FIGUREGENERATOR_HPP
#define FIGUREGENERATOR_HPP


#include "../Component/ComponentEnum.hpp"
#include "../Component/figure.cpp"
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