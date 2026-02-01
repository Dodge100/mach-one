#include "game/Board.h"
#include <random>
#include <stdexcept>

namespace ConnectFour {

std::vector<std::vector<uint64_t>> Board::zobristTable_;

Board::Board(int width, int height)
    : width_(width)
    , height_(height)
    , cells_(width * height, Player::None)
    , columnHeights_(width, 0)
    , hash_(0) {
    
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Board dimensions must be positive");
    }
    
    // Initialize Zobrist table if not done
    if (zobristTable_.empty()) {
        initZobristTable(20, 20);  // Support up to 20x20 boards
    }
}

void Board::initZobristTable(int maxWidth, int maxHeight) {
    std::mt19937_64 rng(12345);  // Fixed seed for consistency
    std::uniform_int_distribution<uint64_t> dist;
    
    zobristTable_.resize(maxWidth * maxHeight);
    for (auto& cell : zobristTable_) {
        cell.resize(3);  // None, Player1, Player2
        for (auto& val : cell) {
            val = dist(rng);
        }
    }
}

bool Board::makeMove(int column, Player player) {
    if (column < 0 || column >= width_) {
        return false;
    }
    
    if (isColumnFull(column)) {
        return false;
    }
    
    if (player == Player::None) {
        return false;
    }
    
    int row = columnHeights_[column];
    cells_[getIndex(column, row)] = player;
    columnHeights_[column]++;
    updateHash(column, row, player);
    
    return true;
}

void Board::undoMove(int column) {
    if (column < 0 || column >= width_) {
        return;
    }
    
    if (columnHeights_[column] == 0) {
        return;
    }
    
    columnHeights_[column]--;
    int row = columnHeights_[column];
    Player player = cells_[getIndex(column, row)];
    
    updateHash(column, row, player);
    cells_[getIndex(column, row)] = Player::None;
}

Player Board::getCell(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return Player::None;
    }
    return cells_[getIndex(x, y)];
}

int Board::getColumnHeight(int column) const {
    if (column < 0 || column >= width_) {
        return 0;
    }
    return columnHeights_[column];
}

bool Board::isColumnFull(int column) const {
    if (column < 0 || column >= width_) {
        return true;
    }
    return columnHeights_[column] >= height_;
}

bool Board::isFull() const {
    for (int col = 0; col < width_; ++col) {
        if (!isColumnFull(col)) {
            return false;
        }
    }
    return true;
}

std::vector<int> Board::getValidMoves() const {
    std::vector<int> moves;
    moves.reserve(width_);
    
    // Center-first ordering for better alpha-beta pruning
    int center = width_ / 2;
    for (int offset = 0; offset < width_; ++offset) {
        int col = (offset % 2 == 0) ? center + offset / 2 : center - (offset + 1) / 2;
        if (col >= 0 && col < width_ && !isColumnFull(col)) {
            moves.push_back(col);
        }
    }
    
    return moves;
}

Board Board::clone() const {
    Board copy(width_, height_);
    copy.cells_ = cells_;
    copy.columnHeights_ = columnHeights_;
    copy.hash_ = hash_;
    return copy;
}

bool Board::operator==(const Board& other) const {
    return width_ == other.width_ 
        && height_ == other.height_ 
        && cells_ == other.cells_;
}

void Board::clear() {
    std::fill(cells_.begin(), cells_.end(), Player::None);
    std::fill(columnHeights_.begin(), columnHeights_.end(), 0);
    hash_ = 0;
}

void Board::updateHash(int x, int y, Player player) {
    int index = getIndex(x, y);
    hash_ ^= zobristTable_[index][static_cast<int>(player)];
}

} // namespace ConnectFour
