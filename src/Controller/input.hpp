#ifndef INPUT_HPP
#define INPUT_HPP


#include <iostream>
#include <thread>
#include <atomic>
#include <ncurses.h>
#include <chrono>
#include "action.cpp"  // This includes the Action enum class

/* The ncurses lib needs to be added to the makefile in the flags section
LDFLAGS = -lncurses 

and write how to install ncurses very important

*/

class Input {
private:
    std::atomic<Action> currentAction;  // Now using the scoped Action enum
    std::atomic<bool> isRunning;
    std::thread inputThread;

    void listenForInput();

public:
    Input();
    ~Input() {
        stop();
        endwin(); // Restore terminal to normal state
    }

    void start();
    void stop();
    Action getAction();
};

#endif