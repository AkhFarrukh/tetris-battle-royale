#include <iostream>
#include <thread>
#include <atomic>
#include <conio.h> // for windows ill add for linux when i find out how ;)

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

    // Stop the input thread because we dont need it active at all times :)
    void stop() {
        isRunning = false;
        if (inputThread.joinable()) {
            inputThread.join();
        }
    }

    // Retrieve the latest action and reset it
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
            if (_kbhit()) { // Check if a key is pressed for windows
                char key = _getch(); // Get the key
                switch (key) {
                    case 'LEFT' | 'q':
                        currentAction = MOVE_LEFT;
                        break;
                    case 'RIGHT'| 'd':
                        currentAction = MOVE_RIGHT;
                        break;
                    case 'UP'| 'z':
                        currentAction = ROTATE;
                        break;
                    case 'DOWN'| 's':
                        currentAction = DROP;
                        break;
                    default:
                        break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // delay to not overload the cpu
        }
    }
};