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

#include "tetrominoshape.cpp"
#include "coordinates.cpp"
#include "block.cpp"
#include "color.cpp"
#include "figure.hpp"
#include "grid.cpp"

class MovingFigure : public Figure{
private:
    std::shared_ptr<Figure> tetromino_;
    Coordinates position_; //position of centerpiece of tetromino

    //vector of actual coordinates of each block
    std::vector<Coordinates> blockPositions_;

    //shared pointer to grid
    std::shared_ptr<Grid> grid_;



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

    //Public methods
    bool spawnSucceeded();

};

#endif //MOVINGFIGURE_HPP
