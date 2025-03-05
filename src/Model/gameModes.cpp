//
// Created by firasse on 02/03/25.
//

#include "gameModes.h"
#include "../Controller/GameController.hpp"
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

//todo poser un compteur pour la durée des Bonus/malus (exemple Inversion des commandes lors des 3 prochain blocs)
void RoyalCompetition::malusReverseControls(GameController controller, bool isReversed){
  	controller.setControlsInverted(isReversed);
}

void RoyalCompetition::malusFreezeControls(GameController controller, bool isBlocked){
  	controller.setControlsBlocked(isBlocked);
}

void RoyalCompetition::bonusReduceFallSpeed(Game game){
  game.setTimeDrop(1000);
}

void RoyalCompetition::malusIncreaseFallSpeed(Game game){
  game.setTimeDrop(250);
}