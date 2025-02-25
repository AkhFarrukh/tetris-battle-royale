# Nom de l'exécutable final
TARGET = ../tetris.exe

# Compilateur et options
CXX = g++
CXXFLAGS = -I$(SRC_DIR)/Component -I$(SRC_DIR)/Controller -I$(SRC_DIR)/Model -I$(SRC_DIR)/View -I$(LIB_DIR)/ncurses-install/include -Wall -Wextra -std=gnu++17 -pthread -O2 -Wpedantic -march=native -Wnull-dereference -Winline -Wconversion -g -fsanitize=address,undefined

# Dossiers sources et objets
SRC_DIR = ../src
BUILD_DIR = ../build

# Liste des fichiers source (excluding src/Server and src/server.cpp)
SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp" ! -path "$(SRC_DIR)/Server/*" ! -name "server.cpp")
OBJS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRCS:.cpp=.o))

# Dossier de la bibliothèque
LIB_DIR = ../lib
NCURSES_DIR = $(LIB_DIR)/ncurses-install

# Commandes principales
.PHONY: all clean run

all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -L$(LIB_DIR) -o $@ $^ -lncursesw

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