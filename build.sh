#!/bin/bash

# Usage:
# ./build.sh          # Build in Release mode
# ./build.sh Debug    # Build in Debug mode with symbols

# Exits on any error
set -e

# Read build type variable and set default to Release
BUILD_TYPE="${1:-Release}"

echo "=== Building in ${BUILD_TYPE} mode ==="

# Check if MinGW-w64 is installed
if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo "Error: x86_64-w64-mingw32-gcc not found."
    exit 1
fi

# Clean previous build files
echo "Cleaning previous build..."
if [ -d "build" ]; then
    rm -rf build
    echo "Removed existing build directory"
fi

# Creat build directory
echo "Creating build directory..."
mkdir build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

# Build Macro Manager
echo "Building Macro Manager..."
make -j$(nproc)

# Check if build was successful
if [ -f "bin/Macro.dll" ]; then
    echo ""
    echo "=== BUILD SUCCESSFUL ==="
    echo "Build type: ${BUILD_TYPE}"
    echo "Output: $(pwd)/bin/Macro.dll"
    echo "Size: $(du -h bin/Macro.dll | cut -f1)"
    echo ""
    echo "To install:"
    echo "  cp bin/Macro.dll /path/to/gw2/addons/"
else
    echo ""
    echo "=== BUILD FAILED ==="
    echo "DLL not found in expected location"
    exit 1
fi