# Tetris Royal

This project is developed as part of the INFO-F209 "Projet de l'année" at the Université Libre de Bruxelles

## Overview

This project includes 1 main component :

- A C++ Program ('tetris') : Launch a modified version of the famous game tetris

## Usage
### Installing needed librairies

This project uses ncurses, SQLite3 and Qt6. If you don't have 
them installed, you can set them up locally for Debian-based machines with the following script.

Set-up command :
```
./build/setup.sh
```


### `serever` Program
Run dedicated server :
```
./server.exe
```


### `tetris` Program

Run game in terminal mode :
```
./tetris.exe
```

Run game in GUI mode :
```
./tetris.exe --gui
```

## Folder Structure

- **src/Component** : Contains all the source code of the game base components 
- **src/Controller** : Contains all the source code that will do the binding between user interraction and the game
- **src/Model** : Contains all the source code for the gameplay
- **src/View** : Contains all the source code for the rendering of the program
- **Makefile** : Builds the C++ program in `src/`
