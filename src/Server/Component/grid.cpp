#include "grid.hpp"

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;

/**
 *@brief Constructeur de la classe Grid.
 *Initialisation de la grille de jeu.
 */
Grid::Grid() {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            vector<Block> row;
            for (int x = 0; x < GRID_WIDTH; ++x) {
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
  if ( x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT ) {
  return matrix[blocLocation.y][blocLocation.x].isEmpty();
  }


  return false; // Retourne false pour indiquer que la vérification a échoué
}


/**
 *@brief Vérifie si la ligne dans lea grid est complète.
 *
 *@param line (type int) ligne de la grid.
 *@return true si la ligne complète, false sinon.
 */
bool Grid::isFullLine(int line) {
  for(int x=0; x < GRID_WIDTH; ++x) {
    if(matrix[line][x].isEmpty()) {
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
void Grid::clearLine(int line) {
  for(int x=0; x < GRID_WIDTH; ++x) {
    matrix[line][x].setEmpty(true);
    matrix[line][x].setColour(Color::nullcolor);
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
        for (int x = 0; x < GRID_WIDTH; ++x) {
            // Prendre les propriétés du bloc au dessus
            matrix[y][x].setColour(matrix[y - 1][x].getColour());
            matrix[y][x].setEmpty(matrix[y - 1][x].isEmpty());

        }
    }

    // Réinitialiser la première ligne (tout en haut) à vide
    for (int x = 0; x < GRID_WIDTH; ++x) {
        matrix[0][x].setColour(Color::nullcolor);
        matrix[0][x].setEmpty(true);

    }
}



/**
 *@brief Supprime toutes les lignes complètes de la grid.
 */
void Grid::clearFullLines() {
    for (int y = 0; y < GRID_HEIGHT; ++y) {  //Parcour le tableau a chaque nouveau placement d'une forme afin de traiter les
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
  if ( x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT ) {
    matrix[y][x].setColour(newblock.getColour());
    matrix[y][x].setEmpty(newblock.isEmpty());
  }
}


bool Grid::isLineBusy(int line) {
    for(int x=0; x < GRID_WIDTH; ++x) {
        if(!matrix[line][x].isEmpty()) {
            return true; // Cette ligne n'est pas complète mais contient des blocs
        }
    }
    return false; // La ligne est complète
}






/**
 *@brief récupère la couleur à une position de la grid.
 *
 *@param x coordonnée x dans la grid.
 *@param y coordonnée y.
 *@return la couleur à une position de la grid. 
 */
Color Grid::getBlockColor(int x, int y) {
  if ( x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT ) {
    return matrix[y][x].getColour();
    };
    return Color::nullcolor;  // Renvoie une couleur de case vide si on sort du tableau
}
/*
 *@brief va ajouter une ligne grise avec un bloc vide afin de saboter la game de l'adversaire
 *
 *@param numLines est le nombres de lignes malus à ajouter
 */
void Grid::malusLines(int numLines) {
    int line = 0; //Tout en haut
    while (line < GRID_HEIGHT && !isLineBusy(line)) {
        ++line;
    }
    if (line > GRID_HEIGHT - 1) { //Si aucune line n'est vide (ce qui aurait deja fait finir la game donc jsp
        --line; //ligne vide au-dessus d'une ligne partiellement rempli
    }

    int heightAfterAddingLines = line - numLines;
    if (heightAfterAddingLines < 0) {  //Si le nombre de ligne à ajouter sort de la grid et va donc faire perdre le joueur
        //todo "Perdu"
    }
    for (int l = line; l > heightAfterAddingLines; --l) {
        srand(static_cast<uint>(time(nullptr)));  // Initialisation avec l'heure actuelle
        int indiceCaseVide = rand() % GRID_WIDTH;   //Selection au hasard de la case qui sera vide
        for (int row=0; row < GRID_WIDTH; row++){
            if (row != indiceCaseVide) {
                Block block = Block(Color::purple, 1, Coordinates(row, l));
                if (l >= 0 && l < GRID_HEIGHT) {
                    setBlock(block);
                }
            }
        }
    }

}

/**
 * @brief Utiliser dans le mode Royal Competition.
 *
 * Sera utiliser pour le malus Thunder qui détruit un bloc 2X2.
 *
 */
void Grid::lightningBlock2X2(Coordinates blocLocation) {
    int x = blocLocation.x;
    int y = blocLocation.y;
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        setBlock(Block(Color::white, 1, blocLocation));
        if (x+1 < GRID_WIDTH) {
            setBlock(Block(Color::white, 1, Coordinates(x+1, y)));
            if (y + 1 < GRID_HEIGHT) {
                setBlock(Block(Color::white, 1, Coordinates(x, y+1)));
                setBlock(Block(Color::white, 1, Coordinates(x+1, y+1)));
            }
        }
        else if (x-1 >= 0) {
            setBlock(Block(Color::white, 1, Coordinates(x-1, y)));
            if (y + 1 < GRID_HEIGHT) {
                setBlock(Block(Color::white, 1, Coordinates(x, y+1)));
                setBlock(Block(Color::white, 1, Coordinates(x-1, y+1)));
            }
        }
    }
}


/**
 * @brief Utiliser dans le mode Royal Competition.
 *
 * Sera utiliser pour le malus Thunder qui détruit un bloc 2X2.
 *
 */
void Grid::deleteBlock2X2(Coordinates blocLocation) {
    int x = blocLocation.x;
    int y = blocLocation.y;
    Block empty = Block(Color::nullcolor, 1, blocLocation);
    empty.setEmpty(true); //todo peut etre devoir crée un block pour chaque lieu
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        setBlock(empty);
        if (x+1 < GRID_WIDTH) {
            empty.setPosition(Coordinates(x+1, y));
            setBlock(empty);
            if (y + 1 < GRID_HEIGHT) {
                empty.setPosition(Coordinates(x, y+1));
                setBlock(empty);
                empty.setPosition(Coordinates(x+1, y+1));
                setBlock(empty);
            }
        }
        else if (x-1 >= 0) {
            setBlock(Block(Color::nullcolor, 1, Coordinates(x-1, y)));
            if (y + 1 < GRID_HEIGHT) {
                setBlock(Block(Color::nullcolor, 1, Coordinates(x, y+1)));
                setBlock(Block(Color::nullcolor, 1, Coordinates(x-1, y+1)));
            }
        }
    }
}


