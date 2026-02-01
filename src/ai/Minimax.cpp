#include "ai/Minimax.h"
#include <algorithm>
#include <limits>
#include <chrono>

namespace ConnectFour {

constexpr int WIN_SCORE = 1000000;
constexpr int LOSS_SCORE = -1000000;

Minimax::Minimax(int winLength, int maxDepth)
    : winChecker_(winLength)
    , maxDepth_(maxDepth)
    , evaluator_(winLength)
    , tt_(256)  // 256 MB transposition table
    , nodesSearched_(0) {
    evaluator_.setWinChecker(&winChecker_);
}

SearchResult Minimax::findBestMove(const Board& board, Player player) {
    nodesSearched_ = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<int> validMoves = board.getValidMoves();
    if (validMoves.empty()) {
        return {0, -1, 0, 0.0};
    }
    
    if (validMoves.size() == 1) {
        return {0, validMoves[0], 1, 0.0};
    }
    
    // Order moves for better pruning
    orderMoves(validMoves, board.getHash(), board, player);
    
    int bestMove = validMoves[0];
    int bestScore = std::numeric_limits<int>::min();
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();
    
    Board tempBoard = board.clone();
    
    for (int move : validMoves) {
        tempBoard.makeMove(move, player);
        
        Player opponent = (player == Player::Player1) ? Player::Player2 : Player::Player1;
        int score = -negamax(tempBoard, opponent, maxDepth_ - 1, -beta, -alpha);
        
        tempBoard.undoMove(move);
        
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        
        alpha = std::max(alpha, score);
    }
    
    // Store result in transposition table
    tt_.store(board.getHash(), bestScore, maxDepth_, NodeType::Exact, bestMove);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double timeSeconds = std::chrono::duration<double>(endTime - startTime).count();
    
    return {bestScore, bestMove, nodesSearched_, timeSeconds};
}

int Minimax::negamax(Board& board, Player player, int depth, int alpha, int beta) {
    nodesSearched_++;
    
    uint64_t hash = board.getHash();
    
    // Probe transposition table
    int ttScore, ttMove;
    if (tt_.probe(hash, depth, alpha, beta, ttScore, ttMove)) {
        return ttScore;
    }
    
    // Check for terminal positions at depth 0
    if (depth == 0) {
        int eval = evaluator_.evaluate(board, player);
        tt_.store(hash, eval, 0, NodeType::Exact, -1);
        return eval;
    }
    
    std::vector<int> validMoves = board.getValidMoves();
    
    // No valid moves = draw
    if (validMoves.empty()) {
        tt_.store(hash, 0, depth, NodeType::Exact, -1);
        return 0;
    }
    
    // Order moves
    orderMoves(validMoves, hash, board, player);
    
    int bestScore = std::numeric_limits<int>::min();
    int bestMove = validMoves[0];
    NodeType nodeType = NodeType::UpperBound;
    
    for (int move : validMoves) {
        board.makeMove(move, player);
        
        // Check for immediate win
        int score;
        if (isTerminal(board, move, score, player)) {
            board.undoMove(move);
            // Store and return immediately for wins
            tt_.store(hash, score, depth, NodeType::Exact, move);
            return score;
        }
        
        Player opponent = (player == Player::Player1) ? Player::Player2 : Player::Player1;
        score = -negamax(board, opponent, depth - 1, -beta, -alpha);
        
        board.undoMove(move);
        
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        
        alpha = std::max(alpha, score);
        
        if (alpha >= beta) {
            nodeType = NodeType::LowerBound;
            break;  // Beta cutoff
        }
        
        if (score > alpha) {
            nodeType = NodeType::Exact;
        }
    }
    
    tt_.store(hash, bestScore, depth, nodeType, bestMove);
    return bestScore;
}

bool Minimax::isTerminal(const Board& board, int lastMove, int& score, Player player) {
    auto winInfo = winChecker_.checkWin(board, lastMove);
    if (winInfo) {
        if (winInfo->winner == player) {
            score = WIN_SCORE;
        } else {
            score = LOSS_SCORE;
        }
        return true;
    }
    
    if (board.isFull()) {
        score = 0;
        return true;
    }
    
    return false;
}

void Minimax::orderMoves(std::vector<int>& moves, uint64_t hash, const Board& board, Player player) {
    // Get best move from transposition table
    int ttBestMove = tt_.getBestMove(hash);
    
    if (ttBestMove != -1) {
        // Move TT best move to front
        auto it = std::find(moves.begin(), moves.end(), ttBestMove);
        if (it != moves.end()) {
            std::iter_swap(moves.begin(), it);
        }
    }
    
    // Moves are already center-ordered from Board::getValidMoves()
}

} // namespace ConnectFour
