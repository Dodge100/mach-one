#pragma once

#include "game/Board.h"
#include "game/WinChecker.h"
#include "Evaluator.h"
#include "TranspositionTable.h"
#include <memory>

namespace ConnectFour {

struct SearchResult {
    int score;
    int bestMove;
    int nodesSearched;
    double timeSeconds;
};

class Minimax {
public:
    Minimax(int winLength, int maxDepth);
    
    // Find best move using minimax with alpha-beta pruning
    SearchResult findBestMove(const Board& board, Player player);
    
    // Set maximum search depth
    void setMaxDepth(int depth) { maxDepth_ = depth; }
    int getMaxDepth() const { return maxDepth_; }
    
    // Get transposition table for external access (used by parallel search)
    TranspositionTable& getTranspositionTable() { return tt_; }
    
private:
    WinChecker winChecker_;
    int maxDepth_;
    Evaluator evaluator_;
    TranspositionTable tt_;
    int nodesSearched_;
    
    // Negamax with alpha-beta pruning
    int negamax(Board& board, Player player, int depth, int alpha, int beta);
    
    // Check if position is terminal (win/draw)
    bool isTerminal(const Board& board, int lastMove, int& score, Player player);
    
    // Order moves for better alpha-beta pruning
    void orderMoves(std::vector<int>& moves, uint64_t hash, const Board& board, Player player);
};

} // namespace ConnectFour
