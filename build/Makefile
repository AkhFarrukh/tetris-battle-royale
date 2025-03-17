# Nom des exécutables finaux
SERVER_TARGET := ../server.exe
CLIENT_TARGET := ../tetris.exe

# Dossiers sources et objets
SRC_DIR = ../src
BUILD_DIR = ../build
LIB_DIR = ../lib
NCURSES_DIR = $(LIB_DIR)/ncurses-install
SQLITE_DIR = $(LIB_DIR)/sqlite-install

# Détection automatique de l'OS (MacOS/Linux)
UNAME_S := $(shell uname -s)

# Chemins par défaut
LDFLAGS = -L$(NCURSES_DIR)/lib -L$(SQLITE_DIR)/lib -L$(LIB_DIR)
INCLUDE_FLAGS = -I$(NCURSES_DIR)/include -I$(SQLITE_DIR)/include -I$(LIB_DIR)/include
LIBS = -lncursesw -lsqlite3

# Ajout spécifique pour macOS uniquement
ifeq ($(UNAME_S),Darwin)
	NCURSES_HOMEBREW := $(shell brew --prefix ncurses)
	SQLITE_HOMEBREW := $(shell brew --prefix sqlite)

	LDFLAGS += -L$(NCURSES_HOMEBREW)/lib -L$(SQLITE_HOMEBREW)/lib
	INCLUDE_FLAGS += -I$(NCURSES_HOMEBREW)/include -I$(SQLITE_HOMEBREW)/include
	export LD_LIBRARY_PATH := $(NCURSES_HOMEBREW)/lib:$(SQLITE_HOMEBREW)/lib:$(LD_LIBRARY_PATH)
endif

# Variables d'environnement
export LD_LIBRARY_PATH := $(LIB_DIR):$(NCURSES_DIR)/lib:$(SQLITE_DIR)/lib:$(LD_LIBRARY_PATH)

# Compilateur et options
CXX = g++
CXXFLAGS = -I$(SRC_DIR)/Component -I$(SRC_DIR)/Player -I$(SRC_DIR)/Controller -I$(SRC_DIR)/Model -I$(SRC_DIR)/View -I$(SRC_DIR)/Database -I$(SRC_DIR)/Server $(INCLUDE_FLAGS) -Wall -Wextra -std=c++2b -pthread -O2 -Wpedantic -march=native -Wnull-dereference -Winline -Wconversion -g

# Liste des fichiers source
SERVER_SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp" ! -name "tetris.cpp")
CLIENT_SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp" ! -name "server.cpp")
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CLIENT_SRCS))

# Commandes principales
.PHONY: all clean run

all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Compilation de l'exécutable server
$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# Compilation de l'exécutable client
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*/*.o $(SERVER_TARGET) $(CLIENT_TARGET)

# Exécuter le jeu après compilation
run: all
	LD_LIBRARY_PATH=$(NCURSES_DIR)/lib:$(SQLITE_DIR)/lib:$(LIB_DIR):$$LD_LIBRARY_PATH ./$(CLIENT_TARGET)
