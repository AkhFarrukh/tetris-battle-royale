//
// Created by firasse on 02/03/25.
//

#include "gameModes.h"
#include "../Controller/GameController.hpp"
#include <unistd.h>

using namespace std;

/**
 * @brief Constructeur de la classe de base Mode.
 *
 * @param numPlayers Nombre de joueurs dans la partie.
 */
Mode::Mode(int numPlayers) : numPlayers_{numPlayers} {}
Mode::~Mode() = default;

Endless::Endless() : Mode(1) {} //Constructeur du mode Endless (1 joueur).
void Endless::setPlayers(int num) { //Mode Endless étant solo, cette fonction ne fait rien.
  cout <<"You can't add more player, the max is 1" << endl;
}
int Endless::getPlayers() { return 1; } //Retourne toujours 1 car ce mode est solo.

/**
 * @brief Applique les règles du mode Endless : simple comptage des points.
 *
 * @param grid Grille du joueur (inutile ici).
 * @param linesCleared Nombre de lignes complétées.
 */
void Endless::applyRules(Grid& grid, int linesCleared) {
    std::cout << "Endless mode: score updated based on " << linesCleared << " lines.\n";
}

Classic::Classic(int numPlayers) : Mode(numPlayers) {}


/**
 * @brief Définit le nombre de joueurs (entre 3 et 9).
 */
void Classic::setPlayers(int num) {
  if(3 <= num && num < 10){
      numPlayers_ = num;
  }
  else{
    perror("The player's number doesn't respect the rules (3 to 9).");
  }
}
int Classic::getPlayers() { return numPlayers_; }

/**
* @brief Applique les règles du mode Classic : envoi de malus en fonction du nombre de lignes complétées.
*
* @param grid Grille du joueur (utilisée pour gérer l'ajout de malus).
* @param linesCleared Nombre de lignes complétées en un coup.
*/
void Classic::applyRules(Grid& grid, int linesCleared) {
    // Déterminer combien de lignes de malus envoyer à un adversaire
    int malus = (linesCleared == 2) ? 1 : (linesCleared == 3) ? 2 : (linesCleared == 4) ? 4 : 0;
    if (malus > 0) {
        std::cout << "Classic mode: sending " << malus << " malus lines.\n";
        // Ajouter les lignes de malus à la grille adverse (à implémenter dans Game et une fonction est deja
        // présente dans grid.cpp)
    }
}

/**
 * @brief Constructeur du mode Duel (toujours 2 joueurs).
 */
Duel::Duel() : Classic(2) {} //Car correspond au mode classic mais c'est du 1 contre 1.

/**
 * @brief Constructeur du mode Royal Competition.
 *
 * @param numPlayers Nombre de joueurs dans la partie.
 */
RoyalCompetition::RoyalCompetition(int numPlayers) : Mode(numPlayers), energy(0) {}
void RoyalCompetition::setPlayers(int num) { numPlayers_ = num; }
int RoyalCompetition::getPlayers() { return numPlayers_; }

/**
 * @brief Applique les règles du mode Royal Competition :
 * - Accumulation d'énergie pour activer des malus/bonus.
 *
 * @param grid Grille du joueur.
 * @param linesCleared Nombre de lignes complétées.
 */
void RoyalCompetition::applyRules(Grid& grid, int linesCleared) {
    // Gagner de l'énergie en fonction des lignes complétées
    energy += linesCleared * 10;
    std::cout << "Royal Competition mode: energy gained = " << energy << "\n";
}

/**
 * @brief Inverse les commandes d'un adversaire. (malus)
 */

//
void RoyalCompetition::malusReverseControls(PlayerState& player){
  	player.countTetriminoGenerated = 1;
    player.inverted = true;
    while(player.countTetriminoGenerated <= 3){}
    player.inverted = false;
}
/**
 * @brief Bloque les commandes d'un adversaire. (malus)
 */
void RoyalCompetition::malusFreezeControls(PlayerState& player){
  	player.countTetriminoGenerated = 1;
    player.blocked = true;
    while(player.countTetriminoGenerated <= 1){}
    player.blocked = false;
}

/**
 * @brief Va ralentir notre prochaine chute. (bonus)
 */
void RoyalCompetition::bonusReduceFallSpeed(PlayerState& player){
  player.countTetriminoGenerated = 1;
  player.slowed = true;
  while(player.countTetriminoGenerated <= 1){}
  player.slowed = false;
}

/**
 * @brief Va accelerer la chute chez l'adversaire séléctionné. (malus)
 */
void RoyalCompetition::malusIncreaseFallSpeed(PlayerState& player){
  player.countTetriminoGenerated = 1;
  player.sped_up = true;
  while(player.countTetriminoGenerated <= 1){}
  player.sped_up = false;

}
/**
 * @brief Va faire tomber la foudre sur un adversaire séléctionner, ce qui aura
 * pour effet de detruire une zone 2X2 de block.(Malus)
 */
void RoyalCompetition::malusThunder(PlayerState& player) {

  int GRID_HEIGHT = 20;
  int GRID_WIDTH = 10;
  srand(time(nullptr));
  int columnTargeted = rand() % GRID_WIDTH; //Va choisir une colonne ou la foudre va s'abattre.
  int thunderPositionLine = 0; //Commence tout en haut.
  while (player.grid->isLocationFree({columnTargeted, thunderPositionLine})) { //Fait chuter la foudre(couleur blanche) et stop au lieu d'impact
    if (thunderPositionLine + 1 == GRID_HEIGHT) {
      break;
    }
    thunderPositionLine += 1;
  }
  player.slowed = true;
  for (int i = 0; i <100; i++) {
    player.grid->lightningBlock2X2(Coordinates(columnTargeted, thunderPositionLine));
    player.grid->deleteBlock2X2(Coordinates(columnTargeted, thunderPositionLine));
  }
  player.slowed = false;

}

/**
 * @brief Va aveugler l'adversaire pendant 5 secondes.(Malus)
 */
void RoyalCompetition::malusBlackOut(PlayerState& player) {
  player.blackout = 10;
}

/**
 * @brief Les 2 prochains Tetrimino seront des cubes 1X1.(bonus)
 */
void RoyalCompetition::bonusBlockCube(PlayerState& player) {
  player.countTetriminoGenerated = 1;
  player.isBonusCube = true;
  while(player.countTetriminoGenerated <= 2){
  }
  player.isBonusCube = false;
}