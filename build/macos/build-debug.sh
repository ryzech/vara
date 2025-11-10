#!/bin/bash
set -e

# Set deployment target to macOS 11.0 (Big Sur) for compatibility.
export MACOSX_DEPLOYMENT_TARGET=11.0
export CMAKE_OSX_DEPLOYMENT_TARGET=11.0

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/../.."
BUILD_DIR="$SCRIPT_DIR/../debug"

echo "Building for macOS (Debug)..."

# Clean previous build if requested
if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# Setup or reconfigure build directory
echo "Configuring build directory..."
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug

# Build
echo "Compiling targets..."
cmake --build "$BUILD_DIR" --parallel $(sysctl -n hw.ncpu)

echo "Debug build complete! Binaries in: $BUILD_DIR"
