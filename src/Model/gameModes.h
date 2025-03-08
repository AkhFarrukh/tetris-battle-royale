//
// Created by firasse on 02/03/25.
//

#ifndef GAMEMODES_H
#define GAMEMODES_H

#include <iostream>
#include "../Component/grid.hpp"
#include "../Controller/GameController.hpp"
#include "game.hpp"

/**
 * @brief Classe de base abstraite pour définir un mode de jeu.
 *
 * Chaque mode de jeu doit implémenter ses propres règles via la méthode `applyRules`.
 */
class Mode {
protected:
    int numPlayers_;  // le nombre de joueur.
public:
    Mode(int numPlayers);
    virtual ~Mode(); // Destructeur virtuel pour assurer la bonne destruction des objets dérivés
    virtual void setPlayers(int num) = 0; //Virtuel oblige les modes à implémenter à leur façon.
    virtual int getPlayers() = 0;
    virtual void applyRules(Grid& grid, int linesCleared) = 0;
};

/**
 * @brief Mode Endless : jeu solo avec difficulté progressive.
 */
class Endless : public Mode {
public:
    Endless();
    void setPlayers(int num) override;
    int getPlayers() override;
    /**
     * @brief Applique les règles spécifiques du mode de jeu.
     *
     * @param grid La grille de jeu du joueur.
     * @param linesCleared Le nombre de lignes complétées.
     */
    void applyRules(Grid& grid, int linesCleared) override; //S'occupe de la gestion de malus
};

/**
 * @brief Mode Classic : jeu multijoueur (3 à 9 joueurs) avec envoi de malus (de base).
 */
class Classic : public Mode {
public:
    Classic(int numPlayers);
    void setPlayers(int num) override;
    int getPlayers() override;
    void applyRules(Grid& grid, int linesCleared) override;
};

/**
 * @brief Mode Duel : variante du mode Classic pour 2 joueurs.
 */
class Duel : public Classic {
public:
    Duel();
};


/**
 * @brief Mode Royal Competition : les joueurs gagnent de l’énergie et choisissent leurs malus/bonus.
 */
class RoyalCompetition : public Mode {
    int energy; //l'énergie s'accumule afin de permettre l'utilisation de malus/bonus
public:
    RoyalCompetition(int numPlayers);
    void setPlayers(int num) override;
    int getPlayers() override;
    void applyRules(Grid& grid, int linesCleared) override;
	void malusReverseControls(Game game, GameController& controller, bool isReverse);
    void malusFreezeControls(Game game, GameController controller, bool isBlocked);
    void bonusReduceFallSpeed(Game game);
    void malusIncreaseFallSpeed(Game game);
    int getEnergyUsed() {return energy;}
};

#endif // GAMEMODES_H