//
// Created by farrukh on 9/12/24.
//

#ifndef MOVINGFIGURE_HPP
#define MOVINGFIGURE_HPP

#include <cmath>
#include <cstdlib>
#include <numbers>
#include <ranges>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "Block.hpp"
#include "figure.hpp"
#include "grid.hpp"


/**
 * @brief Represents a movable figure in the game.
 *
 * The MovingFigure class is responsible for managing the state and behavior of
 * a tetromino figure that moves on the game grid. It extends the Figure class
 * and includes additional attributes and methods for handling the position,
 * rotation, and block positions of the tetromino on the grid.
 *
 * Responsibilities include:
 * - Maintaining the current position and rotation state of the tetromino.
 * - Calculating and updating the actual coordinates of each block of the tetromino.
 * - Interacting with the game grid to determine valid movements and rotations.
 * - placing tetromino on the grid after it stops moving.
 */
class MovingFigure : public Figure{
private:
    std::shared_ptr<Figure> tetromino_;
    Coordinates position_; //position of centerpiece of tetromino

    //vector of actual coordinates of each block
    std::vector<Coordinates> blockPositions_;

    //shared pointer to grid
    std::shared_ptr<Grid> grid_;

    unsigned int rotationState = 0; //todo remove because rotation state is now in figure

    Coordinates getSpawnPosition();
    void updateBlocks();
    void updateBlocks(
            const Coordinates& position,
            const shared_ptr<Figure>& tetromino,
            std::vector<Coordinates>& blockPositions
    );

    void checkBlockPositions();

    void initializeBlockPosition(
            int index,
            const Coordinates& position,
            const shared_ptr<Figure>& tetromino,
            std::vector<Coordinates>& blockPositions
    );

    void updateBlockPosition(
            int index,
            const Coordinates& position,
            const shared_ptr<Figure>& tetromino,
            std::vector<Coordinates>& blockPositions
    );

    bool allowBlock(Coordinates blockPosition);
    bool allowAction(
            const Coordinates newPosition,
            const shared_ptr<Figure>& newFigure
    );

public:
    //Constructor
    MovingFigure(std::shared_ptr<Figure> tetromino, std::shared_ptr<Grid> grid);

    //Getter & Setter
    Coordinates getPosition();
    bool setPosition(const Coordinates newPosition);
    shared_ptr<Figure> getFigure();
    bool setFigure(const shared_ptr<Figure>& newFigure);

    bool setPosAndFig(
            const Coordinates newPosition,
            const shared_ptr<Figure>& newFigure
    );

    std::vector<Block> getBlocksRealPositions();
    //Public methods
    bool spawnSucceeded();

    Color getMovingFigureBlockColor(Coordinates coordinates);

    //todo remove next 3 methods because rotation state is now in figure
    unsigned int getCurrentRotationState();
    void rotateRight();
    void rotateLeft();

};

#endif //MOVINGFIGURE_HPP
