#include "replay/PositionAnalyzer.h"
#include <algorithm>

namespace ConnectFour {

PositionAnalyzer::PositionAnalyzer(GameHistory& history, int analysisDepth)
    : history_(history)
    , analysisDepth_(analysisDepth) {
    
    aiEngine_ = std::make_unique<ParallelMinimax>(history_.getConfig().winLength, analysisDepth);
}

PositionAnalyzer::~PositionAnalyzer() {
    stopBackgroundAnalysis();
}

void PositionAnalyzer::startBackgroundAnalysis() {
    if (analysisThread_ && analysisThread_->joinable()) {
        return;  // Already running
    }
    
    stopAnalysis_ = false;
    analyzedPositions_ = 0;
    
    analysisThread_ = std::make_unique<std::thread>(&PositionAnalyzer::backgroundAnalysisWorker, this);
}

void PositionAnalyzer::stopBackgroundAnalysis() {
    if (analysisThread_) {
        stopAnalysis_ = true;
        if (analysisThread_->joinable()) {
            analysisThread_->join();
        }
        analysisThread_.reset();
    }
}

MoveAnalysis PositionAnalyzer::analyzePosition(size_t moveIndex) {
    MoveAnalysis analysis;
    
    if (moveIndex == 0 || moveIndex >= history_.getAllPositions().size()) {
        // Can't analyze initial position or invalid index
        analysis.evaluation = 0;
        analysis.bestMove = -1;
        analysis.actualMove = -1;
        analysis.evaluationDelta = 0;
        analysis.quality = MoveQuality::Good;
        return analysis;
    }
    
    // Get the position before the move was made
    const auto& prevPos = history_.getPosition(moveIndex - 1);
    const auto& currentPos = history_.getPosition(moveIndex);
    
    // Analyze the position before the move
    auto result = aiEngine_->findBestMove(prevPos.board, prevPos.player);
    
    analysis.bestMove = result.bestMove;
    analysis.actualMove = currentPos.moveColumn;
    
    // Get evaluation of the best move
    Board tempBoard = prevPos.board.clone();
    tempBoard.makeMove(result.bestMove, prevPos.player);
    Player opponent = (prevPos.player == Player::Player1) ? Player::Player2 : Player::Player1;
    auto bestEval = aiEngine_->findBestMove(tempBoard, opponent);
    int bestScore = -bestEval.score;
    
    // Get evaluation of the actual move
    Board actualBoard = prevPos.board.clone();
    actualBoard.makeMove(analysis.actualMove, prevPos.player);
    auto actualEval = aiEngine_->findBestMove(actualBoard, opponent);
    int actualScore = -actualEval.score;
    
    analysis.evaluation = actualScore;
    analysis.evaluationDelta = bestScore - actualScore;
    analysis.quality = classifyMove(analysis.evaluationDelta);
    
    // Cache the analysis
    std::lock_guard<std::mutex> lock(analysisMutex_);
    history_.setPositionAnalysis(moveIndex - 1, bestScore, result.bestMove);
    
    return analysis;
}

bool PositionAnalyzer::isAnalysisComplete() const {
    size_t totalPositions = history_.getAllPositions().size();
    if (totalPositions <= 1) return true;  // Nothing to analyze
    return analyzedPositions_ >= (totalPositions - 1);
}

float PositionAnalyzer::getAnalysisProgress() const {
    size_t totalPositions = history_.getAllPositions().size();
    if (totalPositions <= 1) return 1.0f;
    return static_cast<float>(analyzedPositions_) / static_cast<float>(totalPositions - 1);
}

bool PositionAnalyzer::getMoveAnalysis(size_t moveIndex, MoveAnalysis& outAnalysis) {
    if (moveIndex == 0 || moveIndex >= history_.getAllPositions().size()) {
        return false;
    }
    
    const auto& prevPos = history_.getPosition(moveIndex - 1);
    const auto& currentPos = history_.getPosition(moveIndex);
    
    if (!prevPos.analyzed) {
        return false;
    }
    
    outAnalysis.evaluation = prevPos.evaluation;
    outAnalysis.bestMove = prevPos.bestMove;
    outAnalysis.actualMove = currentPos.moveColumn;
    
    // Calculate delta (need to evaluate actual move if not cached)
    // For simplicity, recalculate if needed
    if (outAnalysis.actualMove == outAnalysis.bestMove) {
        outAnalysis.evaluationDelta = 0;
    } else {
        // Would need to evaluate actual move - simplified for now
        outAnalysis.evaluationDelta = 0;  // TODO: Cache actual move evaluations
    }
    
    outAnalysis.quality = classifyMove(outAnalysis.evaluationDelta);
    
    return true;
}

void PositionAnalyzer::backgroundAnalysisWorker() {
    const auto& positions = history_.getAllPositions();
    
    // Start from position 1 (skip initial empty board)
    for (size_t i = 1; i < positions.size() && !stopAnalysis_; ++i) {
        if (positions[i - 1].analyzed) {
            analyzedPositions_++;
            continue;  // Already analyzed
        }
        
        // Analyze this position
        const auto& pos = positions[i - 1];
        auto result = aiEngine_->findBestMove(pos.board, pos.player);
        
        // Store analysis
        {
            std::lock_guard<std::mutex> lock(analysisMutex_);
            history_.setPositionAnalysis(i - 1, result.score, result.bestMove);
        }
        
        analyzedPositions_++;
    }
}

MoveQuality PositionAnalyzer::classifyMove(int evaluationDelta) const {
    int absDelta = std::abs(evaluationDelta);
    
    if (absDelta <= 10) {
        return MoveQuality::Excellent;
    } else if (absDelta <= 30) {
        return MoveQuality::Good;
    } else if (absDelta <= 60) {
        return MoveQuality::Inaccuracy;
    } else if (absDelta <= 120) {
        return MoveQuality::Mistake;
    } else {
        return MoveQuality::Blunder;
    }
}

} // namespace ConnectFour
