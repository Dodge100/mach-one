#pragma once

#include "Board.h"
#include "WinChecker.h"
#include <vector>
#include <memory>

namespace ConnectFour {

enum class PlayerType {
    Human,
    AI
};

enum class GameStatus {
    InProgress,
    Player1Won,
    Player2Won,
    Draw
};

struct GameConfig {
    int boardWidth = 7;
    int boardHeight = 6;
    int winLength = 4;
    int aiDepth = 8;
    PlayerType player1Type = PlayerType::Human;
    PlayerType player2Type = PlayerType::AI;
};

class GameState {
public:
    GameState(const GameConfig& config);
    
    // Game operations
    bool makeMove(int column);
    bool undoMove();
    
    // Getters
    const Board& getBoard() const { return board_; }
    Player getCurrentPlayer() const { return currentPlayer_; }
    GameStatus getStatus() const { return status_; }
    const std::vector<int>& getMoveHistory() const { return moveHistory_; }
    const GameConfig& getConfig() const { return config_; }
    const WinChecker& getWinChecker() const { return winChecker_; }
    
    // Check if current player is AI
    bool isCurrentPlayerAI() const;
    
    // Get valid moves
    std::vector<int> getValidMoves() const { return board_.getValidMoves(); }
    
    // Reset to initial state
    void reset();
    
    // Load from move history (for replays)
    bool loadFromMoves(const std::vector<int>& moves);
    
private:
    Board board_;
    WinChecker winChecker_;
    Player currentPlayer_;
    GameStatus status_;
    std::vector<int> moveHistory_;
    GameConfig config_;
    
    void switchPlayer();
    void updateGameStatus(int lastColumn);
};

} // namespace ConnectFour
