#!/bin/bash

# Exit on error
set -e

# Paths
PROJECT_ROOT=$(dirname "$(dirname "$(realpath "$0")")")
LIB_DIR="$PROJECT_ROOT/lib"
NCURSES_VERSION="6.5"
NCURSES_INSTALL_DIR="$LIB_DIR/ncurses-install"
SQLITE_VERSION="3490200"
SQLITE_YEAR="2025"
QT_VERSION="6.9.0"
QT_INSTALL_DIR="$LIB_DIR/qt-install"


echo "=== Setting up project dependencies ==="
echo "Project root: $PROJECT_ROOT"
echo "Library directory: $LIB_DIR"

# Create directories if they don't exist
mkdir -p "$LIB_DIR"


# Step 1: Install ncurses locally
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

# Step 3: Install Qt6 locally
if [ ! -d "$QT_INSTALL_DIR" ]; then
  echo "=== Installing Qt6 locally ==="

  if [ ! -d "$LIB_DIR/qt-$QT_VERSION" ]; then
    echo "Downloading Qt6..."
    wget -q -P "$LIB_DIR" "https://download.qt.io/official_releases/qt/6.9/$QT_VERSION/single/qt-everywhere-src-$QT_VERSION.tar.xz"
    tar -xf "$LIB_DIR/qt-everywhere-src-$QT_VERSION.tar.xz" -C "$LIB_DIR"
    mv "$LIB_DIR/qt-everywhere-src-$QT_VERSION" "$LIB_DIR/qt-$QT_VERSION"
    rm "$LIB_DIR/qt-everywhere-src-$QT_VERSION.tar.xz"
  fi

  echo "Building Qt6 (this will take a while)..."
  cd "$LIB_DIR/qt-$QT_VERSION"
  ./configure -prefix "$QT_INSTALL_DIR" \
              -release \
              -opensource -confirm-license \
              -nomake examples -nomake tests \
              -skip qtwebengine -skip qtlocation \
              -skip qtmultimedia -skip qtsensors \
              -skip qtconnectivity -skip qtwayland \
              -no-feature-printer -no-feature-pdf \
              -qt-libpng -qt-libjpeg \
              -- -DCMAKE_BUILD_TYPE=Release

  # Build and install
  cmake --build . --parallel $(nproc)
  cmake --install .

  echo "Qt6 installed locally at $QT_INSTALL_DIR"
  cd "$PROJECT_ROOT"
else
  echo "Qt6 already installed at $QT_INSTALL_DIR"
fi



echo "=== All dependencies installed successfully ==="

# Set environment variables for compilation
export LD_LIBRARY_PATH="$LIB_DIR:$NCURSES_INSTALL_DIR/lib:$LIB_DIR/sqlite-install/lib:$LD_LIBRARY_PATH"
export CXXFLAGS="-std=c++2b -I$NCURSES_INSTALL_DIR/include -I$LIB_DIR/sqlite-install/include $CXXFLAGS"
export LDFLAGS="-L$NCURSES_INSTALL_DIR/lib -L$LIB_DIR/sqlite-install/lib -L$LIB_DIR $LDFLAGS"

export PATH="$QT_INSTALL_DIR/bin:$PATH"
export LD_LIBRARY_PATH="$QT_INSTALL_DIR/lib:$LD_LIBRARY_PATH"
export PKG_CONFIG_PATH="$QT_INSTALL_DIR/lib/pkgconfig:$PKG_CONFIG_PATH"

echo "=== Build complete ==="
echo ""
echo "To run the server: $PROJECT_ROOT/server.exe"
echo "To run the client: $PROJECT_ROOT/tetris.exe"
echo "To run the client with GUI: $PROJECT_ROOT/tetris.exe --gui"


