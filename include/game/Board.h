#pragma once

#include <vector>
#include <optional>

namespace ConnectFour {

enum class Player {
    None = 0,
    Player1 = 1,
    Player2 = 2
};

class Board {
public:
    Board(int width, int height);
    
    // Board operations
    bool makeMove(int column, Player player);
    void undoMove(int column);
    
    // Getters
    Player getCell(int x, int y) const;
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    int getColumnHeight(int column) const;
    bool isColumnFull(int column) const;
    bool isFull() const;
    
    // Get list of valid moves
    std::vector<int> getValidMoves() const;
    
    // Copy and comparison
    Board clone() const;
    bool operator==(const Board& other) const;
    
    // Hash for transposition table
    uint64_t getHash() const { return hash_; }
    
    // Reset board
    void clear();
    
private:
    int width_;
    int height_;
    std::vector<Player> cells_;  // Flattened 1D array for cache efficiency
    std::vector<int> columnHeights_;  // Track height of each column
    uint64_t hash_;  // Zobrist hash
    
    // Zobrist hashing
    static std::vector<std::vector<uint64_t>> zobristTable_;
    static void initZobristTable(int maxWidth, int maxHeight);
    
    int getIndex(int x, int y) const { return y * width_ + x; }
    void updateHash(int x, int y, Player player);
};

} // namespace ConnectFour
