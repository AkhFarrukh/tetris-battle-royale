#ifndef COORDINATES_HPP
#define COORDINATES_HPP

#include <string>
#include <format>
#include <sstream>


class Coordinates{

  public:
    int x, y; // (x,y) coordinates

    // A constructor
    Coordinates(int initialX, int initialY);

    // // Methods
    // std::string toString(){
    //     return std::format("({},{})", std::to_string(x), std::to_string(y));
    // }

    std::string toString();

    // Operator overload
    Coordinates operator+(const Coordinates& other) const;

    Coordinates& operator+=(const Coordinates& other);

    Coordinates operator-(const Coordinates& other) const;

    Coordinates& operator-=(const Coordinates& other);

    bool operator==(const Coordinates& other) const;
    
};


#endif