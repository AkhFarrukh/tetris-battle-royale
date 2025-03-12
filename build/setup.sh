#!/bin/bash

# Exit on error
set -e

# Paths
PROJECT_ROOT=$(dirname "$(dirname "$(realpath "$0")")")
LIB_DIR="$PROJECT_ROOT/lib"
INCLUDE_DIR="$LIB_DIR/include"
NCURSES_VERSION="6.5"
NCURSES_INSTALL_DIR="$LIB_DIR/ncurses-install"
SQLITE_VERSION="3450000"
SQLITE_YEAR="2024"

echo "=== Setting up project dependencies ==="
echo "Project root: $PROJECT_ROOT"
echo "Library directory: $LIB_DIR"

# Create directories if they don't exist
mkdir -p "$LIB_DIR"
mkdir -p "$INCLUDE_DIR"

# Step 1: Download and install ncurses locally
if [ ! -d "$NCURSES_INSTALL_DIR" ]; then
  echo "=== Installing ncurses locally ==="

  if [ ! -d "$LIB_DIR/ncurses-$NCURSES_VERSION" ]; then
    echo "Downloading ncurses..."
    wget -q -P "$LIB_DIR" "https://ftp.gnu.org/pub/gnu/ncurses/ncurses-$NCURSES_VERSION.tar.gz"
    tar -xzf "$LIB_DIR/ncurses-$NCURSES_VERSION.tar.gz" -C "$LIB_DIR"
    rm "$LIB_DIR/ncurses-$NCURSES_VERSION.tar.gz"
  fi

  echo "Building ncurses..."
  cd "$LIB_DIR/ncurses-$NCURSES_VERSION"
  ./configure --prefix="$NCURSES_INSTALL_DIR" \
              --with-shared \
              --without-debug \
              --without-normal \
              --without-manpages \
              --enable-widec
  make -j$(nproc)
  make install
  echo "ncurses installed locally at $NCURSES_INSTALL_DIR"
  cd "$PROJECT_ROOT"
else
  echo "NCurses already installed at $NCURSES_INSTALL_DIR"
fi

# Step 2: Install SQLite3
if [ ! -d "$LIB_DIR/sqlite-install" ]; then
  echo "=== Installing SQLite3 locally ==="

  wget -q -P "$LIB_DIR" "https://www.sqlite.org/$SQLITE_YEAR/sqlite-autoconf-$SQLITE_VERSION.tar.gz"
  tar -xzf "$LIB_DIR/sqlite-autoconf-$SQLITE_VERSION.tar.gz" -C "$LIB_DIR"
  rm "$LIB_DIR/sqlite-autoconf-$SQLITE_VERSION.tar.gz"

  cd "$LIB_DIR/sqlite-autoconf-$SQLITE_VERSION"
  ./configure --prefix="$LIB_DIR/sqlite-install"
  make -j$(nproc)
  make install

  echo "SQLite3 installed locally at $LIB_DIR/sqlite-install"
  cd "$PROJECT_ROOT"
else
  echo "SQLite3 already installed"
fi

echo "=== All dependencies installed successfully ==="

# Set environment variables for compilation
export LD_LIBRARY_PATH="$LIB_DIR:$NCURSES_INSTALL_DIR/lib:$LIB_DIR/sqlite-install/lib:$LD_LIBRARY_PATH"
export CXXFLAGS="-I$NCURSES_INSTALL_DIR/include -I$LIB_DIR/sqlite-install/include -I$INCLUDE_DIR $CXXFLAGS"
export LDFLAGS="-L$NCURSES_INSTALL_DIR/lib -L$LIB_DIR/sqlite-install/lib -L$LIB_DIR $LDFLAGS"

# Step 3: Build the project using the Makefile
echo "=== Building the project ==="
cd "$PROJECT_ROOT/build"
make all

echo "=== Build complete ==="
echo ""
echo "To run the client: ../tetris.exe"
echo "To run the server: ../server.exe"
echo ""
echo "Note: When running manually, make sure to set the environment variables:"
echo "export LD_LIBRARY_PATH=\"$LIB_DIR:$NCURSES_INSTALL_DIR/lib:$LIB_DIR/sqlite-install/lib:\$LD_LIBRARY_PATH\""