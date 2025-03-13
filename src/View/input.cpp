#include "input.hpp"


/**
 *@brief Initialise la fenêtre ncurses.
 */
void Input::start() {
    initscr();        // Initialize ncurses
    cbreak();         // Disable line buffering
    noecho();         // Disable echoing of typed characters
    nodelay(stdscr, TRUE); // Make getch() non-blocking
    keypad(stdscr, TRUE);  // Enable special keys like arrow keys
}

void Input::stop() {
    endwin();
}


/**
 *@brief Récupère l'input du joueur.
 *
 *@return l'action du joueur.
 */
KeyboardAction Input::getKeyboardInput() {
    int ch = getch();  // Non-blocking key press
    if (ch == ERR) {
        return KeyboardAction::NO_ACTION;
    }
    while (getch() != ERR);

    KeyboardAction currentAction = KeyboardAction::NO_ACTION;
    switch (ch) {
        case KEY_LEFT:
        case 'q':
            currentAction = KeyboardAction::LEFT_DIRECTION_KEY;
            break;
        case KEY_RIGHT:
        case 'd':
            currentAction = KeyboardAction::RIGHT_DIRECTION_KEY;
            break;
        case KEY_UP:
        case 'z':
            currentAction = KeyboardAction::LEFT_ROTATION_KEY;
            break;
        case 'r':
            currentAction = KeyboardAction::RIGHT_ROTATION_KEY;
            break;
        case ' ': // The space bar is registered as 32
            currentAction = KeyboardAction::HARD_DROP_KEY;
            break;
        case KEY_DOWN:
        case 's':
            currentAction = KeyboardAction::SOFT_DROP_KEY;
            break;
        case 27:  // ESC key
        case KEY_CANCEL:
            currentAction = KeyboardAction::EXIT_KEY;
            break;
        default:
            currentAction = KeyboardAction::NO_ACTION;
            break;
    }
    return currentAction;
}
