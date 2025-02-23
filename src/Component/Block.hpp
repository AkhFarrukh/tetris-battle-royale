#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "ComponentEnum.hpp"
#include "coordinates.hpp"

/**
 * @brief Class representing a single block of a tetromino figure.
 */
class Block{
    private:
        Color colour; // Block colour as in a human language (e.g. 'red')
        int size; // As a block is a square, the only variable "size" is ok.
        Coordinates position; // The coordinates of a block
        bool empty; // True if the block is not shown on a display or put in a grid

    public:
        // A constructor
        Block(Color _colour, int _size, Coordinates _position); 

        // Getters
        Color getColour();
        int getSize();
        bool isEmpty();

        // Position getters
        Coordinates getPosition();
        int getX();
        int getY();

        // Setters
        void setColour(Color newColour);
        void setSize(int newSize);
        void setEmpty(bool newEmpty);

        // Position setters
        void setPosition(int newX, int newY);
        void setX(int newX);
        void setY(int newY);

        // Position setter (passing 'Coordinates' object as paramater)
        void setPosition(Coordinates newPosition);
};

#endif