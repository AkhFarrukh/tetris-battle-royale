# Tetris Royal

This project is developed as part of the INFO-F209 "Projet de l'année" at the Université Libre de Bruxelles

## Overview

This project includes 1 main component :

- A C++ Program ('tetris') : Launch a modified version of the famous game tetris

## Usage
### Installing needed librairies

A classic librarie giving an API made for developing textual user interface

Install command :
```
sudo apt install ncurses
```

### `tetris` Program

Command format :
```
./tetris
```

## Folder Structure

- **src/Component** : Contains all the source code of the game base components 
- **src/Controller** : Contains all the source code that will do the binding between user interraction and the game
- **src/Model** : Contains all the source code for the gameplay
- **src/View** : Contains all the source code for the rendering of the program
- **Makefile** : Builds the C++ program in `src/`
