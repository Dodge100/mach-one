#!/bin/bash

# Build release binaries for Connect Four
# This script builds an optimized release for the current platform

set -e

echo "========================================="
echo "  Connect Four - Release Build"
echo "========================================="
echo ""

# Detect platform
PLATFORM=$(uname -s)
ARCH=$(uname -m)

case "$PLATFORM" in
    Darwin)
        PLATFORM_NAME="macos"
        ;;
    Linux)
        PLATFORM_NAME="linux"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        PLATFORM_NAME="windows"
        ;;
    *)
        PLATFORM_NAME="unknown"
        ;;
esac

case "$ARCH" in
    x86_64|amd64)
        ARCH_NAME="x64"
        ;;
    arm64|aarch64)
        ARCH_NAME="arm64"
        ;;
    *)
        ARCH_NAME="$ARCH"
        ;;
esac

BINARY_NAME="connectfour-${PLATFORM_NAME}-${ARCH_NAME}"
echo "Building for: $PLATFORM_NAME ($ARCH_NAME)"
echo ""

# Create build directory
rm -rf build-release
mkdir -p build-release
cd build-release

# Configure with optimizations
echo "Configuring..."
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
echo "Building..."
if [ "$PLATFORM" = "Darwin" ]; then
    make -j$(sysctl -n hw.ncpu)
elif [ "$PLATFORM" = "Linux" ]; then
    make -j$(nproc)
else
    cmake --build . --config Release
fi

# Create releases directory
cd ..
mkdir -p releases

# Copy binary
if [ "$PLATFORM_NAME" = "windows" ]; then
    cp build-release/Release/connectfour.exe "releases/${BINARY_NAME}.exe"
    echo ""
    echo "Release binary: releases/${BINARY_NAME}.exe"
else
    cp build-release/connectfour "releases/${BINARY_NAME}"
    chmod +x "releases/${BINARY_NAME}"
    echo ""
    echo "Release binary: releases/${BINARY_NAME}"
fi

echo ""
echo "Build complete!"
echo ""

# Show file info
if command -v file &> /dev/null; then
    file "releases/${BINARY_NAME}"*
fi

ls -lh "releases/${BINARY_NAME}"*
