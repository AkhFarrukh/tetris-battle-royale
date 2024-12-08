#include <iostream>
#include <thread>
#include <chrono>
#include "input.cpp"  // Assumes your Input class is in this file

int main() {
    std::cout << "Input Test Program" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Left: Left Arrow or 'q'" << std::endl;
    std::cout << "  Right: Right Arrow or 'd'" << std::endl;
    std::cout << "  Rotate Left: Up Arrow or 'z'" << std::endl;
    std::cout << "  Rotate Right: 'r'" << std::endl;
    std::cout << "  Drop: Down Arrow or Space" << std::endl;
    std::cout << "  Soft Drop: 's'" << std::endl;
    std::cout << "Press 'Ctrl+C' to exit" << std::endl;

    Input inputHandler;
    inputHandler.start();

    while (true) {
        Action currentAction = inputHandler.getAction();
        
        // Only print if an action was detected
        switch (currentAction) {
            case Action::MOVE_LEFT:
                std::cout << "Moving Left" << std::endl;
                break;
            case Action::MOVE_RIGHT:
                std::cout << "Moving Right" << std::endl;
                break;
            case Action::ROTATE_LEFT:
                std::cout << "Rotating Left" << std::endl;
                break;
            case Action::ROTATE_RIGHT:
                std::cout << "Rotating Right" << std::endl;
                break;
            case Action::DROP:
                std::cout << "Dropping" << std::endl;
                break;
            case Action::SOFT_DROP:
                std::cout << "Soft Dropping" << std::endl;
                break;
            case Action::NONE:
                // Do nothing
                break;
        }

        // Small delay to prevent tight loop and reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}