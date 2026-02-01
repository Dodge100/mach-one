#pragma once

#include "GameHistory.h"
#include "ai/ParallelMinimax.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>

namespace ConnectFour {

enum class MoveQuality {
    Excellent,   // Within 10 points of best
    Good,        // Within 30 points
    Inaccuracy,  // Within 60 points
    Mistake,     // Within 120 points
    Blunder      // More than 120 points worse
};

struct MoveAnalysis {
    int evaluation;
    int bestMove;
    int actualMove;
    int evaluationDelta;  // Difference from best move
    MoveQuality quality;
};

class PositionAnalyzer {
public:
    PositionAnalyzer(GameHistory& history, int analysisDepth = 8);
    ~PositionAnalyzer();
    
    // Start background analysis of all positions
    void startBackgroundAnalysis();
    
    // Stop background analysis
    void stopBackgroundAnalysis();
    
    // Analyze a specific position (blocking)
    MoveAnalysis analyzePosition(size_t moveIndex);
    
    // Check if background analysis is complete
    bool isAnalysisComplete() const;
    
    // Get progress (0.0 to 1.0)
    float getAnalysisProgress() const;
    
    // Get analysis for a move if available
    bool getMoveAnalysis(size_t moveIndex, MoveAnalysis& outAnalysis);
    
private:
    GameHistory& history_;
    int analysisDepth_;
    std::unique_ptr<ParallelMinimax> aiEngine_;
    
    // Background analysis
    std::unique_ptr<std::thread> analysisThread_;
    std::atomic<bool> stopAnalysis_{false};
    std::atomic<size_t> analyzedPositions_{0};
    std::mutex analysisMutex_;
    
    void backgroundAnalysisWorker();
    MoveQuality classifyMove(int evaluationDelta) const;
};

} // namespace ConnectFour
