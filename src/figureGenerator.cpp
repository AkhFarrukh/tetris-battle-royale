#include "figureGenerator.hpp"


// Constructor with specified initial shape
FigureGenerator::FigureGenerator(TetrominoShape prevShape) : prevShape_{prevShape} {
}

// Constructor with random initial shape
FigureGenerator::FigureGenerator() : prevShape_{getRandomShape()} {
}

// Static method to generate a random TetrominoShape
TetrominoShape FigureGenerator::getRandomShape() {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> dis(0, 6);

    return static_cast<TetrominoShape>(dis(gen));
}

// Generate a random Figure, ensuring no consecutive duplicates
Figure FigureGenerator::randomFigure() {
    do {
        shape = getRandomShape();
    } while (shape == prevShape_);

    prevShape_ = shape; 
    return Figure(shape);
}
