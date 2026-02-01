#include "ai/Evaluator.h"
#include <algorithm>
#include <cmath>
#include "game/WinChecker.h"

namespace ConnectFour {

Evaluator::Evaluator(int winLength) : winLength_(winLength) {}

// Set WinChecker for opponent win detection
// (already in header)

int Evaluator::evaluate(const Board& board, Player player) const {
    Player opponent = (player == Player::Player1) ? Player::Player2 : Player::Player1;
    
    // 1. Check if opponent can win next move (immediate threat)
    if (winChecker_) {
        std::vector<int> validMoves = board.getValidMoves();
        for (int move : validMoves) {
            Board temp = board.clone();
            temp.makeMove(move, opponent);
            // Use WinChecker to check if opponent wins after this move
            auto win = winChecker_->checkWin(temp, move);
            if (win && win->winner == opponent) {
                // Opponent can win next move: return huge negative score
                return -999999;
            }
        }
    }

    int score = 0;
    // Evaluate all windows of winLength_ size in all directions
    score += evaluateAllWindows(board, player, opponent);
    // Center control bonus
    score += evaluateCenterControl(board, player);
    return score;
}

int Evaluator::evaluateAllWindows(const Board& board, Player player, Player opponent) const {
    int score = 0;
    int width = board.getWidth();
    int height = board.getHeight();
    
    // Horizontal windows
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x <= width - winLength_; ++x) {
            score += evaluateWindow(board, x, y, 1, 0, player, opponent);
        }
    }
    
    // Vertical windows
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y <= height - winLength_; ++y) {
            score += evaluateWindow(board, x, y, 0, 1, player, opponent);
        }
    }
    
    // Diagonal (bottom-left to top-right)
    for (int x = 0; x <= width - winLength_; ++x) {
        for (int y = 0; y <= height - winLength_; ++y) {
            score += evaluateWindow(board, x, y, 1, 1, player, opponent);
        }
    }
    
    // Diagonal (top-left to bottom-right)
    for (int x = 0; x <= width - winLength_; ++x) {
        for (int y = winLength_ - 1; y < height; ++y) {
            score += evaluateWindow(board, x, y, 1, -1, player, opponent);
        }
    }
    
    return score;
}

int Evaluator::evaluateWindow(const Board& board, int startX, int startY, 
                              int dx, int dy, Player player, Player opponent) const {
    int playerCount = 0;
    int opponentCount = 0;
    int emptyCount = 0;
    
    // Count pieces in this window
    for (int i = 0; i < winLength_; ++i) {
        int x = startX + i * dx;
        int y = startY + i * dy;
        Player cell = board.getCell(x, y);
        
        if (cell == player) {
            playerCount++;
        } else if (cell == opponent) {
            opponentCount++;
        } else {
            emptyCount++;
        }
    }
    
    // Score the window - only useful if it doesn't contain opponent pieces
    int score = 0;
    
    // Player's potential
    if (opponentCount == 0) {
        if (playerCount == winLength_) {
            score += 100000;  // Won
        } else if (playerCount == winLength_ - 1 && emptyCount == 1) {
            score += 5000;  // One away from winning
        } else if (playerCount == winLength_ - 2 && emptyCount == 2) {
            score += 500;  // Two away - strong position
        } else if (playerCount == 1 && emptyCount == winLength_ - 1) {
            score += 10;  // Starting position
        }
    }
    
    // Opponent's potential (block them!)
    if (playerCount == 0) {
        if (opponentCount == winLength_) {
            score -= 100000;  // Lost
        } else if (opponentCount == winLength_ - 1 && emptyCount == 1) {
            score -= 10000;  // MUST block - prioritize over our own threats
        } else if (opponentCount == winLength_ - 2 && emptyCount == 2) {
            score -= 800;  // Opponent building up
        } else if (opponentCount == 1 && emptyCount == winLength_ - 1) {
            score -= 10;
        }
    }
    
    return score;
}

int Evaluator::evaluateCenterControl(const Board& board, Player player) const {
    int score = 0;
    int centerCol = board.getWidth() / 2;
    
    // Center column is most valuable
    for (int y = 0; y < board.getHeight(); ++y) {
        Player cell = board.getCell(centerCol, y);
        if (cell == player) {
            score += 40;
        } else if (cell != Player::None) {
            score -= 40;
        }
    }
    
    // Adjacent to center is also good
    for (int offset = 1; offset <= 1; ++offset) {
        if (centerCol - offset >= 0) {
            for (int y = 0; y < board.getHeight(); ++y) {
                Player cell = board.getCell(centerCol - offset, y);
                if (cell == player) {
                    score += 20;
                } else if (cell != Player::None) {
                    score -= 20;
                }
            }
        }
        if (centerCol + offset < board.getWidth()) {
            for (int y = 0; y < board.getHeight(); ++y) {
                Player cell = board.getCell(centerCol + offset, y);
                if (cell == player) {
                    score += 20;
                } else if (cell != Player::None) {
                    score -= 20;
                }
            }
        }
    }
    
    return score;
}

int Evaluator::evaluateThreats(const Board& board, Player player) const {
    return 0;  // Handled by evaluateAllWindows
}

int Evaluator::evaluateConnections(const Board& board, Player player) const {
    return 0;  // Handled by evaluateAllWindows
}

int Evaluator::countPotentialWins(const Board& board, int x, int y, int dx, int dy, Player player) const {
    int count = 0;
    
    for (int i = 1; i < winLength_; ++i) {
        int nx = x + dx * i;
        int ny = y + dy * i;
        if (nx < 0 || nx >= board.getWidth() || ny < 0 || ny >= board.getHeight()) break;
        
        Player cell = board.getCell(nx, ny);
        if (cell == player) count++;
        else if (cell != Player::None) return 0;
    }
    
    for (int i = 1; i < winLength_; ++i) {
        int nx = x - dx * i;
        int ny = y - dy * i;
        if (nx < 0 || nx >= board.getWidth() || ny < 0 || ny >= board.getHeight()) break;
        
        Player cell = board.getCell(nx, ny);
        if (cell == player) count++;
        else if (cell != Player::None) return 0;
    }
    
    return count;
}

} // namespace ConnectFour
