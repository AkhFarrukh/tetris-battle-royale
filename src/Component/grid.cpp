#include "grid.hpp"



/**
 *@brief Constructeur de la classe Grid.
 *Initialisation de la grille de jeu.
 */
Grid::Grid() {
        for (int y = 0; y < 20; ++y) {
            vector<Block> row;
            for (int x = 0; x < 10; ++x) {
                // Initialiser chaque Block avec une couleur par défaut, une taille, et ses coordonnées
                row.push_back(Block(Color::nullcolor, 1, Coordinates(x, y)));
                row.back().setEmpty(true); // Marquer le bloc comme vide
            }
            matrix.push_back(row);
        }
    }



/**
 *@brief Vérifie si une position dans la matrice est vide ou pas
 * à partir des coordonnées.
 *
 *@param blockLocation Coordonnées de l'emplacement à checker.
 *@return true ou false en fonction de si l'emplacement est vide ou non
 *et false aussi si la vérification a échoué.
 */
bool Grid::isLocationFree(Coordinates blocLocation) {  // prend les coordonées d'un bloc situé dans la matrice et dis si il est vide ou non
  int x = blocLocation.x;
  int y = blocLocation.y;
  if ( x >= 0 && x < 10 && y >= 0 && y < 20 ) {
  return matrix[blocLocation.y][blocLocation.x].isEmpty();
  }


  return false; // Retourne false pour indiquer que la vérification a échoué
}


/**
 *@brief Vérifie si la ligne dans lea grid est complète.
 *
 *@param row (type int) ligne de la grid.
 *@return true si la ligne complète, false sinon.
 */
bool Grid::isFullLine(int row) {
  for(int x=0; x<10; ++x) {
    if(matrix[row][x].isEmpty()) {
      return false; // Case vide touvée, la ligne n'est pas complète
    }
  }
  return true; // La ligne est complète
}



/**
 *@brief Supprime une ligne specifique de la grid.
 *Chaque case de la grid est "vidée" et couleur réinitialisée.
 *
 *@param row ligne qui doit être supprimée.
 */
void Grid::clearLine(int row) {
  for(int x=0; x<10; ++x) {
    matrix[row][x].setEmpty(true);
    matrix[row][x].setColour(Color::nullcolor);
  }
}



/**
 *@brief Applique un effet domino sur la grid quand une ligne est détruite.
 *La méthode va déplacer vers le bas, toutes les lignes situées au dessus 
 *de la ligne détruite, et remplissant donc les cases vides.
 *
 *@param clearedRow (type int) Ligne supprimée (l'indice) 
 */
void Grid::applyDominoEffect(int clearedRow) {
    for (int y = clearedRow; y > 0; --y) { // Parcourir de la ligne supprimée vers le haut
        for (int x = 0; x < 10; ++x) {
            // Prendre les propriétés du bloc au dessus
            matrix[y][x].setColour(matrix[y - 1][x].getColour());
            matrix[y][x].setEmpty(matrix[y - 1][x].isEmpty());

        }
    }

    // Réinitialiser la première ligne (tout en haut) à vide
    for (int x = 0; x < 10; ++x) {
        matrix[0][x].setColour(Color::nullcolor);
        matrix[0][x].setEmpty(true);

    }
}



/**
 *@brief Supprime toutes les lignes complètes de la grid.
 */
void Grid::clearFullLines() {
    for (int y = 0; y < 20; ++y) {  //Parcour le tableau a chaque nouveau placement d'une forme afin de traiter les
        if (isFullLine(y)) {        //lignes completes
            clearLine(y);  // va la vider
            applyDominoEffect(y);  // fait decendre les couleur du dessus
        }
    }
}



/**
 *@brief Place un nouveau block dans la grid.
 *La méthode utilise les coordonnées du block pour le placement.
 *
 *@param newBlock Le block qui est placé.
 */
void Grid::setBlock(Block newblock) {
  Coordinates coordinates = newblock.getPosition();
  int x = coordinates.x;
  int y = coordinates.y;
  if ( x >= 0 && x < 10 && y >= 0 && y < 20 ) {
    matrix[y][x].setColour(newblock.getColour());
    matrix[y][x].setEmpty(newblock.isEmpty());
  }
}


/**
 *@brief récupère la couleur à une position de la grid.
 *
 *@param x coordonnée x dans la grid.
 *@param y coordonnée y.
 *@return la couleur à une position de la grid. 
 */
Color Grid::getBlockColor(int x, int y) {
  if ( x >= 0 && x < 10 && y >= 0 && y < 20 ) {
    return matrix[y][x].getColour();
    };
    return Color::nullcolor;  // Renvoie une couleur de case vide si on sort du tableau
}

void Grid::malusLines(int y, int shift){

}


