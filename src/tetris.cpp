#include "InterFaceTerminal.hpp"

int main() {
    while(true){
        {
        InterfaceTerminal terminal;
        terminal.start(); 
        } 
        // The terminal is destryed but i dont know about the game object and its component(pointers and so)
        // Might be the thing that makes the second and so terminals buggy
    }
    return 0;
}

//add a loop so that if i press escape in game i come back to the menu, but also when i end a game
