#include "Block.hpp"


Color Block::getColour()           {return colour;}
int Block::getSize()               {return size;}
bool Block::isEmpty()              {return empty;}

// Position getters
Coordinates Block::getPosition()   {return position;}
int Block::getX()                  {return position.x;}
int Block::getY()                  {return position.y;}
// Setters
void Block::setColour(Color newColour) {colour = newColour;}
void Block::setSize(int newSize)       {size = newSize;}
void Block::setEmpty(bool newEmpty)    {empty = newEmpty;}
// Position setters
void Block::setPosition(int newX, int newY)        {setX(newX); setY(newY);}
void Block::setX(int newX)                         {position.x = newX;}
void Block::setY(int newY)                         {position.y = newY;}
// Position setter (passing 'Coordinates' object as paramater)
void Block::setPosition(Coordinates newPosition)   {position = newPosition;}