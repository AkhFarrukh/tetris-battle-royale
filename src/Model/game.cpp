#include "game.hpp"

#include <utility>


const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;


Game::Game(shared_ptr<EventQueue> eventQueue)
    :   eventQueue(move(eventQueue)),
        grid(std::make_shared<Grid>()),
        figureGenerator(),
        activeFigure(std::make_shared<MovingFigure>(std::make_shared<Figure>(figureGenerator.randomFigure()), grid)),
        movement(Action::NONE, activeFigure),
        gameData(0, 0, false, "Endless"),
        isRunning(false)
{}

Game::~Game() {}

/**
 *@brief Gère la boucle principale du jeu.   
 */
void Game::run() {
    gameController->startGameInput();
    isRunning = true;
    auto chrono = std::chrono::steady_clock::now();
    while (isRunning) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> currentTime = now - chrono;
        gameController->getAction();
        shared_ptr<MovementActionEvent> movementActionEvent = dynamic_pointer_cast<MovementActionEvent>(eventQueue->getEvent());
        if (movementActionEvent) {
                    Action action = movementActionEvent->getEventData();
        if (action == Action::ESCAPE) {
            stop();
            return;
        }
        movement.setMouvement(action);
        if (currentTime > std::chrono::milliseconds(500)) {
            //todo make timer an event and update every tick (or frame)
            updateGame();
            chrono = std::chrono::steady_clock::now();
        }
        gameController->renderGame();
        checkGameOver();
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

    }
    gameController->stopGameInput();
}

void Game::stop() {
    isRunning = false;
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
 *@brief arrête le jeu si il est "terminé".
 */
void Game::checkGameOver() {
    if (gameData.get_isGameOver()) {
        gameController->printGameOver();
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

void Game::setGameController(shared_ptr<GameController> controller) {
    gameController = move(controller);
}

Color Game::getBlockColor(Coordinates cell) {
    Color blockColor = activeFigure->getMovingFigureBlockColor(cell);
    if (blockColor == Color::nullcolor) {
        blockColor = grid->getBlockColor(cell.x, cell.y);
    }
    return blockColor;
}

