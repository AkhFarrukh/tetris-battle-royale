#include <iostream>
#include <thread>
#include <atomic>
#include <ncurses.h>
#include <chrono>

/* The ncurses lib needs to be added to the makefile*/

class Input {
public:
    enum Action {
        NONE,
        MOVE_LEFT,
        MOVE_RIGHT,
        ROTATE,
        DROP
    };

    Input() : currentAction(NONE), isRunning(true) {}

    ~Input() {
        stop();
        endwin(); // Restore terminal to normal state
    }

    // Start listening for input in a separate thread
    void start() {
        initscr();       // Initialize ncurses
        cbreak();        // Disable line buffering
        noecho();        // Disable echoing of typed characters
        nodelay(stdscr, TRUE); // Make getch() non-blocking
        keypad(stdscr, TRUE);  // Enable special keys like arrow keys
        inputThread = std::thread(&Input::listenForInput, this);
    }

    // Stop the input thread cuz needed only when listening
    void stop() {
        isRunning = false;
        if (inputThread.joinable()) {
            inputThread.join();
        }
    }

    Action getAction() {
        Action action = currentAction.load();
        currentAction = NONE; // Reset after giving the info so it can be reused
        return action;
    }

private:
    std::atomic<Action> currentAction;
    std::atomic<bool> isRunning;
    std::thread inputThread;

    void listenForInput() {
        while (isRunning) {
            int key = getch(); // Non-blocking key press
            switch (key) {
                case KEY_LEFT:
                case 'q': 
                    currentAction = MOVE_LEFT;
                    break;
                case KEY_RIGHT:
                case 'd': 
                    currentAction = MOVE_RIGHT;
                    break;
                case KEY_UP:
                case 'z': 
                    currentAction = ROTATE;
                    break;
                case KEY_DOWN:
                case 's': 
                    currentAction = DROP;
                    break;
                default:
                    break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Delay to avoid CPU overload
        }
    }
};
