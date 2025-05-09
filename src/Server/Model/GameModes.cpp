//
// Created by firasse on 02/03/25.
//

#include "GameModes.h"
#include "GameController.hpp"
#include <unistd.h>

using namespace std;

/**
 * @brief Constructeur de la classe de base Mode.
 *
 * @param numPlayers Nombre de joueurs dans la partie.
 */
Mode::Mode() = default;
Mode::~Mode() = default;

Endless::Endless() = default; //Constructeur du mode Endless (1 joueur).




Classic::Classic() = default;




/**
* @brief Applique les règles du mode Classic : envoi de malus en fonction du nombre de lignes complétées.
*
* @param grid Grille du joueur (utilisée pour gérer l'ajout de malus).
* @param linesCleared Nombre de lignes complétées en un coup.
*/
void Classic::applyRules(PlayerState& player,  PlayerState& targetedPlayer) {
    // Déterminer combien de lignes de malus envoyer à un adversaire
    int malus = (player.completedLines == 2) ? 1 : (player.completedLines == 3) ? 2 : (player.completedLines == 4) ? 4 : 0;
    if (malus > 0) {
      targetedPlayer.grid->malusLines(malus);
    }
    player.completedLines = player.completedLines % 5; //Remise du compteur à 0
}

/**
 * @brief Constructeur du mode Duel (toujours 2 joueurs).
 */
Duel::Duel() = default; //Car correspond au mode classic mais c'est du 1 contre 1.

void Duel::applyRules(PlayerState &player, PlayerState& targetedPlayer) {
  // Déterminer combien de lignes de malus envoyer à un adversaire
  int malus = (player.completedLines == 2) ? 1 : (player.completedLines == 3) ? 2 : (player.completedLines == 4) ? 4 : 0;
  if (malus > 0) {
    targetedPlayer.grid->malusLines(malus);
  }
  player.completedLines = player.completedLines % 5; //Remise du compteur à 0
}

/**
 * @brief Constructeur du mode Royal Competition.
 *
 */
RoyalCompetition::RoyalCompetition() = default;


/**
 * @brief Applique les règles du mode Royal Competition :
 * - Accumulation d'énergie pour activer des malus/bonus.
 *
 * @param grid Grille du joueur.
 * @param linesCleared Nombre de lignes complétées.
 *

*/

/**
 * @brief Inverse les commandes d'un adversaire. (malus)
 */

//
void RoyalCompetition::malusReverseControls(PlayerState& player) {
  player.countTetriminoGenerated = 1;
  player.inverted = true;
}

/**
 * @brief Bloque les commandes d'un adversaire. (malus)
 */
void RoyalCompetition::malusFreezeControls(PlayerState& player){
  	player.countTetriminoGenerated = 1;
    player.blocked = true;
}

/**
 * @brief Va ralentir notre prochaine chute. (bonus)
 */
void RoyalCompetition::bonusReduceFallSpeed(PlayerState& player){
  player.countTetriminoGenerated = 1;
  player.slowed = true;
}

/**
 * @brief Va accelerer la chute chez l'adversaire séléctionné. (malus)
 */
void RoyalCompetition::malusIncreaseFallSpeed(PlayerState& player){
  player.countTetriminoGenerated = 1;
  player.sped_up = true;
}
/**
 * @brief Va faire tomber la foudre sur un adversaire séléctionner, ce qui aura
 * pour effet de detruire une zone 2X2 de block.(Malus)
 */
void RoyalCompetition::malusThunder(PlayerState& player) {
  cout << "malusThunder" << endl;
  int GRID_HEIGHT = 20;
  int GRID_WIDTH = 10;
  srand(static_cast<unsigned int>(time(nullptr)));
  int columnTargeted = rand() % GRID_WIDTH; //Va choisir une colonne ou la foudre va s'abattre.
  int thunderPositionLine = 0; //Commence tout en haut.
  while (player.grid->isLocationFree({columnTargeted, thunderPositionLine})) { //Fait chuter la foudre(couleur blanche) et stop au lieu d'impact
    if (thunderPositionLine + 1 == GRID_HEIGHT) {
      break;
    }
    thunderPositionLine += 1;
  }


  player.grid->lightningBlock2X2(Coordinates(columnTargeted, thunderPositionLine));
  //todo fonctionne mais une animation serait bien ici
  player.grid->deleteBlock2X2(Coordinates(columnTargeted, thunderPositionLine));


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
  cout <<"in bonus cube"<< endl;
  player.countTetriminoGenerated = 1;
  player.isBonusCube = true;
}

//todo ajouter les différents malus et bonus propre au mode Royal competition