#include "Input.hpp"


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
        case 38:
        //case '&':
        case '1':
        case 'm':
            return KeyboardAction::NUMBER_KEY_1;
        case '2':
        case 169:
            return KeyboardAction::NUMBER_KEY_2;
        case 34:
        case '3':
            return KeyboardAction::NUMBER_KEY_3;
        case 39:
        case '4':
            return KeyboardAction::NUMBER_KEY_4;
        case 40:
        case '5':
            return KeyboardAction::NUMBER_KEY_5;
        case '6':
        case 45:
        case 'o':
            return KeyboardAction::NUMBER_KEY_6;
        case '7':
        case 168:
        case 'l':
            return KeyboardAction::NUMBER_KEY_7;
        case '!':
        case '8':
        case 95:
            return KeyboardAction::NUMBER_KEY_8;
        case 167:
        case '9':
            return KeyboardAction::NUMBER_KEY_9;
        case '0':
        case 160:
            return KeyboardAction::NUMBER_KEY_0;

        default:
            return KeyboardAction::NO_ACTION;
    }
}
