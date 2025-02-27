//
// Created by farrukh on 27/02/25.
//

#include "RenderEngineNcurses.hpp"

#include <utility>

const int GRID_HEIGHT = 20; //todo move some place better
const int GRID_WIDTH = 10;



/**
 *@brief affiche le jeu dans son etat actuel.
 * Affichage de la grille de jeu, tetromino (block) , infos de jeu.
 */
void RenderEngineNcurses::renderGame(shared_ptr<Game> game){
    erase();
    mvprintw(0, 0, "Score: %d", game->getGameScore());
    mvprintw(1, 0, "Lines: %d", game->getGameNumberOfBrokenLines());
    mvprintw(2, 0, "+--------------------+");

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        mvprintw(y + 3, 0, "|");
        for (int x = 0; x < GRID_WIDTH; ++x) {
            Color blockColor = game->getBlockColor(Coordinates(x, y));
            if (blockColor != Color::nullcolor) {
                int index = RenderEngineNcurses::applyColor(blockColor);
                mvprintw(y + 3, x * 2 + 1, "\u2588\u2588");
                attroff(COLOR_PAIR(index));
            } else {
                mvprintw(y + 3, x * 2 + 1, "  ");
            }
        }
        mvprintw(y + 3, 21, "|");
    }
    mvprintw(23, 0, "+--------------------+");
    refresh();

}



/**
 *@brief convertit une couleur (de block) en l'associant à un code couleur ncurses.
 *permet ainsi l'affichage correcte à l'écran des blocks.
 *
 *@param blockColor couleur du block (et de type Color).
 *@return l'index du de la couleur
 */
 int RenderEngineNcurses::applyColor(Color blockColor) {
    int pairIndex = 0;
    switch (blockColor) {
        case Color::red:    pairIndex = 1; break;
        case Color::green:  pairIndex = 2; break;
        case Color::yellow: pairIndex = 3; break;
        case Color::blue:   pairIndex = 4; break;
        case Color::orange: pairIndex = 5; break;
        case Color::cyan:   pairIndex = 6; break;
        default:            pairIndex = 0; break;
    }

    if (pairIndex != 0) {
        attron(COLOR_PAIR(pairIndex));
    }
    return pairIndex;
}


void RenderEngineNcurses::printGameOver() {
    mvprintw(24, 0, "Game Over");
    refresh();
}