#include <iostream>
#include  "block.cpp"
#include  "coordinate.cpp"
#include  <vector>
using namespace std;


class Grid {
  vector<vector<Block>> matrix(20, vector<Block>(10));
  public:
    bool isLocationFree(Coordinates coordinates);
}

bool Grid::isLocationFree(Coordinates coordinates) {
  return matrix[coordinates.y][coordinates.x].has_value();

}