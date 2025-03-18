#include "game.hpp"
#include "gameModes.h"
#include <utility>


const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;


// arbitary prices for the malus/bonus. Very unbalanced
#define INCREASE_FALL_SPEED_PRICE   1
#define REVERSE_CONTROLS_PRICE      1
#define FREEZE_CONTROLS_PRICE       1
#define THUNDER_PRICE               1
#define BLACK_OUT_PRICE             1
#define REDUCE_FALL_SPEED_PRICE     1
#define BLOCK_1X1_PRICE             1

Game::Game(){
    gameMode = Modes::NONE_;
    players = vector<PlayerState> ();
    figureGenerator = FigureGenerator();
    isRunning = false;
    mode = make_shared<Mode>();
    numPlayers_ = 0;
}
Game::Game(std::vector<ThreadGameData> playersData, int numPlayers, Modes gameMode)
    :   isRunning(false),
        numPlayers_(numPlayers),
        gameMode(gameMode)
{
    cout<< "Starting Game constructor"<<endl;
    switch (gameMode)
    {
    case Modes::Endless_:
        mode = std::make_shared<Endless>();
        break;
    case Modes::Classic_:
        mode = std::make_shared<Classic>();
        break;
    case Modes::Duel_:
        mode = std::make_shared<Duel>();   
        break;
    case Modes::Royal_:
        mode = std::make_shared<RoyalCompetition>();
        break;

    default:
        throw std::invalid_argument("Unknown game mode: " + gameMode);
        break;
    }
    for (int i = 0; i < numPlayers; i++) {
        PlayerState ps(i, gameMode);

        ps.grid = std::make_shared<Grid>();
        ps.eventQueue = playersData[i].eventQueue;
        ps.fd = playersData[i].client_fd;
        ps.malusBonus = MalusBonus::MalusNONE;

        auto fig = std::make_shared<Figure>(figureGenerator.randomFigure(ps.isBonusCube)); //todo bonus 1X1
        auto movingFig = std::make_shared<MovingFigure>(fig, ps.grid);
        ps.activeFigure = movingFig;

        ps.movement = Mouvement(Action::NONE, movingFig);

        ps.nextTetromino = std::make_shared<Figure>(figureGenerator.randomFigure(ps.isBonusCube));

        ps.gameData = GameData(0, 0, false, gameMode);

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
        cout<<"Starting game loop"<<endl;
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTimeDropEvent = now - chronoDropEvent;
        int time = 500;
        cout<<"Checking timing for players"<<endl;
        for (int p = 0; p < (int)players.size(); p++) {
            if(players[p].sped_up){time = 250;}
            else if(players[p].slowed){time = 1000;}
        }
        cout<<"Before chrono"<<endl;
        if (elapsedTimeDropEvent > std::chrono::milliseconds(time)) {
            cout<<"chrono"<<endl;
            for (int p = 0; p < (int)players.size(); p++) {
                    if (players[p].blackout != 0){players[p].blackout--;} //Il y a 10 unités lors d'un BO, cela durera 5 sec
                    bool canDrop = players[p].movement.setMouvement(Action::SOFT_DROP);
                    cout<<"soft dropping"<<endl;
                    if (!canDrop) {
                        placePieceOnBoard(p);
                    }
            }
            chronoDropEvent = now;
        }

        updateGame();
        cout<<"Updated"<<endl;
        if(broadcastGame() == false)
        {
            perror("Error while broadcasting the game : ");
            isRunning = false;
        }
        cout<<"Broadcasted"<<endl;
        checkGameOver();
        cout<<"CheckGameOver"<<endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
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
            if (!evt) break;
            if (std::dynamic_pointer_cast<MalusBonusEvent>(evt))
                handleMalusBonusEvent(evt, p);
            else {
                if (players[p].malusBonus != MalusBonus::BLOCKED){
                    handleMovementActionEvent(p, evt);
               }
            }
        }
    }
}

Action reverse(Action action) {
    switch (action) {
        case Action::MOVE_RIGHT:
            return Action::MOVE_LEFT;
        case Action::MOVE_LEFT:
            return Action::MOVE_RIGHT;
        case Action::ROTATE_LEFT:
            return Action::ROTATE_RIGHT;
        case Action::ROTATE_RIGHT:
            return Action::ROTATE_LEFT;
        case Action::SOFT_DROP:
            return Action::DROP;  //todo pas sur de cette inversion
        case Action::DROP:
            return Action::SOFT_DROP;
        default:
            return Action::NONE;
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
        players[p].isGameOver = true;
        players[p].isInGame = false;
        int type = MessageType::T_Exit;
        int size = sizeof(MessageType);
        sendAll(players[p].fd, &type, sizeof(int));
        sendAll(players[p].fd, &size, sizeof(int));
        checkGameOver();
        return;
    }
    if (players[p].inverted) {//if malusInverted (RC mode)
        a = reverse(a);
    }
    bool moved = players[p].movement.setMouvement(a);
    if (!moved && a == Action::SOFT_DROP) {
        placePieceOnBoard(p);
    }
}

/**
 * @brief Handles malus/bonus events.
 *This function is made for the mode RoyalCompetition only.
 *It takes the playerID and apply the eventMalusBonus to the targetted player.
 *
 * @param event The event to be handled.
 * @param playerHimself the id of the player for the Bonus.
 */



void Game::handleMalusBonusEvent(std::shared_ptr<Event> e, int playerHimself) {
    auto moveEvt = std::dynamic_pointer_cast<MalusBonusEvent>(e);
    if (!moveEvt) return;

    MalusBonus bm = moveEvt->getMalusBonus();
    int energy = players[playerHimself].energy; //energy of player that activates malus/bonus
    // Vérifier si le mode actuel est RoyalCompetition
    auto royalMode = std::dynamic_pointer_cast<RoyalCompetition>(mode);
    if (!royalMode) {
        std::cerr << "Error: MalusBonus event triggered, but game mode is not RoyalCompetition.\n";
        return;
    }
    int target = moveEvt->getTarget();//Le joueur ciblé par les malus
    // Appliquer le bon malus en fonction du type
    switch (bm) {
        case MalusBonus::MalusIncreaseFallSpeed:
            if (energy >= INCREASE_FALL_SPEED_PRICE) {
                players[playerHimself].energy -= INCREASE_FALL_SPEED_PRICE;
                royalMode->malusIncreaseFallSpeed(players[target]);
            }
        break;
        case MalusBonus::MalusReverseControls:
            if (energy >= REVERSE_CONTROLS_PRICE) {
                players[playerHimself].energy -= REVERSE_CONTROLS_PRICE;
                royalMode->malusReverseControls(players[target]);
            }
        break;
        case MalusBonus::MalusFreezeControls:
            if (energy >= FREEZE_CONTROLS_PRICE) {
                players[playerHimself].energy -= FREEZE_CONTROLS_PRICE;
                royalMode->malusFreezeControls(players[target]);
            }
        break;
        case MalusBonus::MalusThunder:
            if (energy >= THUNDER_PRICE) {
                players[playerHimself].energy -= THUNDER_PRICE;
                royalMode->malusThunder(players[target]);
            }
        break;
        case MalusBonus::MalusBlackOut:
            if (energy >= BLACK_OUT_PRICE) {
                players[playerHimself].energy -= BLACK_OUT_PRICE;
                royalMode->malusBlackOut(players[target]);
            }
        break;
        case MalusBonus::BonusReduceFallSpeed:
            if (energy >= REDUCE_FALL_SPEED_PRICE) {
                players[playerHimself].energy -= REDUCE_FALL_SPEED_PRICE;
                royalMode->bonusReduceFallSpeed(players[playerHimself]);
            }
        break;
        case MalusBonus::Bonus_Block1X1:
            if (energy >= BLOCK_1X1_PRICE) {
                players[playerHimself].energy -= BLOCK_1X1_PRICE;
                royalMode->bonusBlockCube(players[playerHimself]);
            }
        break;
        default:
            std::cerr << "Unknown MalusBonus event.\n";
        break;
    }
}
        //if bonus
        //players[p].slowed = true;``

        //if malus
        //int target = moveEvt->getTarget();
        //players[target].blocked = true;


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

    auto newFig = make_shared<Figure>(figureGenerator.randomFigure(ps.isBonusCube));
    ps.activeFigure = make_shared<MovingFigure>(newFig, ps.grid);
    ps.movement = Mouvement(Action::NONE, ps.activeFigure);

    if (!ps.activeFigure->spawnSucceeded()) {
        ps.isGameOver = true;
        ps.gameData.set_isGameOver(true);
    }
    else {
        ps.countTetriminoGenerated +=1;
        if (ps.countTetriminoGenerated > 1 && ps.slowed) {
            ps.slowed = false;
        }
        if (ps.countTetriminoGenerated > 1 && ps.sped_up) {
            ps.sped_up = false;
        }
        if (ps.countTetriminoGenerated > 1 && ps.blocked) {
            ps.blocked = false;
        }
        if (ps.countTetriminoGenerated > 3 && ps.inverted) {
            ps.inverted = false;
        }
        if (ps.countTetriminoGenerated > 2 && ps.isBonusCube) {
            ps.isBonusCube = false;
        }
    }
}

// void Game::stop() {
//     isRunning = false;
// }


void Game::stop() {
    for (auto &ps : players) {
        if (ps.isInGame) {
            int type = MessageType::T_Exit;
            int size = sizeof(MessageType);
            sendAll(ps.fd, &type, sizeof(int));
            sendAll(ps.fd, &size, sizeof(int));
        }
    }
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
    for (const auto& ps : players) {
        if (!ps.isGameOver) aliveCount++;
    }
    if (gameMode == Modes::Duel_) {
        if (aliveCount <= 1) {
            std::cout << "Duel ended" << std::endl;
            stop();
        }
    } else if (aliveCount < 1) {
        std::cout << "Game over: All players lost" << std::endl;
        stop();
    }
}


void Game::handleMalus(int targetID, int playerID) {
    if (gameMode == Modes::Duel_) {
        auto selectedMode = dynamic_pointer_cast<Duel>(mode);
        selectedMode->applyRules(players[playerID], players[targetID]);
    }
    else if (gameMode == Modes::Classic_) {
        auto selectedMode = dynamic_pointer_cast<Classic>(mode);
        selectedMode->applyRules(players[playerID], players[targetID]);
    }
    else {
        std::cerr << "Error: Malus triggered, but game mode is not Duel or Classic.\n";
        return;
    }

}

int findTarget(int playerId, int numPlayers) {
    if (numPlayers <= 1) {
        throw invalid_argument("Not enough players to find a target.");
    }

    int target;
    do {
        target = rand() % numPlayers;  // Génère un nombre entre 0 et numPlayers - 1
    } while (target == playerId);      // Continue tant que la cible est le joueur lui-même

    return target;
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
            ps.completedLines +=1; //Duel et Classic
            ps.energy += 5; //RC mode
            if (gameMode == Modes::Duel_ || gameMode == Modes::Classic_) { // S'occupe des malus automatiques
                int target = findTarget(p, numPlayers_);
                handleMalus(target, p);

            }
        }
    }

    if (cleared > 0) {
        ps.gameData.updateScore(cleared * 100);
        ps.energy += cleared; // add number of cleared lines to energy
        //malus
        // int targetId = pickTargetPlayer(p);
        // addGarbageLines(targetId, cleared);
    }
}


/*
Color Game::getBlockColor(Coordinates cell) {
    Color blockColor = activeFigure->getMovingFigureBlockColor(cell);
    if (blockColor == Color::nullcolor) {
        blockColor = grid->getBlockColor(cell.x, cell.y);
    }
    if (isGridHidden) {
        return Color::white; //Afin que le joueur soit aveuglé (malus du mode Royal Competition)
    }
    return blockColor;
}
*/



bool Game::broadcastGame() {
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
                Color c;
                if (ps.blackout == 0) {//Le temps d'aveuglement est nul
                    c = ps.grid->getBlockColor(col, row);
                }
                else { //On est aveuglé
                    c = Color::white; //Blackout malus (RC mode)
                }
                nd.grid[row * GRID_WIDTH + col] = c;
            }
        }
        if (!ps.isGameOver) {
            for (auto& figureBlock : ps.activeFigure->getBlocksRealPositions()) {
                nd.grid[figureBlock.getY() * GRID_WIDTH + figureBlock.getX()] = figureBlock.getColour();
            }
        }
        
        netArray.push_back(std::move(nd));
    }
    //Envoie toutes les infos nécessaire à l'affichage à chaque joueur
    for (auto &ps : players) {
        int sock = ps.fd; 
        if (sock <= 0) continue;

        int type = MessageType::T_PlayerData;
        if (!sendAll(sock, &type, sizeof(type))){return false;}

        int size = static_cast<int>(netArray.size());
        if (!sendAll(sock, &size, sizeof(size))){return false;}
        for (auto& player : netArray) {
            if (!sendAll(sock, &player.playerId, sizeof(player.playerId))) {return false;}
            if (!sendAll(sock, &player.isGameOver, sizeof(player.isGameOver))) {return false;}
            if (!sendAll(sock, &player.linesCleared, sizeof(player.linesCleared))) {return false;}
            if (!sendAll(sock, &player.score, sizeof(player.score))) {return false;}
        
            int gridSize = static_cast<int>(player.grid.size());
            if (!sendAll(sock, &gridSize, sizeof(gridSize))) {return false;}
            if (!sendAll(sock, player.grid.data(), gridSize * sizeof(Color))) {return false;}
        }
        //cout << "Sent netplayerdatda to " << sock << endl;
    }
    return true;
}

