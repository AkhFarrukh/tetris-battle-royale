//
// Created by firasse on 02/03/25.
//

#ifndef GAMEMODES_H
#define GAMEMODES_H

#include <iostream>
#include "Grid.hpp"
#include "GameController.hpp"

/**
 * @brief Classe de base abstraite pour définir un mode de jeu.
 *
 * Chaque mode de jeu doit implémenter ses propres règles via la méthode `applyRules`.
 */
class Mode {
public:
    Mode();
    virtual ~Mode(); // Destructeur virtuel pour assurer la bonne destruction des objets dérivés

};

/**
 * @brief Mode Endless : jeu solo avec difficulté progressive.
 */
class Endless : public Mode {
public:
    Endless();
};

/**
 * @brief Mode Classic : jeu multijoueur (3 à 9 joueurs) avec envoi de malus (de base).
 */
class Classic : public Mode {
public:
    Classic();
    virtual void applyRules(PlayerState& player, PlayerState& targetedPlayer);
};

/**
 * @brief Mode Duel : variante du mode Classic pour 2 joueurs.
 */
class Duel : public Classic {
public:
    Duel();
    void applyRules(PlayerState& player, PlayerState& targetedPlayer) override;

};


/**
 * @brief Mode Royal Competition : les joueurs gagnent de l’énergie et choisissent leurs malus/bonus.
 */
class RoyalCompetition : public Mode {

public:
    RoyalCompetition();
    void malusReverseControls(PlayerState& player);
    void malusFreezeControls(PlayerState& player);
    void bonusReduceFallSpeed(PlayerState& player);
    void malusIncreaseFallSpeed(PlayerState& player);
    void malusThunder(PlayerState& player);
    void malusBlackOut(PlayerState& player);
    void bonusBlockCube(PlayerState& player);

};

#endif // GAMEMODES_H