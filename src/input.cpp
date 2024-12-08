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

Input::Input() : currentAction(Action::NONE), isRunning(true) {}  // Correct initialization with Action::NONE

// Get the action and then reset it
Action Input::getAction() {
    Action action = currentAction.load();
    currentAction = Action::NONE;  // Reset after giving the action
    return action;
}

// Stop the input thread, necessary when listening
void Input::stop() {
    isRunning = false;
    if (inputThread.joinable()) {
        inputThread.join();
    }
}

// Start listening for input in a separate thread
void Input::start() {
    initscr();        // Initialize ncurses
    cbreak();         // Disable line buffering
    noecho();         // Disable echoing of typed characters
    nodelay(stdscr, TRUE); // Make getch() non-blocking
    keypad(stdscr, TRUE);  // Enable special keys like arrow keys
    inputThread = std::thread(&Input::listenForInput, this);
}

// Listen for input and update the currentAction
void Input::listenForInput() {
    while (isRunning) {
        int key = getch();  // Non-blocking key press
        switch (key) {
            case KEY_LEFT:
            case 'q':
                currentAction = Action::MOVE_LEFT;
                break;
            case KEY_RIGHT:
            case 'd':
                currentAction = Action::MOVE_RIGHT; 
                break;
            case KEY_UP:
            case 'z':
                currentAction = Action::ROTATE_LEFT;  
                break;
            case 'r':
                currentAction = Action::ROTATE_RIGHT;
                break;
            case ' ': // The space bar is registered as 32
                currentAction = Action::DROP;  
                break;
            case KEY_DOWN:
            case 's':
                currentAction = Action::SOFT_DROP;
                break;
            default:
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Delay to avoid CPU overload
    }
}
