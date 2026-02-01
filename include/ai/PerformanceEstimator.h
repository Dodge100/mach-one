#pragma once

#include "game/Board.h"
#include <chrono>

namespace ConnectFour {

class PerformanceEstimator {
public:
    PerformanceEstimator();
    
    // Calibrate by measuring nodes per second on a sample position
    void calibrate(const Board& board, int depth);
    
    // Estimate search time for given configuration
    double estimateSearchTime(int boardWidth, int depth) const;
    
    // Update estimate based on actual search results
    void updateEstimate(int nodesSearched, double actualTime);
    
    // Get warning level for estimated time
    enum class WarningLevel {
        None,        // < 3s
        Caution,     // 3-10s
        Warning,     // 10-30s
        Critical     // > 30s
    };
    
    WarningLevel getWarningLevel(double estimatedSeconds) const;
    
    bool isCalibrated() const { return calibrated_; }
    
private:
    bool calibrated_;
    double nodesPerSecond_;
    double correctionFactor_;
    
    // Estimate nodes for a given depth and branching factor
    int64_t estimateNodes(int branchingFactor, int depth) const;
};

} // namespace ConnectFour
