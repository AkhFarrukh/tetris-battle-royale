# Nom de l'exécutable final
TARGET = ../tetris.exe

# Compilateur et options
CXX = g++
CXXFLAGS = -I$(SRC_DIR)/include -I$(LIB_DIR)/ncurses-install/include -Wall -Wextra -std=gnu++17 -pthread -O2 -Wpedantic -march=native -Wnull-dereference -Winline -Wconversion -g
-fsanitize=address,undefined
# Dossiers sources et objets
SRC_DIR = ../src
BUILD_DIR = ../build

# Liste des fichiers source
SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRCS:.cpp=.o))

# Dossier de la bibliothèque
LIB_DIR = ../lib
NCURSES_DIR = $(LIB_DIR)/ncurses-install

# Commandes principales
.PHONY: all clean run

all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	g++ $(CXXFLAGS) -L$(LIB_DIR) -o $@ $^ -lncurses

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*/*.o $(TARGET)

# Exécuter le jeu après compilation
run: all
	LD_LIBRARY_PATH=$(NCURSES_DIR)/lib:$$LD_LIBRARY_PATH ./$(TARGET)
