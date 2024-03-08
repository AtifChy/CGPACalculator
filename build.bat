@echo off

where /q cmake ninja conan || (
    echo "One or more required tools (cmake, ninja, conan) is not installed. Please install them and try again."
    exit /b 1
)

if not exist build {
    mkdir build
}

cmake -B .\build\ -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES="conan_provider.cmake"
cmake --build .\build\ --config Release --target CGPACalculator