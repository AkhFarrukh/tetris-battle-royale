//For the tests build for the Component 
#include <cassert>


#include <src/Component/block.cpp>
#include <src/Component/ComponentEnum.hpp>


void testBlock_01 ()
{
    Block block(Color::cyan, 150, Coordinates(15, 15));
    assert (block.getColour() == Color::cyan);
    assert (block.getSize() == 150);
    assert (block.getPosition() == Coordinates(15,15));
}
