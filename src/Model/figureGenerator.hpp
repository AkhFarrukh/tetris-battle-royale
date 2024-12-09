#include "tetrominoShape.cpp"
#include "figure.cpp"
#include "coordinates.cpp"
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
