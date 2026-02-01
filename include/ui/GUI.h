#pragma once

#include "game/GameState.h"
#include "ai/ParallelMinimax.h"
#include "ai/PerformanceEstimator.h"
#include "replay/GameHistory.h"
#include "replay/PositionAnalyzer.h"

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <memory>
#include <string>

namespace ConnectFour {

class GUI {
public:
    GUI();
    ~GUI();
    
    // Main entry point
    void run();
    
private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool running_;
    
    std::unique_ptr<GameState> gameState_;
    std::unique_ptr<ParallelMinimax> aiEngine_;
    std::unique_ptr<PerformanceEstimator> perfEstimator_;
    std::unique_ptr<GameHistory> gameHistory_;
    std::unique_ptr<PositionAnalyzer> analyzer_;
    
    enum class Mode {
        MainMenu,
        Playing,
        Replay,
        Settings
    };
    
    Mode currentMode_;
    
    // Window dimensions
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 700;
    static constexpr int CELL_SIZE = 70;
    static constexpr int BOARD_OFFSET_X = 50;
    static constexpr int BOARD_OFFSET_Y = 100;
    
    // Initialization
    bool initialize();
    void cleanup();
    
    // Main loop
    void handleEvents();
    void update();
    void render();
    
    // Event handlers
    void handleMouseClick(int x, int y);
    void handleKeyPress(SDL_Keycode key);
    
    // Rendering
    void renderBoard(const Board& board);
    void renderCell(int x, int y, Player player);
    void renderUI();
    void renderReplayControls();
    
    // Game logic
    void makeAIMove();
    int getColumnFromMouseX(int mouseX) const;
    
    // UI helpers
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    void drawRect(int x, int y, int w, int h, SDL_Color color, bool filled = true);
    void drawCircle(int centerX, int centerY, int radius, SDL_Color color, bool filled = true);
};

} // namespace ConnectFour
