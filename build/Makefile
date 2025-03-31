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

# Qt6 flags (via pkg-config)
QT_PACKAGES = Qt6Core Qt6Widgets Qt6Gui
QT_CXXFLAGS := $(shell pkg-config --cflags $(QT_PACKAGES))
QT_LDFLAGS := $(shell pkg-config --libs $(QT_PACKAGES))

# Chemins par défaut
LDFLAGS = -L$(NCURSES_DIR)/lib -L$(SQLITE_DIR)/lib -L$(LIB_DIR)
INCLUDE_FLAGS = -I$(NCURSES_DIR)/include -I$(SQLITE_DIR)/include -I$(LIB_DIR)/include
LIBS = -lncursesw -lsqlite3
MOC := moc

# macOS specific
ifeq ($(UNAME_S),Darwin)
	NCURSES_HOMEBREW := $(shell brew --prefix ncurses)
	SQLITE_HOMEBREW := $(shell brew --prefix sqlite)
	QT_HOMEBREW := $(shell brew --prefix qt)

	LDFLAGS += -L$(NCURSES_HOMEBREW)/lib -L$(SQLITE_HOMEBREW)/lib -L$(QT_HOMEBREW)/lib
	INCLUDE_FLAGS += -I$(NCURSES_HOMEBREW)/include -I$(SQLITE_HOMEBREW)/include -I$(QT_HOMEBREW)/include
	export LD_LIBRARY_PATH := $(NCURSES_HOMEBREW)/lib:$(SQLITE_HOMEBREW)/lib:$(QT_HOMEBREW)/lib:$(LD_LIBRARY_PATH)
	MOC := $(QT_HOMEBREW)/share/qt/libexec/moc
endif

export LD_LIBRARY_PATH := $(LIB_DIR):$(NCURSES_DIR)/lib:$(SQLITE_DIR)/lib:$(LD_LIBRARY_PATH)

# Compilateur
CXX = g++
COMMON_CXXFLAGS = -I$(SRC_DIR)/Component -I$(SRC_DIR)/Player -I$(SRC_DIR)/Controller -I$(SRC_DIR)/Model -I$(SRC_DIR)/View -I$(SRC_DIR)/Database -I$(SRC_DIR)/Server -I$(SRC_DIR)/Gui $(INCLUDE_FLAGS) -Wall -Wextra -std=c++2b -pthread -O2 -Wpedantic -march=native -Wnull-dereference -Winline -Wconversion -g

# Sources
SERVER_SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp" ! -name "tetris.cpp" ! -path "$(SRC_DIR)/Gui/*")
CLIENT_SRCS = $(shell find $(SRC_DIR) -type f -name "*.cpp" ! -name "server.cpp")

# Objet principaux
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CLIENT_SRCS))

# --- AUTOMOC ---
# Détection des fichiers .hpp avec Q_OBJECT
MOC_HEADERS := $(shell grep -l Q_OBJECT $(SRC_DIR)/Gui/*.hpp 2>/dev/null)
MOC_CPP := $(patsubst $(SRC_DIR)/Gui/%.hpp, $(BUILD_DIR)/Gui/moc_%.cpp, $(MOC_HEADERS))
MOC_OBJS := $(patsubst %.cpp, %.o, $(MOC_CPP))

CLIENT_OBJS += $(MOC_OBJS)

# Commandes principales
.PHONY: all clean run

all: $(SERVER_TARGET) $(CLIENT_TARGET)

# --- LINK ---
$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(COMMON_CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(COMMON_CXXFLAGS) $(QT_CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS) $(QT_LDFLAGS)

# --- OBJ compilation ---
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON_CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

# --- MOC generation ---
$(BUILD_DIR)/Gui/moc_%.cpp: $(SRC_DIR)/Gui/%.hpp
	@mkdir -p $(dir $@)
	$(MOC) $(QT_CXXFLAGS) $< -o $@

# --- Compile MOC files ---
$(BUILD_DIR)/Gui/%.o: $(BUILD_DIR)/Gui/%.cpp
	$(CXX) $(COMMON_CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

# --- CLEAN ---
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*/*.o $(BUILD_DIR)/Gui/moc_*.cpp $(SERVER_TARGET) $(CLIENT_TARGET)

run: all
	LD_LIBRARY_PATH=$(NCURSES_DIR)/lib:$(SQLITE_DIR)/lib:$(LIB_DIR):$$LD_LIBRARY_PATH ./$(CLIENT_TARGET)
