//
// Created by farrukh on 27/02/25.
//

#include "RenderEngineNcurses.hpp"

#include <utility>

const int GRID_HEIGHT = 20; //todo move some place better
const int GRID_WIDTH = 10;





/*
 * @brief find index of the client player in the players vector.
 */
int RenderEngineNcurses::getClientPlayerIndex(const vector<NetPlayerData>& players, bool targetPlayerMenu) {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players.at(i).playerId == -1) {
            return i;
        }
    }
    return 0; // Return 0 if no player no client player found (spectator mode)
}

/*
 * @brief Render malus menu for the client player in royal mode.
 */
void RenderEngineNcurses::drawMalusMenu(const NetPlayerData& playerClient) {
    // Calculate position for the right-side menu
    // Position it after the last game in the first row
    const int maxPlayersPerRow = 5;//todo define in the beginning
    int menuX = (GRID_WIDTH * 2 + 3) * maxPlayersPerRow;
    int menuY = 0;

    // Draw menu box
    mvprintw(menuY, menuX,     "+---------------------------+"); // todo make increments less dependent on menuY
    mvprintw(menuY + 1, menuX, "| MENU MALUS / BONUS        |");
    mvprintw(menuY + 2, menuX, "+---------------------------+");

    // Get energy from the first player for display (assuming all players have same energy in royal mode)
    mvprintw(menuY + 3, menuX, "| Energy: %-16d |", 42); //todo change to playerClient.energy when it will be available
    //mvprintw(menuY + 3, menuX, "| Energy: %-16d |", playerClient.energy);
    mvprintw(menuY + 4, menuX, "+---------------------------+");
    mvprintw(menuY + 5, menuX, "| Available Maluses:        |");

    // List of available maluses with their corresponding number keys
    const char* maluses[] = {
        "1. Increase fall speed",
        "2. Reverse controls",
        "3. Freeze controls",
        "4. Blackout",
        "5. Thunder",
    };

    for (int i = 0; i < 5; i++) {
        mvprintw(menuY + 6 + i, menuX, "| %-25s |", maluses[i]);
    }

    mvprintw(menuY + 11, menuX, "+---------------------------+");
    mvprintw(menuY + 12, menuX, "| Available Bonuses:        |");

    // List of available maluses with their corresponding number keys
    const char* bonuses[] = {
        "6. Bonus block 1x1",
        "7. Reduce fall speed",
    };

    for (int i = 0; i < 2; i++) {
        mvprintw(menuY + 13 + i, menuX, "| %-25s |", bonuses[i]);
    }
    mvprintw(menuY + 15 , menuX, "+---------------------------+");
}


/*
 * @brief Render target player menu for the client player in royal mode.
 */
void RenderEngineNcurses::drawPlayerSelectionMenu(const vector<NetPlayerData>& players) {
    // Calculate position for the right-side menu (same as malus menu)
    const int maxPlayersPerRow = 5; //todo define in the beginning
    int menuX = (GRID_WIDTH * 2 + 3) * maxPlayersPerRow;
    int menuY = 0;

    // Draw menu box
    mvprintw(menuY, menuX,     "+---------------------------+");
    mvprintw(menuY + 1, menuX, "| SELECT TARGET PLAYER      |");
    mvprintw(menuY + 2, menuX, "+---------------------------+");

    // List available players with their corresponding number keys
    int menuItemCount = 0;
    for (size_t i = 0; i < players.size() && i <= 9; ++i) {

        // Display player ID with corresponding key
        if (menuItemCount < 9) { // Limit to 9 players (keys 1-9)
            mvprintw(menuY + 3 + menuItemCount, menuX, "| %d. Player ID: %-11d |",
                menuItemCount + 1, players.at(i).playerId);
            menuItemCount++;
        }
    }

    //todo maybe later
    // Draw the bottom of the box
    mvprintw(menuY + 3 + menuItemCount, menuX, "+---------------------------+");
    //mvprintw(menuY + 4 + menuItemCount, menuX, "| Press 0 to return         |");
    //mvprintw(menuY + 5 + menuItemCount, menuX, "+---------------------------+");
}



/*
 * @brief Render the games grid with additional menus for royal mode.
 */
void RenderEngineNcurses::renderGamesRoyal(const vector<NetPlayerData>& players, bool targetPlayerMenu) {
    erase();
    // Render the regular games grid
    renderGames(players);
    int clientPlayerIndex = getClientPlayerIndex(players, targetPlayerMenu);
    if (clientPlayerIndex == -1) {
        // Handle the case where no client player is found
        return;
    }
    NetPlayerData playerClient = players.at(clientPlayerIndex);
    if (!targetPlayerMenu) {
        drawMalusMenu(playerClient); // Draw the malus menu
    } else {
        drawPlayerSelectionMenu(players); // Draw the player selection menu
    }
}

void RenderEngineNcurses::renderGames(const vector<shared_ptr<NetPlayerData>>& players) {
    std::cout<<"Starting to render in renderGames"<<std::endl;
    erase();
    Coordinates offset(0, 0);
    const int maxPlayersPerRow = 5; //todo move to a better place
    std::cout<<"players.size() : "<<players.size()<<std::endl;
    for (size_t i = 0; i < players.size(); ++i) {
        if (i > 0 && i % maxPlayersPerRow == 0) {
            offset.x = 0;
            offset.y += GRID_HEIGHT + 4; // Adjust offset for next row
        }


        renderGame(players[i], offset);
        //offset.x += GRID_WIDTH * 2 + 3; // Adjust offset for next game
        offset.x += (GRID_WIDTH + 1) * 2;
    }
    refresh();
}
/*
*OVERLOAD NOT TAKING SHARED_PTR
*/
void RenderEngineNcurses::renderGames(const vector<NetPlayerData>& players) {
    //std::cout<<"Starting to render in renderGames"<<std::endl;
    erase();
    Coordinates offset(0, 0);
    const int maxPlayersPerRow = 5; //todo move to a better place
    //std::cout<<"players.size() : "<<players.size()<<std::endl;
    for (size_t i = 0; i < players.size(); ++i) {
        if (i > 0 && i % maxPlayersPerRow == 0) {
            offset.x = 0;
            offset.y += GRID_HEIGHT + 4; // Adjust offset for next row
        }

        //std::cout<<"rendering..."<<std::endl;
        renderGame(players.at(i), offset);
        offset.x += GRID_WIDTH * 2 + 3; // Adjust offset for next game
    }
    refresh();
}

void RenderEngineNcurses::renderGame(const NetPlayerData& player, Coordinates offset) {
    mvprintw(offset.y, offset.x, "Score: %d", player.score);
    mvprintw(offset.y + 1, offset.x, "Lines: %d", player.linesCleared);
    mvprintw(offset.y + 2, offset.x, "+--------------------+");
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        mvprintw(y + 3 + offset.y, offset.x, "|");
        for (int x = 0; x < GRID_WIDTH; ++x) {
            Color blockColor = Color::nullcolor;

            int index = y * GRID_WIDTH + x; // Convert 2D coordinates to 1D index

            // Check if the index is within bounds
            if (index < static_cast<int>(player.grid.size())) {
                blockColor = player.grid[index];
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

    // Color the limits of the grid in white if playerID is -1
    if (player.playerId == -1) {
        highlightGridBorders(offset);
    }

    refresh();
}

/**
 *@brief Highlight the borders of the grid to disntinguish the client player in any mode.
 *
 *@param offset Coordinates of the grid that needs to be highlighted.
 */
void RenderEngineNcurses::highlightGridBorders(Coordinates offset) {
    int colorIndex = applyColor(Color::red); //todo init good color pairs to make thing prettier probably in constructor
    attron(COLOR_PAIR(colorIndex));
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        mvprintw(y + 3 + offset.y, offset.x, "|");
        mvprintw(y + 3 + offset.y, offset.x + GRID_WIDTH * 2 + 1, "|");
    }
    mvprintw(offset.y + 2, offset.x, "+--------------------+");
    mvprintw(offset.y + GRID_HEIGHT + 3, offset.x, "+--------------------+");
    attroff(COLOR_PAIR(colorIndex));
}



// // une mileur implimentation pour la deuxieme rederGame quand shared_ptr<NetPlayerData>
//void RenderEngineNcurses::renderGame(shared_ptr<NetPlayerData> player, Coordinates offset) {
//    if (player) {
//        renderGame(*player, offset);
//    }
//}

void RenderEngineNcurses::renderGame(shared_ptr<NetPlayerData> player, Coordinates offset) {
    std::cout<<"Starting to render 1 player game in renderGame"<<std::endl;
    mvprintw(offset.y, offset.x, "Score: %d", player->score);
    mvprintw(offset.y + 1, offset.x, "Lines: %d", player->linesCleared);
    mvprintw(offset.y + 2, offset.x, "+--------------------+");
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        mvprintw(y + 3 + offset.y, offset.x, "|");
        for (int x = 0; x < GRID_WIDTH; ++x) {
            Color blockColor = Color::nullcolor;

            int index = y * GRID_WIDTH + x;// Convert 2D coordinates to 1D index

            // Check if the index is within bounds
            if (index < static_cast<int>(player->grid.size())) {
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

void RenderEngineNcurses::renderGame(shared_ptr<NetPlayerData> player) {
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
     case Color::white:       pairIndex = 9; break;
     default:                 pairIndex = 0; break;
 }

 if (pairIndex != 0) {
     attron(COLOR_PAIR(pairIndex));
 }else{
    attroff(A_BOLD);
  }
 return pairIndex;
}


void RenderEngineNcurses::printGameOver() {
    //faut mieux calculer la place exact avec les resultat de getmaxyx(), pcq la 24 ne convient pas tt les terminals
    mvprintw(24, 0, "Game Over");
    refresh();
}
