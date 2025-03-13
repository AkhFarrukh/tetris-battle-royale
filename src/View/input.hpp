#ifndef INPUT_HPP
#define INPUT_HPP


#include <iostream>
#include <thread>
#include <atomic>
#include <ncursesw/ncurses.h>
#include <chrono>
#include "ControllerEnum.hpp" // This includes the Action enum class
#include "keyboardAction.hpp"

/* The ncurses lib needs to be added to the makefile in the flags section
LDFLAGS = -lncurses 

and write how to install ncurses very important

*/

class Input {
public:
    static KeyboardAction getKeyboardInput();
    static void start();
    static void stop();
};

#endif
