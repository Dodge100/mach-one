#pragma once

#include "game/Board.h"

namespace ConnectFour {

class Evaluator {
public:
    Evaluator(int winLength);
    
    // Evaluate position for the given player
    // Positive = good for player, negative = bad for player
    int evaluate(const Board& board, Player player) const;

    // Set WinChecker for opponent win detection
    void setWinChecker(const class WinChecker* winChecker) { winChecker_ = winChecker; }
    
private:
    int winLength_;

    // For opponent win detection
    const class WinChecker* winChecker_ = nullptr;
    
    // Evaluation components
    int evaluateAllWindows(const Board& board, Player player, Player opponent) const;
    int evaluateWindow(const Board& board, int startX, int startY, 
                       int dx, int dy, Player player, Player opponent) const;
    int evaluateCenterControl(const Board& board, Player player) const;
    int evaluateThreats(const Board& board, Player player) const;
    int evaluateConnections(const Board& board, Player player) const;
    
    // Helper to check potential winning sequences
    int countPotentialWins(const Board& board, int x, int y, int dx, int dy, Player player) const;
};

} // namespace ConnectFour
