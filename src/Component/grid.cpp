#include "grid.hpp"


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

bool Grid::isLocationFree(Coordinates blocLocation) {  // prend les coordonées d'un bloc situé dans la matrice et dis si il est vide ou non
  int x = blocLocation.x;
  int y = blocLocation.y;
  if ( x >= 0 && x < 10 && y >= 0 && y < 20 ) {
  return matrix[blocLocation.y][blocLocation.x].isEmpty();
  }


  return false; // Retourne false pour indiquer que la vérification a échoué
}

bool Grid::isFullLine(int row) {
  for(int x=0; x<10; ++x) {
    if(matrix[row][x].isEmpty()) {
      return false; // Case vide touvée, la ligne n'est pas complète
    }
  }
  return true; // La ligne est complète
}

void Grid::clearLine(int row) {
  for(int x=0; x<10; ++x) {
    matrix[row][x].setEmpty(true);
    matrix[row][x].setColour(Color::nullcolor);
  }
}

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

void Grid::clearFullLines() {
    for (int y = 0; y < 20; ++y) {  //Parcour le tableau a chaque nouveau placement d'une forme afin de traiter les
        if (isFullLine(y)) {        //lignes completes
            clearLine(y);  // va la vider
            applyDominoEffect(y);  // fait decendre les couleur du dessus
        }
    }
}

void Grid::setBlock(Block newblock) {
  Coordinates coordinates = newblock.getPosition();
  int x = coordinates.x;
  int y = coordinates.y;
  if ( x >= 0 && x < 10 && y >= 0 && y < 20 ) {
    matrix[y][x].setColour(newblock.getColour());
    matrix[y][x].setEmpty(newblock.isEmpty());
  }
}

Color Grid::getBlockColor(int x, int y) {
  if ( x >= 0 && x < 10 && y >= 0 && y < 20 ) {
    return matrix[y][x].getColour();
    };
    return Color::nullcolor;  // Renvoie une couleur de case vide si on sort du tableau
}

void Grid::malusLines(int y, int shift){

}


