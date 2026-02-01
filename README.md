# Connect Four Bot

High-performance Connect 4 game with optimal AI using parallel minimax search, configurable board sizes, and game analysis features.

## Features

- **Configurable Board**: Any board size (default 7×6) with configurable win length (default 4)
- **Powerful AI**: Parallel minimax with alpha-beta pruning, transposition tables, and configurable search depth
- **Game Modes**: Human vs Human, Human vs AI, AI vs AI
- **Replay & Analysis**: Review games with AI-powered position evaluation and move suggestions
- **Cross-Device Save/Load**: Transfer games between devices via JSON save files
- **Dual Interface**: Both CLI (terminal) and GUI (SDL2) modes

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15 or higher
- SDL2 development libraries

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

## Performance

- Search depth 8-10 recommended for standard 7×6 board
- Parallel search scales well with 2-8 CPU cores
- Performance warnings shown for expensive configurations
- Background analysis runs automatically after game completion

## License

MIT License - See LICENSE file for details
