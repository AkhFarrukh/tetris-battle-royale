//
// Created by farrukh on 27/02/25.
//

#include "RenderEngineNcurses.hpp"

#include <utility>

const int GRID_HEIGHT = 20; //todo move some place better
const int GRID_WIDTH = 10;



void RenderEngineNcurses::renderGames(const vector<shared_ptr<Game::NetPlayerData>>& players) {
    erase();
    Coordinates offset(0, 0);
    const int maxPlayersPerRow = 5; //todo move to a better place

    for (size_t i = 0; i < players.size(); ++i) {
        if (i > 0 && i % maxPlayersPerRow == 0) {
            offset.x = 0;
            offset.y += GRID_HEIGHT + 4; // Adjust offset for next row
        }


        renderGame(players[i], offset);
        offset.x += GRID_WIDTH * 2 + 3; // Adjust offset for next game
    }
    refresh();
}

void RenderEngineNcurses::renderGame(shared_ptr<Game::NetPlayerData> player, Coordinates offset) {
    mvprintw(offset.y, offset.x, "Score: %d", player->score);
    mvprintw(offset.y + 1, offset.x, "Lines: %d", player->linesCleared);
    mvprintw(offset.y + 2, offset.x, "+--------------------+");
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        mvprintw(y + 3 + offset.y, offset.x, "|");
        for (int x = 0; x < GRID_WIDTH; ++x) {
            Color blockColor = Color::nullcolor;

            int index = y * GRID_WIDTH + x;// Convert 2D coordinates to 1D index

            // Check if the index is within bounds
            if (index < player->grid.size()) {
                blockColor = player->grid[index];
            }

            if (blockColor != Color::nullcolor) {
                int colorIndex = applyColor(blockColor);
                mvprintw(y + 3 + offset.y, offset.x + x * 2 + 1, "\u2588\u2588");
                attroff(COLOR_PAIR(colorIndex));
            } else {
                mvprintw(y + 3 + offset.y, offset.x + x * 2 + 1, "  ");
            }
        }
        mvprintw(y + 3 + offset.y, offset.x + GRID_WIDTH * 2 + 1, "|");
    }
    mvprintw(offset.y + GRID_HEIGHT + 3, offset.x, "+--------------------+");
}

void RenderEngineNcurses::renderGame(shared_ptr<Game::NetPlayerData> player) {
    renderGame(player, Coordinates(0, 0));
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
     case Color::red:         pairIndex = 1; break;
     case Color::green:       pairIndex = 2; break;
     case Color::yellow:      pairIndex = 3; break;
     case Color::blue:        pairIndex = 4; break;
     case Color::orange:      pairIndex = 5; break;
     case Color::cyan:        pairIndex = 6; break;
     case Color::purple:      pairIndex = 7; break;
     case Color::grey:        pairIndex = 8; break;
     // todo case Color::white:       pairIndex = 9; break;
     default:                 pairIndex = 0; break;
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