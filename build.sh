#!/bin/bash

# Build script for Connect Four

set -e  # Exit on error

echo "Building Connect Four..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
echo "Building..."
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

echo ""
echo "Build complete!"
echo "Executable: build/connectfour"
echo ""
echo "To run:"
echo "  ./build/connectfour --cli    (for CLI mode)"
echo "  ./build/connectfour --gui    (for GUI mode)"
