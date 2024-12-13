# Nom de l'exécutable final
TARGET = ../tetris.exe

# Compilateur et options
CXX = g++
CXXFLAGS = -I../src/include -Wall -Wextra -std=gnu++17 -lncurses -pthread -O2 -Wpedantic -march=native -Wnull-dereference -Winline -Wconversion -g -fsanitize=address,undefined

# Dossiers sources et objets
SRC_DIR = ../src
BUILD_DIR = .

# Liste des fichiers source
SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRCS:.cpp=.o))

# Commandes principales
.PHONY: all clean run

all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)

# Exécuter le jeu après compilation
run: all
	../$(notdir $(TARGET))

