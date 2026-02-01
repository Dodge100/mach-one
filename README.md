# Connect Four Bot

High-performance Connect 4 game with optimal AI using parallel minimax search, configurable board sizes, and game analysis features.

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15 or higher
- SDL2 development libraries
- A local version of this repo

### macOS
```bash
brew install sdl2 cmake
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)
```

### Linux
```bash
sudo apt-get install libsdl2-dev cmake g++
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### Windows
```bash
# Install SDL2 and CMake
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

## Usage

```bash
# Launch with GUI
./connectfour --gui

# Launch with CLI
./connectfour --cli

# Configure game settings interactively in either mode
```
