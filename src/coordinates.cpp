#include <string>
#include <format>



class Coordinates{
  private:

  public:
    int x, y; // (x,y) coordinates
    // A constructor
    Coordinates(int initialX, int initialY) : x{initialX}, y{initialY} {}


    // Methods
    std::string toString(){
        return std::format("({},{})", std::to_string(x), std::to_string(y));
    }

    // Operator overload
    Coordinates operator+(const Coordinates& other) const {
      return Coordinates{x + other.x, y + other.y};
    }

    Coordinates& operator+=(const Coordinates1 other){
      x += other.x;
      y += other.y;
      return *this;
    }

    Coordinates operator-(const Coordinates& other) const {
      return Coordinates{x - other.x, y - other.y};
    }

    Coordinates& operator-=(const Coordinates1 other){
      x -= other.x;
      y -= other.y;
      return *this;
    }

    bool operator==(const Coordinates& other) const{
      return x == other.x && y == other.y;
    }


};