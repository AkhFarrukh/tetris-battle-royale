#include <string>
#include <format>
#include <sstream>


class Coordinates{

  public:
    int x, y; // (x,y) coordinates

    // A constructor
    Coordinates(int initialX, int initialY) : x{initialX}, y{initialY} {}

    // // Methods
    // std::string toString(){
    //     return std::format("({},{})", std::to_string(x), std::to_string(y));
    // }

    std::string toString() {
      std::ostringstream oss;
      oss << "(" << x << "," << y << ")";
      return oss.str();
    }


    // Operator overload
    Coordinates operator+(const Coordinates& other) const { // "+" operator
      return Coordinates{x + other.x, y + other.y};
    }

    Coordinates& operator+=(const Coordinates& other){      // "+=" operator
      x += other.x;
      y += other.y;
      return *this;
    }

    Coordinates operator-(const Coordinates& other) const { // "-" operator
      return Coordinates{x - other.x, y - other.y};
    }

    Coordinates& operator-=(const Coordinates& other){      // "-=" operator
      x -= other.x;
      y -= other.y;
      return *this;
    }

    bool operator==(const Coordinates& other) const{        // "==" operator
      return x == other.x && y == other.y;
    }
    
};