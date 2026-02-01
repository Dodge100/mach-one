#pragma once

#include "game/GameState.h"
#include "ai/ParallelMinimax.h"
#include "ai/PerformanceEstimator.h"
#include "replay/GameHistory.h"
#include "replay/PositionAnalyzer.h"
#include <memory>
#include <string>

namespace ConnectFour {

class CLI {
public:
    CLI();
    ~CLI();
    
    // Main entry point
    void run();
    
private:
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
    
    // Main menu
    void showMainMenu();
    void handleMainMenuChoice(int choice);
    
    // Game play
    void playGame();
    void renderBoard(const Board& board) const;
    void showGameStatus() const;
    int getHumanMove();
    void makeAIMove();
    void showPerformanceWarning(double estimatedTime);
    
    // Replay mode
    void replayMode();
    void showReplayControls() const;
    void showPositionAnalysis(size_t moveIndex);
    
    // Settings
    void settingsMenu();
    void configureBoard();
    void configureAI();
    void configurePlayers();
    
    // Save/Load
    void saveGame();
    void loadGame();
    
    // Utilities
    void clearScreen() const;
    void waitForEnter() const;
    std::string getPlayerSymbol(Player player) const;
    std::string getMoveQualityString(MoveQuality quality) const;
};

} // namespace ConnectFour
