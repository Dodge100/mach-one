#include "replay/GameHistory.h"
#include <stdexcept>

namespace ConnectFour {

GameHistory::GameHistory(const GameConfig& config)
    : currentIndex_(0)
    , config_(config) {
}

void GameHistory::loadFromGameState(const GameState& gameState) {
    positions_.clear();
    currentIndex_ = 0;
    
    // Create initial empty position
    Board initialBoard(config_.boardWidth, config_.boardHeight);
    HistoricalPosition initialPos{
        initialBoard,  // board
        Player::Player1,  // player
        -1,  // moveColumn
        0,  // evaluation
        -1,  // bestMove
        false  // analyzed
    };
    positions_.push_back(std::move(initialPos));
    
    // Replay all moves
    Board board(config_.boardWidth, config_.boardHeight);
    Player player = Player::Player1;
    
    const auto& moves = gameState.getMoveHistory();
    for (int move : moves) {
        board.makeMove(move, player);
        
        HistoricalPosition pos{
            board.clone(),  // board
            player,  // player
            move,  // moveColumn
            0,  // evaluation
            -1,  // bestMove
            false  // analyzed
        };
        positions_.push_back(std::move(pos));
        
        player = (player == Player::Player1) ? Player::Player2 : Player::Player1;
    }
    
    currentIndex_ = positions_.size() - 1;
}

bool GameHistory::stepForward() {
    if (currentIndex_ >= positions_.size() - 1) {
        return false;
    }
    currentIndex_++;
    return true;
}

bool GameHistory::stepBackward() {
    if (currentIndex_ == 0) {
        return false;
    }
    currentIndex_--;
    return true;
}

bool GameHistory::jumpToMove(size_t moveIndex) {
    if (moveIndex >= positions_.size()) {
        return false;
    }
    currentIndex_ = moveIndex;
    return true;
}

bool GameHistory::jumpToStart() {
    currentIndex_ = 0;
    return true;
}

bool GameHistory::jumpToEnd() {
    if (positions_.empty()) {
        return false;
    }
    currentIndex_ = positions_.size() - 1;
    return true;
}

const Board& GameHistory::getCurrentBoard() const {
    if (currentIndex_ >= positions_.size()) {
        throw std::out_of_range("Invalid position index");
    }
    return positions_[currentIndex_].board;
}

Player GameHistory::getCurrentPlayer() const {
    if (currentIndex_ >= positions_.size()) {
        return Player::None;
    }
    return positions_[currentIndex_].player;
}

const HistoricalPosition& GameHistory::getPosition(size_t index) const {
    if (index >= positions_.size()) {
        throw std::out_of_range("Invalid position index");
    }
    return positions_[index];
}

bool GameHistory::isPositionAnalyzed(size_t index) const {
    if (index >= positions_.size()) {
        return false;
    }
    return positions_[index].analyzed;
}

void GameHistory::setPositionAnalysis(size_t index, int evaluation, int bestMove) {
    if (index >= positions_.size()) {
        return;
    }
    positions_[index].evaluation = evaluation;
    positions_[index].bestMove = bestMove;
    positions_[index].analyzed = true;
}

} // namespace ConnectFour
