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