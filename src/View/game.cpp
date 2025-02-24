#include "game.hpp"

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
    isRunning = true;
    inputHandler.start();
    auto chrono = chrono::steady_clock::now();
    while (isRunning) {
        auto now = chrono::steady_clock::now();
        chrono::duration<double> currentTime = now-chrono;
        processInput();
        if (currentTime > chrono::milliseconds(500))
        {
            updateGame();
            chrono = chrono::steady_clock::now();
        }
        system("clear");
        renderGame();
        checkGameOver();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    inputHandler.stop();
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

    Action action = inputHandler.getAction();
    if (action == Action::ESCAPE) {
            stop();
            return;
        }

    if (action != Action::NONE) {
        movement.setMouvement(action);
    }
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
    std::cout << "Score: " << gameData.get_score() << endl << '\r';
    std::cout << "Lines: " << gameData.get_numberBrokenLines() << endl << '\r';

    std::cout << "\033[1;37m+" << std::string(20, '-') << "+\033[0m\n\r";
    for (int y = 0; y < 20; ++y) {
        std::cout << "\033[1;37m|\033[0m";
        for (int x = 0; x < 10; ++x) {
            if(activeFigure->getMovingFigureBlockColor(Coordinates(x, y)) != Color::nullcolor){
                Color blockColor = activeFigure->getMovingFigureBlockColor(Coordinates(x, y));
                std::cout << co(blockColor) << "  \033[0m";
            }
            else if (grid->getBlockColor(x, y) == Color::nullcolor) {
                std::cout << "  ";
            } 
            else {
                Color blockColor = grid->getBlockColor(x, y);
                std::cout << co(blockColor) << "  \033[0m";
            }
        }
        std::cout << "\033[1;37m|\033[0m"; 
        std::cout << endl << '\r';
    }
    std::cout << "\033[1;37m+" << std::string(20, '-') << "+\033[0m\n"; 
}



/**
 *@brief convertit une couleur (de block) en l'associant à un code couleur ANSI.
 *permet ainsi l'affichage correcte à l'écran des blocks. 
 *
 *@param blockColor couleur du block (et de type Color).
 *@return le code couleur associé.
 */
std::string Game::co(Color blockColor){
    std::string colorCode;
    switch (blockColor) {
        case Color::red:      colorCode = "\033[41m"; break;
        case Color::green:    colorCode = "\033[42m"; break;
        case Color::yellow:   colorCode = "\033[43m"; break;
        case Color::blue:     colorCode = "\033[44m"; break;
        case Color::orange:  colorCode = "\033[45m"; break;
        case Color::cyan:     colorCode = "\033[46m"; break;
        default:              colorCode = "\033[40m"; break;
    }
    return colorCode;
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

    for (int y = 0; y < 20; ++y) {
        if (grid->isFullLine(y)) {
            grid->clearLine(y);
            grid->applyDominoEffect(y);
            ++clearedLines;
            gameData.updateLines(1);
        }
    }

    if (clearedLines > 0) {
        gameData.updateScore(clearedLines * 100);
    
    }
}

