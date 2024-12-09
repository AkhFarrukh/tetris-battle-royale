# Nom de l'exécutable final
TARGET = tetris

# Compilateur et options
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -lncurses -pthread

# Dossiers
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Liste des fichiers source
SRCS = $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Commandes principales
.PHONY: all clean run

all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Exécuter le jeu après compilation
run: all
	./$(TARGET)

