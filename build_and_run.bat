@echo off
echo ========================================
echo Building Orbit Simulation
echo ========================================

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Configure CMake (only needed first time or when CMakeLists.txt changes)
cd build
cmake .. -G "Visual Studio 17 2022"
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)

REM Build the project in Release mode
echo.
echo Building Release configuration...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    cd ..
    pause
    exit /b %ERRORLEVEL%
)

REM Run the executable
echo.
echo ========================================
echo Running Orbit Simulation
echo ========================================
cd Release
OrbitSimulation.exe
cd ..\..


