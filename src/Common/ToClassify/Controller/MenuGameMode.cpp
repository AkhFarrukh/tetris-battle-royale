//
// Created by farrukh on 25/02/25.
//

#include "MenuGameMode.hpp"


// Prefix increment (++mode)
MenuGameMode& operator++(MenuGameMode& mode) {
    if (mode == ROYAL_COMPETITION) return mode; // Prevent overflow
    mode = static_cast<MenuGameMode>(static_cast<int>(mode) + 1);
    return mode;
}

// Postfix increment (mode++)
MenuGameMode operator++(MenuGameMode& mode, int) {
    MenuGameMode old = mode;
    ++mode; // Use the prefix version
    return old;
}

// Prefix decrement (--mode)
MenuGameMode& operator--(MenuGameMode& mode) {
    if (mode == ENDLESS) return mode; // Prevent underflow
    mode = static_cast<MenuGameMode>(static_cast<int>(mode) - 1);
    return mode;
}

// Postfix decrement (mode--)
MenuGameMode operator--(MenuGameMode& mode, int) {
    MenuGameMode old = mode;
    --mode; // Use the prefix version
    return old;
}