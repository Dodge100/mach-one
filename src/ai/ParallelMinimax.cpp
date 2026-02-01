#include "ai/ParallelMinimax.h"
#include <algorithm>
#include <limits>

namespace ConnectFour {

ParallelMinimax::ParallelMinimax(int winLength, int maxDepth, size_t numThreads)
    : winLength_(winLength)
    , maxDepth_(maxDepth)
    , threadPool_(std::make_unique<ThreadPool>(numThreads))
    , sharedTT_(256) {  // 256 MB shared transposition table
}

SearchResult ParallelMinimax::findBestMove(const Board& board, Player player) {
    std::vector<int> validMoves = board.getValidMoves();
    
    if (validMoves.empty()) {
        return {0, -1, 0, 0.0};
    }
    
    if (validMoves.size() == 1) {
        return {0, validMoves[0], 1, 0.0};
    }
    
    // For shallow depths or few moves, use sequential search
    if (maxDepth_ < 6 || validMoves.size() < 3) {
        return sequentialSearch(board, player);
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Parallel root search: evaluate each move in parallel
    struct MoveScore {
        int move;
        int score;
        int nodes;
    };
    
    std::vector<std::future<MoveScore>> futures;
    futures.reserve(validMoves.size());
    
    Player opponent = (player == Player::Player1) ? Player::Player2 : Player::Player1;
    
    for (int move : validMoves) {
        futures.push_back(threadPool_->submit([this, board, move, player, opponent]() -> MoveScore {
            // Each thread gets its own minimax instance
            Minimax minimax(winLength_, maxDepth_ - 1);
            
            Board tempBoard = board.clone();
            tempBoard.makeMove(move, player);
            
            auto result = minimax.findBestMove(tempBoard, opponent);
            int score = -result.score;  // Negate because we're the opponent
            
            return {move, score, result.nodesSearched};
        }));
    }
    
    // Collect results
    int bestMove = validMoves[0];
    int bestScore = std::numeric_limits<int>::min();
    int totalNodes = 0;
    
    for (size_t i = 0; i < futures.size(); ++i) {
        MoveScore result = futures[i].get();
        totalNodes += result.nodes;
        
        if (result.score > bestScore) {
            bestScore = result.score;
            bestMove = result.move;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double timeSeconds = std::chrono::duration<double>(endTime - startTime).count();
    
    // Store best result in shared TT
    sharedTT_.store(board.getHash(), bestScore, maxDepth_, NodeType::Exact, bestMove);
    
    return {bestScore, bestMove, totalNodes, timeSeconds};
}

SearchResult ParallelMinimax::sequentialSearch(const Board& board, Player player) {
    Minimax minimax(winLength_, maxDepth_);
    return minimax.findBestMove(board, player);
}

} // namespace ConnectFour
