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
    static std::chrono::steady_clock::time_point lastInputTime;
    auto now = std::chrono::steady_clock::now();
    if (now - lastInputTime < std::chrono::milliseconds(50)) { 
        return KeyboardAction::NO_ACTION;
    }

    int ch = getch();
    if (ch == ERR) {
        return KeyboardAction::NO_ACTION;
    }
    lastInputTime = now;

    switch (ch) {
        case KEY_LEFT:
        case 'q':
            return  KeyboardAction::LEFT_DIRECTION_KEY;
        case KEY_RIGHT:
        case 'd':
            return  KeyboardAction::RIGHT_DIRECTION_KEY;
        case KEY_UP:
        case 'z':
            return  KeyboardAction::LEFT_ROTATION_KEY;
            break;
        case 'r':
            return KeyboardAction::RIGHT_ROTATION_KEY;
            break;
        case ' ': // The space bar is registered as 32
            return KeyboardAction::HARD_DROP_KEY;
        case KEY_DOWN:
        case 's':
            return KeyboardAction::SOFT_DROP_KEY;
            break;
        case 27:  // ESC key
        case KEY_CANCEL:
            return KeyboardAction::EXIT_KEY;
        case '&':
        default:
            return KeyboardAction::NO_ACTION;
    }
}
