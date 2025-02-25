#include "game.hpp"


const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;


Game::Game()
    : grid(std::make_shared<Grid>()),
      figureGenerator(),
      activeFigure(std::make_shared<MovingFigure>(std::make_shared<Figure>(figureGenerator.randomFigure()), grid)),
      movement(Action::NONE, activeFigure),
      gameData(0, 0, false, "Endless"),
      isRunning(false) {}

Game::~Game() {}

/**
 *@brief Gère la boucle principale du jeu.   
 */
void Game::run() {
    initscr();            // Initialize the window
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input characters
    nodelay(stdscr, TRUE); // Make getch non-blocking
    keypad(stdscr, TRUE); // Enable special keys to be captured

    isRunning = true;
    auto chrono = std::chrono::steady_clock::now();
    while (isRunning) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> currentTime = now - chrono;
        processInput();
        if (currentTime > std::chrono::milliseconds(500)) {
            updateGame();
            chrono = std::chrono::steady_clock::now();
        }
        renderGame();
        checkGameOver();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    endwin(); // End ncurses mode
}

void Game::stop() {
    isRunning = false;
}


/**
 * @brief gestion des actions en fonction de l'input du joueur.
 */
void Game::processInput() {
    // ici j'appelle Mouvment::setMouvment() avec l'action obtenue par Input::getAction() pour faire les deplacment,
    // et Mouvement, À son tour, Mouvement appelle MovingFigure::setPosition() pour mettre à jour
    // la position de activeFigure ou faire un rotation.
    
    // a confirmer par @hmz @farrukh

    int ch = getch();
    if (ch == ERR) {
        return;
    }
    while (getch() != ERR);  // Clear any leftover inputs

    Action action = Action::NONE;
    switch (ch) {
        case KEY_LEFT:
        case 'q':
            action = Action::MOVE_LEFT;
            break;

        case KEY_RIGHT:
        case 'd':
            action = Action::MOVE_RIGHT;
            break;

        case KEY_UP:
        case 'z':
            action = Action::ROTATE_LEFT;
            break;

        case 'r':
            action = Action::ROTATE_RIGHT;
            break;

        case ' ':
            action = Action::DROP;
            break;

        case KEY_DOWN:
        case 's':
            action = Action::SOFT_DROP;
            break;

        case 27: // ESC
            action = Action::ESCAPE;
            break;

        default:
            action = Action::NONE;
            break;
    }

    if (action != Action::NONE) {
        if (action == Action::ESCAPE) {
            stop();
            return;
        }
        //eventQueue.push(action);
        movement.setMouvement(action);

    }
    refresh();
}


/**
 * @brief Met à jour l'état du jeu à chaque itération de la boucle.
 * 
 * Tente de faire descendre la figure active. Si l'action échoue, la figure est placée,
 * les lignes complètes sont traitées, et une nouvelle figure est générée. 
 * Si la nouvelle figure ne peut pas apparaître, le jeu se termine.
 */
void Game::updateGame() {
    
    // Je tente de faire descendre la activeFigure en appelant Mouvement::setMouvement() avec l'action SOFT_DROP. 
    // si elle retourne false, ca veut dire MovingFigure::setPosition() consider cette action comme !allowAction.


    if (!movement.setMouvement(Action::SOFT_DROP)) {
        // si c'est le cas je place la figure.
        for (const auto& block : activeFigure->getBlocksRealPositions()) {
            grid->setBlock(block);
        }
        handleLineCompletion();
        activeFigure = std::make_shared<MovingFigure>(std::make_shared<Figure>(figureGenerator.randomFigure()), grid);
        movement = Mouvement(Action::NONE, activeFigure);

        // je vérifie si la nouvelle figure peut apparaître sur la grille.
        // si ce n'est pas le cas, ca signifie que le jeu est terminé.
        if (!activeFigure->spawnSucceeded()) {
            gameData.set_isGameOver(true);
            stop();
        }
    }
}


/**
 *@brief affiche le jeu dans son etat actuel.
 * Affichage de la grille de jeu, tetromino (block) , infos de jeu.
 */
void Game::renderGame() {
    erase();
    mvprintw(0, 0, "Score: %d", gameData.get_score());
    mvprintw(1, 0, "Lines: %d", gameData.get_numberBrokenLines());
    mvprintw(2, 0, "+--------------------+");

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        mvprintw(y + 3, 0, "|");
        for (int x = 0; x < GRID_WIDTH; ++x) {
            Color blockColor = activeFigure->getMovingFigureBlockColor(Coordinates(x, y));
            if (blockColor == Color::nullcolor) {
                blockColor = grid->getBlockColor(x, y);
            }
            if (blockColor != Color::nullcolor) {
                int index = applyColor(blockColor);
                mvprintw(y + 3, x * 2 + 1, "\u25A0");
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
int Game::applyColor(Color blockColor) {
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

/**
 *@brief arrête le jeu si il est "terminé".
 */
void Game::checkGameOver() {
    if (gameData.get_isGameOver()) {
        std::cout << "Game Over!\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        stop();
    }
}


/**
 *@brief gestion de la complétion des lignes.
 * vérification de chaque ligne de la grille pour savoir si elle est complète.
 * Si une ligne est complète, elle est supprimée et un effet de domino va agir sur les lignes situées au-dessus.
 * Ensuite mise à jour du score et du nombre de lignes cassées.
 */
void Game::handleLineCompletion() {
    int clearedLines = 0;
    int lineClearedScore = 100; // ligne complétée -> 100 points

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        if (grid->isFullLine(y)) {
            grid->clearLine(y);
            grid->applyDominoEffect(y);
            ++clearedLines;
            gameData.updateLines(1);
        }
    }

    if (clearedLines > 0) {
        gameData.updateScore(clearedLines * lineClearedScore);
    
    }
}

