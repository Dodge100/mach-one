# Mach One

Connect 4 engine using parallel minimax search and game analysis features.

For now using the cli is preferred since it has the most support for features however the gui can also let you play games against it.

## Usage
Download the latest [release](https://github.com/Dodge100/mach-one/actions) and click on it to play.

## Development

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

running:
```bash
# Launch with GUI
./connectfour --gui

# Launch with CLI
./connectfour --cli

# Configure game settings interactively in either mode
```
