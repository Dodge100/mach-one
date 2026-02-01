#include "game/WinChecker.h"
#include <algorithm>

namespace ConnectFour {

WinChecker::WinChecker(int winLength) : winLength_(winLength) {
    if (winLength < 3) {
        winLength_ = 3;  // Minimum sensible win length
    }
}

std::optional<WinInfo> WinChecker::checkWin(const Board& board, int lastColumn) const {
    if (lastColumn < 0 || lastColumn >= board.getWidth()) {
        return std::nullopt;
    }
    
    int lastRow = board.getColumnHeight(lastColumn) - 1;
    if (lastRow < 0) {
        return std::nullopt;
    }
    
    Player player = board.getCell(lastColumn, lastRow);
    if (player == Player::None) {
        return std::nullopt;
    }
    
    WinInfo win;
    win.winner = player;
    
    // Check horizontal
    if (checkDirection(board, lastColumn, lastRow, 1, 0, player, win)) {
        win.direction = WinInfo::Horizontal;
        return win;
    }
    
    // Check vertical
    if (checkDirection(board, lastColumn, lastRow, 0, 1, player, win)) {
        win.direction = WinInfo::Vertical;
        return win;
    }
    
    // Check diagonal up-right
    if (checkDirection(board, lastColumn, lastRow, 1, 1, player, win)) {
        win.direction = WinInfo::DiagonalUp;
        return win;
    }
    
    // Check diagonal down-right
    if (checkDirection(board, lastColumn, lastRow, 1, -1, player, win)) {
        win.direction = WinInfo::DiagonalDown;
        return win;
    }
    
    return std::nullopt;
}

std::optional<WinInfo> WinChecker::checkWinFull(const Board& board) const {
    // Slower full-board scan for loaded games
    for (int y = 0; y < board.getHeight(); ++y) {
        for (int x = 0; x < board.getWidth(); ++x) {
            Player player = board.getCell(x, y);
            if (player == Player::None) continue;
            
            WinInfo win;
            win.winner = player;
            
            // Only check right and down directions to avoid duplicates
            if (checkDirection(board, x, y, 1, 0, player, win)) {
                win.direction = WinInfo::Horizontal;
                return win;
            }
            if (checkDirection(board, x, y, 0, 1, player, win)) {
                win.direction = WinInfo::Vertical;
                return win;
            }
            if (checkDirection(board, x, y, 1, 1, player, win)) {
                win.direction = WinInfo::DiagonalUp;
                return win;
            }
            if (checkDirection(board, x, y, 1, -1, player, win)) {
                win.direction = WinInfo::DiagonalDown;
                return win;
            }
        }
    }
    
    return std::nullopt;
}

int WinChecker::countDirection(const Board& board, int x, int y, int dx, int dy, Player player) const {
    int count = 0;
    int curX = x;
    int curY = y;
    
    while (curX >= 0 && curX < board.getWidth() && 
           curY >= 0 && curY < board.getHeight() &&
           board.getCell(curX, curY) == player) {
        count++;
        curX += dx;
        curY += dy;
    }
    
    return count;
}

bool WinChecker::checkDirection(const Board& board, int x, int y, int dx, int dy, 
                                Player player, WinInfo& outWin) const {
    // Count in both directions
    int countForward = countDirection(board, x, y, dx, dy, player);
    int countBackward = countDirection(board, x, y, -dx, -dy, player);
    
    // Total count (subtract 1 because center position counted twice)
    int total = countForward + countBackward - 1;
    
    if (total >= winLength_) {
        // Calculate start and end positions
        outWin.startX = x - dx * (countBackward - 1);
        outWin.startY = y - dy * (countBackward - 1);
        outWin.endX = x + dx * (countForward - 1);
        outWin.endY = y + dy * (countForward - 1);
        return true;
    }
    
    return false;
}

} // namespace ConnectFour
