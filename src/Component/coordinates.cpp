#include "coordinates.hpp"

// A constructor
Coordinates::Coordinates(int initialX, int initialY) : x{initialX}, y{initialY} {}

// // Methods
// std::string toString(){
//     return std::format("({},{})", std::to_string(x), std::to_string(y));
// }


/**
 *@brief Formate les coordonnées sous la forme (x,y).

 *@return Les coordonnées formatées.
 */
std::string Coordinates::toString() {
  std::ostringstream oss;
  oss << "(" << x << "," << y << ")";
  return oss.str();
}


// Operator overload
Coordinates Coordinates::operator+(const Coordinates& other) const { // "+" operator
  return Coordinates{x + other.x, y + other.y};
}

Coordinates& Coordinates::operator+=(const Coordinates& other){      // "+=" operator
  x += other.x;
  y += other.y;
  return *this;
}


Coordinates Coordinates::operator-(const Coordinates& other) const { // "-" operator
  return Coordinates{x - other.x, y - other.y};
}

Coordinates& Coordinates::operator-=(const Coordinates& other){      // "-=" operator
  x -= other.x;
  y -= other.y;
  return *this;
}

bool Coordinates::operator==(const Coordinates& other) const{        // "==" operator
  return x == other.x && y == other.y;
}

