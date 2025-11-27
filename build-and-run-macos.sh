#!/bin/bash

echo "========================================"
echo "Building Orbit Simulation"
echo "========================================"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Configure CMake (only needed first time or when CMakeLists.txt changes)
cd build
cmake .. -G "Unix Makefiles"
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project in Release mode
echo ""
echo "Building Release configuration..."
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Build failed!"
    cd ..
    exit 1
fi

# Run the executable
echo ""
echo "========================================"
echo "Running Orbit Simulation"
echo "========================================"
./OrbitSimulation
cd ..
