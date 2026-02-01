#include "game/GameState.h"
#include <stdexcept>

namespace ConnectFour {

GameState::GameState(const GameConfig& config)
    : board_(config.boardWidth, config.boardHeight)
    , winChecker_(config.winLength)
    , currentPlayer_(Player::Player1)
    , status_(GameStatus::InProgress)
    , config_(config) {
    
    // Validate configuration
    if (config.winLength > std::min(config.boardWidth, config.boardHeight)) {
        throw std::invalid_argument("Win length cannot exceed board dimensions");
    }
}

bool GameState::makeMove(int column) {
    if (status_ != GameStatus::InProgress) {
        return false;
    }
    
    if (!board_.makeMove(column, currentPlayer_)) {
        return false;
    }
    
    moveHistory_.push_back(column);
    updateGameStatus(column);
    
    if (status_ == GameStatus::InProgress) {
        switchPlayer();
    }
    
    return true;
}

bool GameState::undoMove() {
    if (moveHistory_.empty()) {
        return false;
    }
    
    int lastColumn = moveHistory_.back();
    moveHistory_.pop_back();
    
    board_.undoMove(lastColumn);
    
    // Reset status if game was over
    status_ = GameStatus::InProgress;
    
    // Switch back to previous player
    currentPlayer_ = (currentPlayer_ == Player::Player1) ? Player::Player2 : Player::Player1;
    
    return true;
}

bool GameState::isCurrentPlayerAI() const {
    if (currentPlayer_ == Player::Player1) {
        return config_.player1Type == PlayerType::AI;
    } else {
        return config_.player2Type == PlayerType::AI;
    }
}

void GameState::reset() {
    board_.clear();
    moveHistory_.clear();
    currentPlayer_ = Player::Player1;
    status_ = GameStatus::InProgress;
}

bool GameState::loadFromMoves(const std::vector<int>& moves) {
    reset();
    
    for (int column : moves) {
        if (!makeMove(column)) {
            reset();
            return false;
        }
    }
    
    return true;
}

void GameState::switchPlayer() {
    currentPlayer_ = (currentPlayer_ == Player::Player1) ? Player::Player2 : Player::Player1;
}

void GameState::updateGameStatus(int lastColumn) {
    // Check for win
    auto winInfo = winChecker_.checkWin(board_, lastColumn);
    if (winInfo) {
        status_ = (winInfo->winner == Player::Player1) ? GameStatus::Player1Won : GameStatus::Player2Won;
        return;
    }
    
    // Check for draw
    if (board_.isFull()) {
        status_ = GameStatus::Draw;
        return;
    }
    
    status_ = GameStatus::InProgress;
}

} // namespace ConnectFour
