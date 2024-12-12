#include "InterFaceTerminal.hpp"
#include <ncurses.h>

InterfaceTerminal::InterfaceTerminal() : gameClass() {}

InterfaceTerminal::~InterfaceTerminal() {}

void InterfaceTerminal::start() {
    // Initialize ncurses
    initscr();            // Start curses mode
    cbreak();             // Line buffering disabled
    noecho();             // Don't echo() while we do getch
    keypad(stdscr, TRUE); // We get F1, F2 etc..
    curs_set(0);          // Hide the cursor
    
    const int numModes = 4;
    int selectedMode = 0;  // Tracks the currently selected menu option

    while (true) {
        // Clear the screen before printing the updated menu
        clear();

        // Print the menu
        printw("Welcome to Tetris Royale (Terminal Edition)!\n");
        printw("Select a game mode then press Enter to start:\n\n");
        // Game Modes 
        const char* modeNames[] = {
            "Endless Mode",
            "Classic Mode",
            "Duel Mode", 
            "Royal Competition Mode"
        };
        // Prints the modes right
        for (int i = 0; i < numModes; ++i) {
            if (i == selectedMode) {
                printw("-> ");  // If "hovered"  
            } else {
                printw("   ");
            }
            
            printw("%d. %s", i + 1, modeNames[i]);
            
            if (i != 0) {
                printw(" (Unreleased)");
            }
            printw("\n");
        }
        
        printw("\nUse UP and DOWN arrow keys to navigate\n");
        printw("Press ENTER to select a mode\n");
        printw("Press 'q' to quit\n");

        // Refresh the screen to display updates
        refresh();

        // Get user input directly
        int ch = getch();

        switch (ch) {
            case KEY_UP:
                if (selectedMode > 0) {
                    selectedMode--;
                }
                break;

            case KEY_DOWN:
                if (selectedMode < numModes - 1) {
                    selectedMode++;
                }
                break;

            case '\n':  // Enter key
                if (selectedMode == 0) {  // Only Endless Mode is functional
                    clear();
                    printw("Endless Mode selected! Starting game...\n");
                    refresh();
                    
                    // Small delay to show the selection
                    napms(500);
                    endwin();
                    gameClass.run();  // Start the selected game mode
                    return;  // Exit the method after game is done
                } else {
                    // litte msg for the ppl that will still click  ;)
                    clear();
                    printw("%s is not available yet.\n", modeNames[selectedMode]);
                    printw("Press any key to return to menu...\n");
                    refresh();
                    getch();  // Wait for a keypress and keeps the screen from being refreshed
                }
                break;
            //quit the menu :/
            case 'q':
            case 'Q':
                endwin();
                return;  // Exit the method
        }
    }
}
