#include <iostream>
#include <thread>
#include <atomic>
#include <conio.h> // For capturing keyboard input (Windows-specific)

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
    }

    // Start listening for input in a separate thread
    void start() {
        inputThread = std::thread(&Input::listenForInput, this);
    }

    // Stop the input thread
    void stop() {
        isRunning = false;
        if (inputThread.joinable()) {
            inputThread.join();
        }
    }

    // Retrieve the latest action and reset it
    Action getAction() {
        Action action = currentAction.load();
        currentAction = NONE; // Reset after consuming
        return action;
    }

private:
    std::atomic<Action> currentAction;
    std::atomic<bool> isRunning;
    std::thread inputThread;

    // Listen for keyboard inputs
    void listenForInput() {
        while (isRunning) {
            if (_kbhit()) { // Check if a key is pressed
                char key = _getch(); // Get the key
                switch (key) {
                    case 'q':
                        currentAction = MOVE_LEFT;
                        break;
                    case 'd':
                        currentAction = MOVE_RIGHT;
                        break;
                    case 'z':
                        currentAction = ROTATE;
                        break;
                    case 's':
                        currentAction = DROP;
                        break;
                    default:
                        break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Polling delay
        }
    }
};



