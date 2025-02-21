#include "input.hpp"

Input::Input() : currentAction(Action::NONE), isRunning(true) {}  // Correct initialization with Action::NONE



/**
 *@brief Récupère l'action de l'utilisateur.
 *Et réinitilise l'action courante à None.

 *@return l'action de l'utilisateur (type Action).
 */
Action Input::getAction() {
    Action action = currentAction.load();
    currentAction = Action::NONE;  // Reset after giving the action
    return action;
}



/**
 *@brief Stop le thread d'input.
 */
void Input::stop() {       // Stop the input thread, necessary when listening
    isRunning = false;
    if (inputThread.joinable()) {
        inputThread.join();
    }
}


/**
 *@brief Démarre l'écoute de l'input dans un thread séparé.
 */
void Input::start() {
    initscr();        // Initialize ncurses
    cbreak();         // Disable line buffering
    noecho();         // Disable echoing of typed characters
    nodelay(stdscr, TRUE); // Make getch() non-blocking
    keypad(stdscr, TRUE);  // Enable special keys like arrow keys
    inputThread = std::thread(&Input::listenForInput, this);
}



/**
 *@brief Traite l'input du joueur en l'associant à une action.
 *L'action actuelle est mise à jour.
 */
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
            case 27:  // ESC key
            case KEY_CANCEL:
                currentAction = Action::ESCAPE;
                break;
            default:
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Delay to avoid CPU overload
    }
}
