#pragma once

#include "game/GameState.h"
#include <vector>

namespace ConnectFour {

struct HistoricalPosition {
    Board board;
    Player player;
    int moveColumn;
    int evaluation;      // Cached AI evaluation
    int bestMove;        // AI's suggested best move
    bool analyzed;       // Whether this position has been analyzed
};

class GameHistory {
public:
    GameHistory(const GameConfig& config);
    
    // Initialize from a completed or ongoing game
    void loadFromGameState(const GameState& gameState);
    
    // Navigation
    bool stepForward();
    bool stepBackward();
    bool jumpToMove(size_t moveIndex);
    bool jumpToStart();
    bool jumpToEnd();
    
    // Getters
    const Board& getCurrentBoard() const;
    Player getCurrentPlayer() const;
    size_t getCurrentMoveIndex() const { return currentIndex_; }
    size_t getTotalMoves() const { return positions_.size() - 1; }  // -1 for initial position
    
    const HistoricalPosition& getPosition(size_t index) const;
    const std::vector<HistoricalPosition>& getAllPositions() const { return positions_; }
    
    // Analysis data access
    bool isPositionAnalyzed(size_t index) const;
    void setPositionAnalysis(size_t index, int evaluation, int bestMove);
    
    // Configuration
    const GameConfig& getConfig() const { return config_; }
    
private:
    std::vector<HistoricalPosition> positions_;  // positions_[0] is initial empty board
    size_t currentIndex_;
    GameConfig config_;
};

} // namespace ConnectFour
