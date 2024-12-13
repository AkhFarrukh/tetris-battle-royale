#!/bin/bash

# Exit on error
set -e

# Paths
PROJECT_ROOT=$(dirname "$(dirname "$(realpath "$0")")")
LIB_DIR="$PROJECT_ROOT/lib"
INCLUDE_DIR="$PROJECT_ROOT/src/include"
BUILD_DIR="$PROJECT_ROOT/build"
NCURSES_VERSION="6.5"

echo "=== Setting up the environment ==="
echo "Project root: $PROJECT_ROOT"
echo "Library directory: $LIB_DIR"
echo "Include directory: $INCLUDE_DIR"
echo "Build directory: $BUILD_DIR"

# Step 1: Download ncurses
if [ ! -d "$LIB_DIR/ncurses-$NCURSES_VERSION" ]; then
  echo "Downloading ncurses..."
  mkdir -p "$LIB_DIR"
  wget -q -P "$LIB_DIR" "https://ftp.gnu.org/pub/gnu/ncurses/ncurses-$NCURSES_VERSION.tar.gz"
  tar -xzf "$LIB_DIR/ncurses-$NCURSES_VERSION.tar.gz" -C "$LIB_DIR"
fi

# Step 2: Build and install ncurses (shared library)
echo "Building and installing ncurses..."
cd "$LIB_DIR/ncurses-$NCURSES_VERSION"
./configure --prefix="$LIB_DIR/ncurses-install" \
            --libdir="$LIB_DIR" \
            --includedir="$LIB_DIR/include" \
            --enable-shared \
            --disable-static
make -j$(nproc)
make install

# Step 3: Update environment variables
echo "Updating environment variables..."
export LD_LIBRARY_PATH="$LIB_DIR:$LD_LIBRARY_PATH"
export CFLAGS="-I$LIB_DIR/include $CFLAGS"
export LDFLAGS="-L$LIB_DIR $LDFLAGS"

# Step 4: Compile the project
echo "Compiling the project..."
cd "$BUILD_DIR"
make clean || true
make -j$(nproc)

# Step 5: Run the application (optional)
echo "Running the application..."
"$BUILD_DIR/tetris.exe"

echo "=== Build and setup completed successfully ==="
