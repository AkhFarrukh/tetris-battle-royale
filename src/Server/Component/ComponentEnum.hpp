#ifndef COMPONENT_ENUM_HPP
#define COMPONENT_ENUM_HPP

//
// Created by farrukh on 23/11/24.
//

/**  
 *@brief This enum class serves to easily distinguish colors without using strings or other data types.
 * It contains generalized names of colors used in the game.
 * More colors can be added later.
 * Exact RGB values of each color are defined by the interface palette.
 */
enum class Color{
    nullcolor,  //Empty case 
    cyan,    // Line
    yellow,  // Square
    purple,  // T-shape
    green,   // S-shape (left)
    red,     // Z-shape (right)
    orange,  // L-shape (right)
    blue,    // J-shape (left)
    grey,     // Malus
    white    //Thunder & BlackOut
};

/** 
 *@brief This enum class serves to distinguish the shape of a tetromino figure.
 */
enum class TetrominoShape {
    I, // Line
    O, // Square
    T, // T-shape
    S, // S-shape (left)
    Z, // Z-shape (right)
    L,  // L-shape (right)
    J, // J-shape (left)
    ONE_BLOCK_CUBE //1X1
};
#endif