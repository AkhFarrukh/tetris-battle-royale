#include "game.hpp"

#include <utility>


const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;




Game::Game(std::vector<ThreadGameData> playersData, int numPlayers, string mode)
    :   isRunning(false),
        gameMode(mode)

{
    cout<< "Starting Game constructor"<<endl;
    for (int i = 0; i < numPlayers; i++) {
        PlayerState ps(i);

        ps.grid = std::make_shared<Grid>();
        ps.eventQueue = playersData[i].eventQueue;
        ps.fd = playersData[i].client_fd;

        auto fig = std::make_shared<Figure>(figureGenerator.randomFigure());
        auto movingFig = std::make_shared<MovingFigure>(fig, ps.grid);
        ps.activeFigure = movingFig;

        ps.movement = Mouvement(Action::NONE, movingFig);

        ps.nextTetromino = std::make_shared<Figure>(figureGenerator.randomFigure());

        ps.gameData = GameData(0, 0, false, "Endless mode");

        players.push_back(ps);
    }
    cout<<"Ending Game constructor"<<endl;
}

Game::~Game() {}

/**
 *@brief Gère la boucle principale du jeu.   
 */
void Game::run() {
    isRunning = true;
    auto chronoDropEvent = std::chrono::steady_clock::now();

    while (isRunning) {

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTimeDropEvent = now - chronoDropEvent;

        if (elapsedTimeDropEvent > std::chrono::milliseconds(500)) {
            for (int p = 0; p < (int)players.size(); p++) {
                if (!players[p].isGameOver) {
                    bool canDrop = players[p].movement.setMouvement(Action::SOFT_DROP);
                    if (!canDrop) {
                        placePieceOnBoard(p);
                    }
                }
            }
            chronoDropEvent = now;
        }

        updateGame();
        broadcastGame();
        checkGameOver();
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

    for (int p = 0; p < (int)players.size(); p++) {
        if (players[p].isGameOver) continue;
        while (true) {
            auto evt = players[p].eventQueue->getEvent();
            if (!evt) break; // no more events
            handleMovementActionEvent(p, evt);
        }
    }
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
void Game::handleMovementActionEvent(int p, std::shared_ptr<Event> e) {
    auto moveEvt = std::dynamic_pointer_cast<MovementActionEvent>(e);
    if (!moveEvt) return;

    Action a = moveEvt->getEventData();
    if (a == Action::ESCAPE) {
        stop();
        return;
    }
    bool moved = players[p].movement.setMouvement(a);
    if (!moved && a == Action::SOFT_DROP) {
        placePieceOnBoard(p);
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


void Game::handleTimerDropEvent(std::shared_ptr<Event> event) {
    auto timerEvt = std::dynamic_pointer_cast<TimerDropEvent>(event);
    if (!timerEvt) {
        return;
    }

    for (int p = 0; p < (int)players.size(); p++) {
        if (!players[p].isGameOver) {
            bool canDrop = players[p].movement.setMouvement(Action::SOFT_DROP);
            if (!canDrop) {
                placePieceOnBoard(p);
            }
        }
    }
}
 */

void Game::placePieceOnBoard(int p) {
    auto &ps = players[p];

    for (auto &block : ps.activeFigure->getBlocksRealPositions()) {
        ps.grid->setBlock(block);
    }

    handleLineCompletion(p);

    auto newFig = make_shared<Figure>(figureGenerator.randomFigure());
    ps.activeFigure = make_shared<MovingFigure>(newFig, ps.grid);
    ps.movement = Mouvement(Action::NONE, ps.activeFigure);

    if (!ps.activeFigure->spawnSucceeded()) {
        ps.isGameOver = true;
        ps.gameData.set_isGameOver(true);
    }
}



void Game::stop() {
    isRunning = false;
}

/*
void Game::checkInput() {
    gameController->getAction();
}
*/


/**
 *@brief arrête le jeu si il est "terminé".
 */
void Game::checkGameOver() {
    int aliveCount = 0;
    for (auto &ps : players) {
        if (!ps.isGameOver) {
            aliveCount++;
        }
    }
    if (aliveCount < 1) {
        stop();
    }
}


/**
 *@brief gestion de la complétion des lignes.
 * vérification de chaque ligne de la grille pour savoir si elle est complète.
 * Si une ligne est complète, elle est supprimée et un effet de domino va agir sur les lignes situées au-dessus.
 * Ensuite mise à jour du score et du nombre de lignes cassées.
 */
void Game::handleLineCompletion(int p) {
    auto &ps = players[p];

    int cleared = 0;
    const int GRID_HEIGHT = 20; // adapt if needed
    for (int y = 0; y < GRID_HEIGHT; y++) {
        if (ps.grid->isFullLine(y)) {
            ps.grid->clearLine(y);
            ps.grid->applyDominoEffect(y);
            cleared++;
            ps.gameData.updateLines(1);
        }
    }

    if (cleared > 0) {
        ps.gameData.updateScore(cleared * 100);

        //malus
        // int targetId = pickTargetPlayer(p);
        // addGarbageLines(targetId, cleared);
    }
}

void Game::setGameController(shared_ptr<GameController> controller) {
    gameController = move(controller);
}
/*
Color Game::getBlockColor(Coordinates cell) {
    Color blockColor = activeFigure->getMovingFigureBlockColor(cell);
    if (blockColor == Color::nullcolor) {
        blockColor = grid->getBlockColor(cell.x, cell.y);
    }
    return blockColor;
}
*/


void Game::broadcastGame() {
    std::vector<NetPlayerData> netArray;

    for (auto &ps : players) {
        NetPlayerData nd;
        nd.playerId    = ps.playerId;
        nd.isGameOver  = ps.isGameOver;
        nd.linesCleared= ps.gameData.get_numberBrokenLines();
        nd.score       = ps.gameData.get_score();

        int w = 10;
        int h = 20;
        nd.grid.resize(w * h);

        //4 blocks 
        
        for (int row = 0; row < h; row++) {
            for (int col = 0; col < w; col++) {
                Color c = ps.grid->getBlockColor(col, row);

                if (!ps.isGameOver) {
                    for (auto& figureBlock : ps.activeFigure->getBlocksRealPositions()) {
                      if (figureBlock.getX() == col && figureBlock.getY() == row) {
                        c = ps.activeFigure->getMovingFigureBlockColor(Coordinates(figureBlock.getX(), figureBlock.getY()));
                        break;
                      }
                    }
                  }
              
                  nd.grid[row * GRID_WIDTH + col] = c;
            }
        }
        netArray.push_back(std::move(nd));
    }

    for (auto &ps : players) {
        int sock = ps.fd; 
        if (sock <= 0) continue;

        int type = MessageType::T_PlayerData;
        if (!sendAll(sock, &type, sizeof(type))) {
            continue;
        }

        int size = netArray.size();
        if (!sendAll(sock, &size, sizeof(size))) {
            continue;
        }
        cout << "netArray.size() : " << netArray.size()<<endl;
        for (auto& player : netArray) {
            sendAll(sock, &player.playerId, sizeof(player.playerId));
            sendAll(sock, &player.isGameOver, sizeof(player.isGameOver));
            sendAll(sock, &player.linesCleared, sizeof(player.linesCleared));
            sendAll(sock, &player.score, sizeof(player.score));
        
            int gridSize = player.grid.size();
            sendAll(sock, &gridSize, sizeof(gridSize));
            if (gridSize > 0)
                sendAll(sock, player.grid.data(), gridSize * sizeof(Color));
        }
        //size_t vectorNPDsize = static_cast<size_t>(sizeof(NetPlayerData) * netArray.size());
        //if (!sendAll(sock, &vectorNPDsize, sizeof(vectorNPDsize))) {
        //    continue;
        //}
        //int size = netArray.size();
        //if (!sendAll(sock, &size, sizeof(size))) {
        //    continue;
        //}
        //cout << "netArray.size() : " << netArray.size()<<endl;
        //if (!sendAll(sock, netArray.data(), vectorNPDsize)) {
        //    break;
        //}
    }
}

