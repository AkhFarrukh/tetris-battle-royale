#include <iostream>
#include <string>
#include "coordinates.cpp"
#include "tetrominocolor.cpp"

class Block{
    private:
        Color colour; // Block colour as in a human language (e.g. 'red')
        int size; // As a block is a square, the only variable "size" is ok.
        Coordinates position; // The coordinates of a block
        bool empty;

    public:
        // A constructor
        Block(Color _colour, int _size, Coordinates _position) : 
              colour{_colour}, size{_size}, position{_position}, empty{false} {}

        // Getters
        Color getColour()           {return colour;}
        int getSize()               {return size;}
        Coordinates getPosition()   {return position;}
        bool isEmpty()              {return empty;}

        // Setters
        void setColour(Color newColour) {colour = newColour;}
        void setSize(int newSize)       {size = newSize;}
        void setEmpty(bool newEmpty)    {empty = newEmpty;}
};
