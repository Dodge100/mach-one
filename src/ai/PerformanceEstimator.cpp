#include "ai/PerformanceEstimator.h"
#include "ai/Minimax.h"
#include "game/WinChecker.h"
#include <cmath>
#include <algorithm>

namespace ConnectFour {

PerformanceEstimator::PerformanceEstimator()
    : calibrated_(false)
    , nodesPerSecond_(100000.0)  // Default estimate
    , correctionFactor_(1.0) {
}

void PerformanceEstimator::calibrate(const Board& board, int depth) {
    // Quick calibration search
    Minimax minimax(4, std::min(depth, 6));  // Cap calibration depth
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = minimax.findBestMove(board, Player::Player1);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double timeSeconds = std::chrono::duration<double>(endTime - startTime).count();
    
    if (timeSeconds > 0.0 && result.nodesSearched > 0) {
        nodesPerSecond_ = result.nodesSearched / timeSeconds;
        calibrated_ = true;
    }
}

double PerformanceEstimator::estimateSearchTime(int boardWidth, int depth) const {
    // Effective branching factor (typically 4-5 for Connect 4)
    int branchingFactor = std::max(3, std::min(boardWidth, 7));
    double effectiveBF = branchingFactor * 0.7;  // Alpha-beta reduces effective BF
    
    int64_t estimatedNodes = estimateNodes(static_cast<int>(effectiveBF), depth);
    double estimatedTime = estimatedNodes / nodesPerSecond_;
    
    return estimatedTime * correctionFactor_;
}

void PerformanceEstimator::updateEstimate(int nodesSearched, double actualTime) {
    if (actualTime > 0.0 && nodesSearched > 0) {
        double actualNPS = nodesSearched / actualTime;
        
        // Moving average to smooth out variations
        if (calibrated_) {
            nodesPerSecond_ = 0.7 * nodesPerSecond_ + 0.3 * actualNPS;
        } else {
            nodesPerSecond_ = actualNPS;
            calibrated_ = true;
        }
    }
}

PerformanceEstimator::WarningLevel PerformanceEstimator::getWarningLevel(double estimatedSeconds) const {
    if (estimatedSeconds > 30.0) {
        return WarningLevel::Critical;
    } else if (estimatedSeconds > 10.0) {
        return WarningLevel::Warning;
    } else if (estimatedSeconds > 3.0) {
        return WarningLevel::Caution;
    }
    return WarningLevel::None;
}

int64_t PerformanceEstimator::estimateNodes(int branchingFactor, int depth) const {
    if (depth <= 0) return 1;
    
    // Geometric series: b^0 + b^1 + b^2 + ... + b^d
    int64_t nodes = 0;
    int64_t power = 1;
    
    for (int d = 0; d <= depth; ++d) {
        nodes += power;
        power *= branchingFactor;
        
        // Prevent overflow
        if (power > 1000000000LL) {
            power = 1000000000LL;
        }
    }
    
    return nodes;
}

} // namespace ConnectFour
