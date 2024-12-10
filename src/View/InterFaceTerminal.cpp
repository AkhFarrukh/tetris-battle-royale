#include "InterFaceTerminal.hpp"

InterfaceTerminal::InterfaceTerminal() : gameClass() {}

InterfaceTerminal::~InterfaceTerminal() {}

void InterfaceTerminal::start() {
    std::cout << "Welcome to Tetris Royale (Terminal Edition)!\n";
    std::cout << "Press Ctrl+C to exit at any time.\n";
    gameClass.run();
}

