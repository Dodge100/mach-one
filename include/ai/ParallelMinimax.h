#pragma once

#include "Minimax.h"
#include "ThreadPool.h"
#include <memory>

namespace ConnectFour {

class ParallelMinimax {
public:
    ParallelMinimax(int winLength, int maxDepth, size_t numThreads = 0);
    
    // Find best move using parallel search
    SearchResult findBestMove(const Board& board, Player player);
    
    void setMaxDepth(int depth) { maxDepth_ = depth; }
    int getMaxDepth() const { return maxDepth_; }
    
private:
    int winLength_;
    int maxDepth_;
    std::unique_ptr<ThreadPool> threadPool_;
    TranspositionTable sharedTT_;  // Shared lock-free TT
    
    // Sequential search for single move (fallback)
    SearchResult sequentialSearch(const Board& board, Player player);
};

} // namespace ConnectFour
