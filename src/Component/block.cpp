#pragma once
#include "ComponentEnum.hpp"
#include "coordinates.cpp"


class Block{
    private:
        Color colour; // Block colour as in a human language (e.g. 'red')
        int size; // As a block is a square, the only variable "size" is ok.
        Coordinates position; // The coordinates of a block
        bool empty; // True if the block is not shown on a display or put in a grid

    public:
        // A constructor
        Block(Color _colour, int _size, Coordinates _position) : 
              colour{_colour}, size{_size}, position{_position}, empty{false} {}

        // Getters
        Color getColour()           {return colour;}
        int getSize()               {return size;}
        bool isEmpty()              {return empty;}

        // Position getters
        Coordinates getPosition()   {return position;}
        int getX()                  {return position.x;}
        int getY()                  {return position.y;}

        // Setters
        void setColour(Color newColour) {colour = newColour;}
        void setSize(int newSize)       {size = newSize;}
        void setEmpty(bool newEmpty)    {empty = newEmpty;}

        // Position setters
        void setPosition(int newX, int newY)        {setX(newX); setY(newY);}
        void setX(int newX)                         {position.x = newX;}
        void setY(int newY)                         {position.y = newY;}

        // Position setter (passing 'Coordinates' object as paramater)
        void setPosition(Coordinates newPosition)   {position = newPosition;}
};