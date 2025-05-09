#include "FigureGenerator.hpp"


// Constructor with specified initial shape
FigureGenerator::FigureGenerator(TetrominoShape prevShape) : prevShape_{prevShape} {
}

// Constructor with random initial shape
FigureGenerator::FigureGenerator() : prevShape_{getRandomShape()} {
}


/**
 *@brief methode statique qui génère une forme de tetromino au hasard.
 */
TetrominoShape FigureGenerator::getRandomShape() {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> dis(0, 6);

    return static_cast<TetrominoShape>(dis(gen));
}

/**
 *@brief Génère une figure au hasard et s'assure qu'il n'y ait pas de similaires consecutives.
 */
Figure FigureGenerator::randomFigure(bool isBonusCube) {
    if (isBonusCube) {
        return Figure(TetrominoShape::ONE_BLOCK_CUBE); //Bonus (RC mode)
    }
    do {
        shape = getRandomShape();
    } while (shape == prevShape_);

    prevShape_ = shape; 
    return Figure(shape);
}
