#include <iostream>
#include <string>
#include <format>


class Coordinates{
    private:
        int x, y; // (x,y) coordinates

    public:
        // A constructor
        Coordinates(int initialX, int initialY) : x{initialX}, y{initialY} {}

        // Getters
        int getX() {return x;}
        int getY() {return y;}

        // Setters
        void setX(int newX) {x = newX;}
        void setY(int newY) {y = newY;}

        // Methods
        std::string toString(){
            return std::format("({},{})", std::to_string(x), std::to_string(y));
        }
};


class Block{
    private:
        std::string colour; // Block colour as in a human language (e.g. 'red')
        int size; // As a block is a square, the only variable "size" is ok.
        Coordinates position; // The coordinates of a block

    public:
        // A constructor
        Block(std::string _colour, int _size, Coordinates _position) : 
              colour{_colour}, size{_size}, position{_position} {}

        // Getters
        std::string getColour()     {return colour;}
        int getSize()               {return size;}
        Coordinates getPosition()   {return position;}

        // Setters
        void setColour(std::string newColour)   {colour = newColour;}
        void setSize(int newSize)               {size = newSize;}
};