cmake_minimum_required(VERSION 3.15)
project(ConnectFour VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler optimizations for Release builds
if(CMAKE_BUILD_TYPE MATCHES Release)
    if(MSVC)
        add_compile_options(/O2 /arch:AVX2)
    else()
        add_compile_options(-O3 -march=native -flto)
        add_link_options(-flto)
    endif()
endif()

# Find required packages
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

# Download nlohmann/json if not found
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
)

# Source files
set(GAME_SOURCES
    src/game/Board.cpp
    src/game/WinChecker.cpp
    src/game/GameState.cpp
)

set(AI_SOURCES
    src/ai/Evaluator.cpp
    src/ai/TranspositionTable.cpp
    src/ai/Minimax.cpp
    src/ai/ThreadPool.cpp
    src/ai/PerformanceEstimator.cpp
    src/ai/ParallelMinimax.cpp
)

set(REPLAY_SOURCES
    src/replay/GameHistory.cpp
    src/replay/PositionAnalyzer.cpp
)

set(IO_SOURCES
    src/io/GameSerializer.cpp
)

set(UI_SOURCES
    src/ui/CLI.cpp
    src/ui/GUI.cpp
)

# Main executable
add_executable(connectfour
    src/main.cpp
    ${GAME_SOURCES}
    ${AI_SOURCES}
    ${REPLAY_SOURCES}
    ${IO_SOURCES}
    ${UI_SOURCES}
)

# Link libraries
target_link_libraries(connectfour
    PRIVATE
    ${SDL2_LIBRARIES}
    Threads::Threads
    nlohmann_json::nlohmann_json
)

# Platform-specific settings
if(APPLE)
    target_link_libraries(connectfour PRIVATE "-framework CoreFoundation")
endif()

# Installation
install(TARGETS connectfour DESTINATION bin)
