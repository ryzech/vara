#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/../.."
BUILD_DIR="$SCRIPT_DIR/release"

echo "Building for macOS (Release)..."

# Clean previous build if requested
if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# Setup or reconfigure build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Build directory exists, reconfiguring..."
    meson setup "$BUILD_DIR" "$PROJECT_ROOT" --reconfigure \
        --buildtype=release \
        --optimization=3 \
        --strip
else
    echo "Setting up new build directory..."
    meson setup "$BUILD_DIR" "$PROJECT_ROOT" \
        --buildtype=release \
        --optimization=3 \
        --strip
fi

# Build
meson compile -C "$BUILD_DIR" -j $(sysctl -n hw.ncpu)

echo "Release build complete! Binaries in: $BUILD_DIR"
