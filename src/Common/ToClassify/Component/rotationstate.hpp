//
// Created by farrukh on 16/02/25.
//

#ifndef ROTATIONSTATE_HPP
#define ROTATIONSTATE_HPP

/*
 * @brief Defines the possible rotation states for Tetrominoes of Figure class.
 *
 * The `RotationState` enum class represents the four possible orientations of a Tetromino
 * in the game. These states correspond to the Tetromino's orientation relative to the
 * initial spawn state (0° rotation).
 *
 * States:
 * - `Spawn` (0°): The initial spawn state of the Tetromino.
 * - `Right` (90°): The state resulting from a single clockwise rotation from the spawn state.
 * - `Left` (-90°): The state resulting from a single counter-clockwise rotation from the spawn state.
 * - `Two` (180°): The state resulting from two successive rotations (in either direction) from the spawn state.
 *
 * https://tetris.wiki/Super_Rotation_System
 */

enum RotationState {
    SPAWN = 0,   // Initial spawn state (0°)
    RIGHT = 1, // Clockwise rotation (90°)
    LEFT = 2, // Counter-clockwise rotation (-90°)
    TWO = 3 // Two successive rotations (180°)
};
#endif //ROTATIONSTATE_HPP
