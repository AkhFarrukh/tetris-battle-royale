#ifndef COORDINATES_HPP
#define COORDINATES_HPP

#include <string>
#include <sstream>

/**
 * @brief Represents a pair of coordinates in a 2D space.
 *
 * The Coordinates class encapsulates the properties and operations related
 * to a pair of integer coordinates (x, y). It provides constructors,
 * methods for string representation, and operator overloads for arithmetic
 * and comparison.
 */
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
