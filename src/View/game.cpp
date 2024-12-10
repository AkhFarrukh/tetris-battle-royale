#include "./game.hpp"

Game::Game()
    : grid(std::make_shared<Grid>()),
      figureGenerator(),
      activeFigure(std::make_shared<MovingFigure>(figureGenerator.randomFigure(), grid)),
      movement(Action::NONE, activeFigure),
      gameData(0, 0, false, "Endless"),
      isRunning(false) {}

Game::~Game() {}

void Game::run() {
    isRunning = true;
    inputHandler.start();

    while (isRunning) {
        processInput();
        updateGame();
        renderGame();
        checkGameOver();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    inputHandler.stop();
}

void Game::stop() {
    isRunning = false;
}

void Game::processInput() {
    // ici j'appelle Mouvment::setMouvment() avec l'action obtenue par Input::getAction() pour faire les deplacment,
    // et Mouvement, À son tour, Mouvement appelle MovingFigure::setPosition() pour mettre à jour
    // la position de activeFigure ou faire un rotation.
    
    // a confirmer par @hmz @farrukh

    Action action = inputHandler.getAction();
    if (action != Action::NONE) {
        movement.setMouvement(action);
    }
}
void Game::updateGame() {
    //ici on met à jour l'état du jeu à chaque itération de la boucle en ligne:17

    // Je tente de faire descendre la activeFigure en appelant Mouvement::setMouvement() avec l'action SOFT_DROP. 
    // si elle retourne false, ca veut dire MovingFigure::setPosition() consider cette action comme !allowAction.
    if (!movement.setMouvement(Action::SOFT_DROP)) { 
        // si c'est le cas je place la figure.
        for (const auto& block : activeFigure->getFigBlocks()) {
            grid->setBlock(block);
        }
        handleLineCompletion();
        activeFigure = std::make_shared<MovingFigure>(figureGenerator.randomFigure(), grid);
        movement = Mouvement(Action::NONE, activeFigure);

        // je vérifie si la nouvelle figure peut apparaître sur la grille.
        // si ce n'est pas le cas, ca signifie que le jeu est terminé.
        if (!activeFigure->spawnSucceeded()) {
            gameData.set_isGameOver(true);
            stop();
        }
    }
}

void Game::renderGame() {
    std::cout << "Score: " << gameData.get_score() << "\n";
    std::cout << "Lines: " << gameData.get_numberBrokenLines() << "\n";

    //je peut faire un truc plus presentable apres, c'est juste un test version 
    for (int y = 0; y < 20; ++y) {
        for (int x = 0; x < 10; ++x) {
            if (grid->getBlockColor(x, y) == Color::nullcolor) {
                std::cout << ". ";
            } else {
                std::cout << "# ";
            }
        }
        std::cout << "\n";
    }
}

void Game::checkGameOver() {
    if (gameData.get_isGameOver()) {
        std::cout << "Game Over!\n";
        stop();
    }
}

void Game::handleLineCompletion() {
    int clearedLines = 0;
    for (int y = 0; y < 20; ++y) {
        if (grid->isFullLine(y)) {
            grid->clearLine(y);
            ++clearedLines;
        }
    }

    if (clearedLines > 0) {
        gameData.updateScore(clearedLines * 100); // jsp comment vous aller le faire, mais si on consider que chauqe ligne est 100 point
    }
}
