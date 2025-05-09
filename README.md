# Tetris Royal

This project is developed as part of the INFO-F209 "Projet de l'année" at the Université Libre de Bruxelles

## Overview

This project includes 3 main components :

- A C++ Program ('server.exe') : Launches a local server 
- A C++ Program ('tetris.exe') : Launches a modified version of the famous game tetris, it needs to have server running in local
- An SQL-Lite Database : Containing all the informations about the clients that the server will need to achieve interractions between them

## Usage
### Installing needed librairies

This project uses ncurses, SQLite3 and Qt6. If you don't have 
them installed, you can set them up locally for Debian-based machines with the following script.

Set-up command :
```
./build/setup.sh
```


### `server` Program
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

### `Makefile` Program

Build the executables (objects are build in `build` folder) :
```
make
```

Clean all the object files & executables :
```
make clean
```

Clean only a part of the object files related to the `-xxx` :
```
make clean-gui
make clean-server
make clean-client
```


## Folder Structure

- **src/Client** : Contains all the source code only related to client app
- **src/Server** : Contains all the source code only related to the server (server & game logic)
- **src/Common** : Contains all the source code that's common between the client and the server
- **Makefile** : Builds the C++ program in `src/`
- **server.exe & tetris.exe** : executables
- **createtables.sql** : sql script to build the sql-lite file
- **statements.sql** : contains the queries doable on the db
