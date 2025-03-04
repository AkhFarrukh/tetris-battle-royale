# Nom des l'exécutables finaux
SERVER_TARGET := ../server.exe
CLIENT_TARGET := ../tetris.exe

# Compilateur et options
CXX = g++
CXXFLAGS = -I$(SRC_DIR)/Component -I$(SRC_DIR)/Controller -I$(SRC_DIR)/Model -I$(SRC_DIR)/View -I$(SRC_DIR)/Database -I$(SRC_DIR)/Server -I$(LIB_DIR)/ncurses-install/include -Wall -Wextra -std=c++2b -pthread -O2 -Wpedantic -march=native -Wnull-dereference -Winline -Wconversion -g
#-fsanitize=address,undefined

# Dossiers sources et objets
SRC_DIR = ../src
BUILD_DIR = ../build

# Liste des fichiers source
# SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp" ! -path "$(SRC_DIR)/Server/*" ! -name "server.cpp")
SERVER_SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp" ! -name "tetris.cpp")
CLIENT_SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp" ! -path "$(SRC_DIR)/Server/*" ! -name "server.cpp")
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CLIENT_SRCS))

# Dossier de la bibliothèque
LIB_DIR = ../lib
NCURSES_DIR = $(LIB_DIR)/ncurses-install

# Commandes principales
.PHONY: all clean run

all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Compilation de l'exécutable server
$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -L$(LIB_DIR) -o $@ $^ -lncursesw -lsqlite3

# Compilation de l'exécutable client
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -L$(LIB_DIR) -o $@ $^ -lncursesw -lsqlite3

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Make build dir if doesn't exist
#$(BUILD_DIR):
#	@mkdir -p $(dir $@)

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*/*.o $(SERVER_TARGET) $(CLIENT_TARGET)

# Exécuter le jeu après compilation
run: all
	LD_LIBRARY_PATH=$(NCURSES_DIR)/lib:$$LD_LIBRARY_PATH ./$(TARGET)