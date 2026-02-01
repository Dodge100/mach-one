#include "ai/TranspositionTable.h"
#include <algorithm>

namespace ConnectFour {

TranspositionTable::TranspositionTable(size_t sizeMB) {
    size_t numEntries = (sizeMB * 1024 * 1024) / sizeof(TTEntry);
    // Round to power of 2 for faster modulo
    numEntries = std::max(size_t(1024), numEntries);
    numEntries_ = numEntries;
    
    // Allocate array of TTEntry (default initialized)
    entries_ = std::make_unique<TTEntry[]>(numEntries);
}

TranspositionTable::~TranspositionTable() = default;

void TranspositionTable::store(uint64_t hash, int score, int depth, NodeType type, int bestMove) {
    size_t index = getIndex(hash);
    TTEntry& entry = entries_[index];
    
    // Always replace strategy (simpler and works well for game trees)
    // Could implement depth-preferred replacement for better performance
    entry.hash.store(hash, std::memory_order_relaxed);
    entry.score.store(score, std::memory_order_relaxed);
    entry.depth.store(static_cast<int8_t>(depth), std::memory_order_relaxed);
    entry.type.store(static_cast<uint8_t>(type), std::memory_order_relaxed);
    entry.bestMove.store(static_cast<int8_t>(bestMove), std::memory_order_relaxed);
}

bool TranspositionTable::probe(uint64_t hash, int depth, int alpha, int beta, int& score, int& bestMove) const {
    size_t index = getIndex(hash);
    const TTEntry& entry = entries_[index];
    
    // Check if hash matches
    uint64_t storedHash = entry.hash.load(std::memory_order_relaxed);
    if (storedHash != hash) {
        return false;
    }
    
    // Check if depth is sufficient
    int storedDepth = entry.depth.load(std::memory_order_relaxed);
    if (storedDepth < depth) {
        // Still get best move for ordering even if depth insufficient
        bestMove = entry.bestMove.load(std::memory_order_relaxed);
        return false;
    }
    
    int storedScore = entry.score.load(std::memory_order_relaxed);
    NodeType storedType = static_cast<NodeType>(entry.type.load(std::memory_order_relaxed));
    bestMove = entry.bestMove.load(std::memory_order_relaxed);
    
    // Check if stored score is useful
    switch (storedType) {
        case NodeType::Exact:
            score = storedScore;
            return true;
            
        case NodeType::LowerBound:
            if (storedScore >= beta) {
                score = storedScore;
                return true;
            }
            break;
            
        case NodeType::UpperBound:
            if (storedScore <= alpha) {
                score = storedScore;
                return true;
            }
            break;
    }
    
    return false;
}

int TranspositionTable::getBestMove(uint64_t hash) const {
    size_t index = getIndex(hash);
    const TTEntry& entry = entries_[index];
    
    if (entry.hash.load(std::memory_order_relaxed) == hash) {
        return entry.bestMove.load(std::memory_order_relaxed);
    }
    
    return -1;
}

void TranspositionTable::clear() {
    for (size_t i = 0; i < numEntries_; ++i) {
        entries_[i].hash.store(0, std::memory_order_relaxed);
        entries_[i].score.store(0, std::memory_order_relaxed);
        entries_[i].depth.store(0, std::memory_order_relaxed);
        entries_[i].type.store(0, std::memory_order_relaxed);
        entries_[i].bestMove.store(-1, std::memory_order_relaxed);
    }
}

} // namespace ConnectFour
