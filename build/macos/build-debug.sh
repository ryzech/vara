#!/bin/bash
set -e

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
if [ -d "$BUILD_DIR" ]; then
    echo "Build directory exists, reconfiguring..."
    meson setup "$BUILD_DIR" "$PROJECT_ROOT" --reconfigure \
        --buildtype=debug \
        --optimization=0 \
        -Dwarning_level=3
else
    echo "Setting up new build directory..."
    meson setup "$BUILD_DIR" "$PROJECT_ROOT" \
        --buildtype=debug \
        --optimization=0 \
        -Dwarning_level=3
fi

# Build
meson compile -C "$BUILD_DIR" -j $(sysctl -n hw.ncpu)

echo "Debug build complete! Binaries in: $BUILD_DIR"
