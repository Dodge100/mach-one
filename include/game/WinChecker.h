#pragma once

#include "Board.h"
#include <optional>

namespace ConnectFour {

struct WinInfo {
    Player winner;
    int startX, startY;
    int endX, endY;
    enum Direction { Horizontal, Vertical, DiagonalUp, DiagonalDown } direction;
};

class WinChecker {
public:
    WinChecker(int winLength);
    
    // Check if the last move resulted in a win
    std::optional<WinInfo> checkWin(const Board& board, int lastColumn) const;
    
    // Check if there's a win anywhere on the board (slower, for loaded games)
    std::optional<WinInfo> checkWinFull(const Board& board) const;
    
    int getWinLength() const { return winLength_; }
    
private:
    int winLength_;
    
    // Helper to count consecutive pieces in a direction
    int countDirection(const Board& board, int x, int y, int dx, int dy, Player player) const;
    
    // Check specific direction from position
    bool checkDirection(const Board& board, int x, int y, int dx, int dy, Player player, 
                       WinInfo& outWin) const;
};

} // namespace ConnectFour
