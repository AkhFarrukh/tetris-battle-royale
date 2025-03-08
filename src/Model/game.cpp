#include "game.hpp"
#include <utility>


const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;


Game::Game(shared_ptr<EventQueue> eventQueue, int uid)
    :   eventQueue(move(eventQueue)),
        grid(std::make_shared<Grid>()),
        figureGenerator(),
        activeFigure(std::make_shared<MovingFigure>(std::make_shared<Figure>(figureGenerator.randomFigure()), grid)),
        movement(Action::NONE, activeFigure),
        gameData(0, 0, false, "Endless"),
        isRunning(false),
        user_id(uid)
        // royalCompetition(2)
{}

Game::~Game() {}


int Game::getTimeDrop(){
  return timeDropEvent;
}

void Game::setTimeDrop(int newTime){
  timeDropEvent = newTime;
}
/**
 *@brief Gère la boucle principale du jeu.   
 */
void Game::run() {
    gameController->startGameInput();
    isRunning = true;
    auto chronoDropEvent = std::chrono::steady_clock::now();

    while (isRunning) {

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTimeDropEvent = now - chronoDropEvent;

        checkInput();

        if (elapsedTimeDropEvent > std::chrono::milliseconds(timeDropEvent)) { // malus/bonus vitesse
            //todo make variable interval of time(maybe corresponding to level)
            //todo check gamedata for level
            eventQueue->addEvent(make_shared<TimerDropEvent>());
            chronoDropEvent = std::chrono::steady_clock::now();
        }

        updateGame();
        gameController->renderGame();
        checkGameOver();

    }
    gameController->stopGameInput();
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

    shared_ptr<Event> event = eventQueue->getEvent(); // get the next event from the queue

    //next function take an event and try to cast it as an appopriate event subclass
    // if successful, it will treat the event
    handleMovementActionEvent(event); // handle keyboard imput events
    handleTimerDropEvent(event);       // handle timer events (soft drop figure and line completion check)

}


/**
 * @brief Handles movement action events.
 *
 * This function processes movement action events by casting the event to a
 * MovementActionEvent. If the event is successfully cast, it retrieves the
 * action data and performs the corresponding movement. If the action is
 * ESCAPE, it stops the game.
 *
 * @param event The event to be handled.
 */
void Game::handleMovementActionEvent(shared_ptr<Event> event) {
    shared_ptr<MovementActionEvent> movementActionEvent = dynamic_pointer_cast<MovementActionEvent>(event);
    if (movementActionEvent) {
        Action action = movementActionEvent->getEventData();
        if (action == Action::ESCAPE) {
            stop();
            return;
        }
        movement.setMouvement(action);
    }
}


/**
 * @brief Handles timer drop events.
 *
 * This function processes timer drop events by casting the event to a
 * TimerDropEvent. If the event is successfully cast, it attempts to move the
 * active figure down. If the movement fails, the figure is placed on the grid,
 * line completion is checked, and a new figure is generated. If the new figure
 * cannot spawn, the game ends.
 *
 * @param event The event to be handled.
 */
void Game::handleTimerDropEvent(shared_ptr<Event> event) {
    shared_ptr<TimerDropEvent> timerDropEvent = dynamic_pointer_cast<TimerDropEvent>(event);
    if (timerDropEvent) {
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
            else{
              countTetriminoGenerated +=1;
            }
        }
    }
}

int Game::getTetriminoCounter(){
  return countTetriminoGenerated;
}

void Game::setTetriminoCount(int& count){
  countTetriminoGenerated = count;
}

// void Game::stop() {
//     isRunning = false;
// }


void Game::stop() {
    if (!isRunning) return;
    isRunning = false;
    int finalScore = gameData.get_score();
    std::string gameMode = gameData.get_gameMode();

    bool won;
    if(gameMode == "Endless"){
        won = false;
    }else{
        won = !gameData.get_isGameOver();
    }
    int energyUsed;
    if (gameMode == "Royal"){
        energyUsed = royalCompetition.getEnergyUsed();
    } else{ 
        energyUsed = 0;
    }

    userDatabaseManager.updatePlayerStats(user_id, finalScore, won, energyUsed, gameMode);
}



void Game::checkInput() {
    gameController->getAction();
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


