#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/../.."
BUILD_DIR="$SCRIPT_DIR/../release"

echo "Building for Linux (Release)..."

# Clean previous build if requested
if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# Setup or reconfigure build directory
echo "Configuring build directory..."
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

# Build
echo "Compiling targets..."
cmake --build "$BUILD_DIR" --parallel "$(nproc)"

echo "Release build complete! Binaries in: $BUILD_DIR"
