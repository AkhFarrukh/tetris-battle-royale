//
// Created by farrukh on 27/02/25.
//

#include "RenderEngineNcurses.hpp"

#include <utility>


const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;

const char* maluses[] = {
    "1. Increase fall speed",
    "2. Reverse controls",
    "3. Freeze controls",
    "4. Blackout",
    "5. Thunder",
};

const char* bonuses[] = {
    "6. Bonus block 1x1",
    "7. Reduce fall speed",
};


#define GRID_RELATIVE_OFFSET_Y 3 //offset from the relative top of the grid to the first line of the grid
#define ENDGAME_MESSAGE_OFFSET_Y 5 //offset from the relative top of the grid to the endgame message
#define ENDGAME_DEFEAT_MESSAGE_STRING "GAME OVER"
#define ENDGAME_VICTORY_MESSAGE_STRING "You are the winner !"

#define BLOCK_SYMBOL "\u2588\u2588"
#define BLANK_SPACE_SYMBOL "  "
#define GIRD_HORIZONTAL_BORDER_DECOR_STRING "+--------------------+" // 2 * GRID_WIDTH + 2

#define GRID_VERTICAL_BORDER_DECOR_STRING "|"

#define SCORE_STRING "Score: %d"
#define LINES_STRING "Lines: %d"

#define MAX_PLAYERS_PER_ROW 5
#define HORIZONTAL_SPACE_BETWEEN_GRIDS 1

#define SIDE_MENU_HORIZONTAL_BORDER_DECOR_STRING "+---------------------------+"
#define MENU_MALUS_BONUS_TITLE "MENU MALUS / BONUS"
#define MENU_AVAiLABLE_MALUS_TITLE "Available Maluses:"
#define MENU_AVAiLABLE_BONUS_TITLE "Available Bonuses:"
#define MENU_SELECT_TARGET_PLAYER_TITLE "SELECT TARGET PLAYER "




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

void RenderEngineNcurses::drawTitleWithBorder(Coordinates offset, string title) {
    // Draw the title with a border
    mvprintw(offset.y, offset.x, SIDE_MENU_HORIZONTAL_BORDER_DECOR_STRING);
    mvprintw(offset.y + 1, offset.x, "| %-25s |", title.c_str());
}


/*
 * @brief Render malus menu for the client player in royal mode.
 */
void RenderEngineNcurses::drawMalusMenu(const NetPlayerData& playerClient, int numberOfPlayers) {
    // Calculate position for the right-side menu
    // Position it after the last game in the first row
    int menuX = (GRID_WIDTH * 2 + 2 + HORIZONTAL_SPACE_BETWEEN_GRIDS)
                * (numberOfPlayers < MAX_PLAYERS_PER_ROW ? numberOfPlayers : MAX_PLAYERS_PER_ROW);
    int menuY = 0;
    // Draw menu box
    drawTitleWithBorder(Coordinates(menuX, menuY), MENU_MALUS_BONUS_TITLE);
    mvprintw(menuY + 2, menuX, SIDE_MENU_HORIZONTAL_BORDER_DECOR_STRING);
    // Get energy from the first player for display (assuming all players have same energy in royal mode)
    mvprintw(menuY + 3, menuX, "| Energy: %-17d |", playerClient.nrg);
    drawTitleWithBorder(Coordinates(menuX, menuY + 4), MENU_AVAiLABLE_MALUS_TITLE);
    // List of available maluses with their corresponding number keys
    for (int i = 0; i < 5; i++) {
        mvprintw(menuY + 6 + i, menuX, "| %-25s |", maluses[i]);
    }
    drawTitleWithBorder(Coordinates(menuX, menuY + 11), MENU_AVAiLABLE_BONUS_TITLE);
    // List of available maluses with their corresponding number keys
    for (int i = 0; i < 2; i++) {
        mvprintw(menuY + 13 + i, menuX, "| %-25s |", bonuses[i]);
    }
    mvprintw(menuY + 15 , menuX, SIDE_MENU_HORIZONTAL_BORDER_DECOR_STRING);
}


/*
 * @brief Render target player menu for the client player in royal mode.
 */
void RenderEngineNcurses::drawPlayerSelectionMenu(const vector<NetPlayerData>& players, int numberOfPlayers) {
    // Calculate position for the right-side menu (same as malus menu)
    int menuX = (GRID_WIDTH * 2 + 2 + HORIZONTAL_SPACE_BETWEEN_GRIDS)
                   * (numberOfPlayers < MAX_PLAYERS_PER_ROW ? numberOfPlayers : MAX_PLAYERS_PER_ROW);
    int menuY = 0;

    // Draw menu box
    drawTitleWithBorder(Coordinates(menuX, menuY), MENU_SELECT_TARGET_PLAYER_TITLE);
    mvprintw(menuY + 2, menuX, SIDE_MENU_HORIZONTAL_BORDER_DECOR_STRING);

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
    mvprintw(menuY + 3 + menuItemCount, menuX, SIDE_MENU_HORIZONTAL_BORDER_DECOR_STRING);
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
        drawMalusMenu(playerClient, players.size()); // Draw the malus menu
    } else {
        drawPlayerSelectionMenu(players, players.size()); // Draw the player selection menu
    }
}




/*
*OVERLOAD NOT TAKING SHARED_PTR
*/
void RenderEngineNcurses::renderGames(const vector<NetPlayerData>& players) {
    erase();
    Coordinates offset(0, 0);
    for (size_t i = 0; i < players.size(); ++i) {
        if (i > 0 && i % MAX_PLAYERS_PER_ROW == 0) {
            offset.x = 0;
            offset.y += GRID_HEIGHT + 4; // Adjust offset for next row 2 for borders and 2 for the score and lines
        }
        renderGame(players.at(i), offset);
        offset.x += GRID_WIDTH * 2 + 2 + HORIZONTAL_SPACE_BETWEEN_GRIDS; // Adjust offset for next game
    }
    refresh();
}

void RenderEngineNcurses::renderGame(const NetPlayerData& player, Coordinates offset) {
    mvprintw(offset.y, offset.x, SCORE_STRING, player.score);
    mvprintw(offset.y + 1, offset.x, LINES_STRING, player.linesCleared);
    mvprintw(offset.y + 2, offset.x, GIRD_HORIZONTAL_BORDER_DECOR_STRING);
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        mvprintw(y + GRID_RELATIVE_OFFSET_Y + offset.y, offset.x, GRID_VERTICAL_BORDER_DECOR_STRING);
        for (int x = 0; x < GRID_WIDTH; ++x) {
            Color blockColor = Color::nullcolor;

            int index = y * GRID_WIDTH + x; // Convert 2D coordinates to 1D index

            // Check if the index is within bounds
            if (index < static_cast<int>(player.grid.size())) {
                blockColor = player.grid[index];
            }

            printBlock(blockColor, offset, Coordinates(x, y));
        }
        mvprintw(y + GRID_RELATIVE_OFFSET_Y + offset.y, offset.x + GRID_WIDTH * 2 + 1, GRID_VERTICAL_BORDER_DECOR_STRING);
    }
    mvprintw(offset.y + GRID_HEIGHT + GRID_RELATIVE_OFFSET_Y, offset.x, GIRD_HORIZONTAL_BORDER_DECOR_STRING);
    if(player.endGame || player.isGameOver){
        printEndgameMessage(player, offset);
    }
    // Color the limits of the grid in white if playerID is -1
    if (player.playerId == -1) {
        highlightGridBorders(offset);
    }
    refresh();
}


/**
 *@brief Print a block of the grid with the specified color and coordinates.
 *
 *@param blockColor The color of the block to be printed.
 *@param offset Coordinates of the grid where the block will be printed.
 *@param currentCoordinates The current coordinates of the block in the grid.
 */
void RenderEngineNcurses::printBlock(Color blockColor, Coordinates offset, Coordinates currentCoordinates) {
    if (blockColor != Color::nullcolor) {
        int colorIndex = applyColor(blockColor);
        mvprintw(currentCoordinates.y + GRID_RELATIVE_OFFSET_Y + offset.y, offset.x + currentCoordinates.x * 2 + 1, BLOCK_SYMBOL);
        attroff(COLOR_PAIR(colorIndex));
    } else {
        mvprintw(currentCoordinates.y + GRID_RELATIVE_OFFSET_Y + offset.y, offset.x + currentCoordinates.x * 2 + 1, BLANK_SPACE_SYMBOL);
    }
}


/**
 *@brief Print the game over or you win message for the player.
 *
 *@param player The player data containing game over status.
 *@param offset Coordinates of the grid where the message will be printed.
 */
void RenderEngineNcurses::printEndgameMessage(const NetPlayerData& player, Coordinates offset) {
    int colorIndex;
    if(player.isGameOver){
        colorIndex = applyColor(Color::red);
        attron(COLOR_PAIR(colorIndex));
        mvprintw(offset.y + GRID_HEIGHT + ENDGAME_MESSAGE_OFFSET_Y, offset.x, ENDGAME_DEFEAT_MESSAGE_STRING);}
    else {
        colorIndex = applyColor(Color::green);
        attron(COLOR_PAIR(colorIndex));
        mvprintw(offset.y + GRID_HEIGHT + ENDGAME_MESSAGE_OFFSET_Y, offset.x, ENDGAME_VICTORY_MESSAGE_STRING);}
    attroff(COLOR_PAIR(colorIndex));
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
        mvprintw(y + GRID_RELATIVE_OFFSET_Y + offset.y, offset.x, GRID_VERTICAL_BORDER_DECOR_STRING);
        mvprintw(y + GRID_RELATIVE_OFFSET_Y + offset.y, offset.x + GRID_WIDTH * 2 + 1, GRID_VERTICAL_BORDER_DECOR_STRING);
    }
    mvprintw(offset.y + 2, offset.x, GIRD_HORIZONTAL_BORDER_DECOR_STRING);
    mvprintw(offset.y + GRID_HEIGHT + GRID_RELATIVE_OFFSET_Y, offset.x, GIRD_HORIZONTAL_BORDER_DECOR_STRING);
    attroff(COLOR_PAIR(colorIndex));
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
    //todo in interfaceterminal decide how to handle colors properly
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







//-------------------unused method may be outdated--------------------------
void RenderEngineNcurses::printGameOver() {
    //faut mieux calculer la place exact avec les resultat de getmaxyx(), pcq la 24 ne convient pas tt les terminals
    mvprintw(24, 0, "Game Over");
    refresh();
}

//-------------------unused method may be outdated--------------------------
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


//-------------------unused method may be outdated--------------------------
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

//unused method may be outdated
void RenderEngineNcurses::renderGame(shared_ptr<NetPlayerData> player) {
    renderGame(player, Coordinates(0, 0));
}

