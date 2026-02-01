#pragma once

#include <atomic>
#include <vector>
#include <memory>
#include <cstdint>

namespace ConnectFour {

enum class NodeType : uint8_t {
    Exact,
    LowerBound,
    UpperBound
};

struct TTEntry {
    std::atomic<uint64_t> hash{0};
    std::atomic<int32_t> score{0};
    std::atomic<int8_t> depth{0};
    std::atomic<uint8_t> type{0};  // NodeType
    std::atomic<int8_t> bestMove{-1};
    
    // Padding to 64 bytes (cache line size) to avoid false sharing
    char padding[64 - sizeof(hash) - sizeof(score) - sizeof(depth) - sizeof(type) - sizeof(bestMove)];
    
    // Default constructor
    TTEntry() = default;
    
    // Delete copy/move constructors since atomics aren't copyable
    TTEntry(const TTEntry&) = delete;
    TTEntry& operator=(const TTEntry&) = delete;
    TTEntry(TTEntry&&) = delete;
    TTEntry& operator=(TTEntry&&) = delete;
};

static_assert(sizeof(TTEntry) == 64, "TTEntry must be 64 bytes for cache line alignment");

class TranspositionTable {
public:
    TranspositionTable(size_t sizeMB = 256);
    ~TranspositionTable();
    
    // Store position evaluation
    void store(uint64_t hash, int score, int depth, NodeType type, int bestMove);
    
    // Probe for stored evaluation
    bool probe(uint64_t hash, int depth, int alpha, int beta, int& score, int& bestMove) const;
    
    // Get best move if available (for move ordering)
    int getBestMove(uint64_t hash) const;
    
    // Clear table
    void clear();
    
    // Statistics
    size_t getSize() const { return numEntries_; }
    
private:
    std::unique_ptr<TTEntry[]> entries_;  // Use array instead of vector
    size_t numEntries_;
    
    size_t getIndex(uint64_t hash) const {
        return hash % numEntries_;
    }
};

} // namespace ConnectFour
