#include "io/GameSerializer.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace ConnectFour {

bool GameSerializer::saveGame(const GameState& gameState, const std::string& filepath) {
    try {
        json j;
        
        j["version"] = "1.0";
        
        const auto& config = gameState.getConfig();
        j["board"]["width"] = config.boardWidth;
        j["board"]["height"] = config.boardHeight;
        j["board"]["winLength"] = config.winLength;
        
        // Save board state as 2D array
        json cells = json::array();
        for (int y = 0; y < config.boardHeight; ++y) {
            json row = json::array();
            for (int x = 0; x < config.boardWidth; ++x) {
                row.push_back(static_cast<int>(gameState.getBoard().getCell(x, y)));
            }
            cells.push_back(row);
        }
        j["board"]["cells"] = cells;
        
        // Game state
        j["gameState"]["currentPlayer"] = static_cast<int>(gameState.getCurrentPlayer());
        j["gameState"]["moveHistory"] = gameState.getMoveHistory();
        
        std::string statusStr;
        switch (gameState.getStatus()) {
            case GameStatus::InProgress: statusStr = "in_progress"; break;
            case GameStatus::Player1Won: statusStr = "player1_won"; break;
            case GameStatus::Player2Won: statusStr = "player2_won"; break;
            case GameStatus::Draw: statusStr = "draw"; break;
        }
        j["gameState"]["status"] = statusStr;
        
        // AI configuration
        j["ai"]["maxDepth"] = config.aiDepth;
        j["ai"]["player1Type"] = (config.player1Type == PlayerType::Human) ? "human" : "ai";
        j["ai"]["player2Type"] = (config.player2Type == PlayerType::Human) ? "human" : "ai";
        
        // Ensure directory exists
        fs::path path(filepath);
        if (path.has_parent_path()) {
            fs::create_directories(path.parent_path());
        }
        
        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2);  // Pretty print with 2-space indentation
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool GameSerializer::saveGameWithAnalysis(const GameHistory& history, const std::string& filepath) {
    try {
        json j;
        
        j["version"] = "1.0";
        
        const auto& config = history.getConfig();
        j["board"]["width"] = config.boardWidth;
        j["board"]["height"] = config.boardHeight;
        j["board"]["winLength"] = config.winLength;
        
        // Save move history with analysis
        json moves = json::array();
        const auto& positions = history.getAllPositions();
        
        for (size_t i = 1; i < positions.size(); ++i) {
            const auto& pos = positions[i];
            json move;
            move["column"] = pos.moveColumn;
            move["player"] = static_cast<int>(pos.player);
            
            if (i > 0 && positions[i - 1].analyzed) {
                move["evaluation"] = positions[i - 1].evaluation;
                move["bestMove"] = positions[i - 1].bestMove;
            }
            
            moves.push_back(move);
        }
        j["moves"] = moves;
        
        // AI configuration
        j["ai"]["maxDepth"] = config.aiDepth;
        j["ai"]["player1Type"] = (config.player1Type == PlayerType::Human) ? "human" : "ai";
        j["ai"]["player2Type"] = (config.player2Type == PlayerType::Human) ? "human" : "ai";
        
        // Ensure directory exists
        fs::path path(filepath);
        if (path.has_parent_path()) {
            fs::create_directories(path.parent_path());
        }
        
        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2);
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

std::optional<GameState> GameSerializer::loadGame(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        json j = json::parse(file);
        
        // Parse configuration
        GameConfig config;
        config.boardWidth = j["board"]["width"];
        config.boardHeight = j["board"]["height"];
        config.winLength = j["board"]["winLength"];
        config.aiDepth = j["ai"]["maxDepth"];
        
        std::string p1Type = j["ai"]["player1Type"];
        std::string p2Type = j["ai"]["player2Type"];
        config.player1Type = (p1Type == "human") ? PlayerType::Human : PlayerType::AI;
        config.player2Type = (p2Type == "human") ? PlayerType::Human : PlayerType::AI;
        
        // Create game state
        GameState gameState(config);
        
        // Load move history
        std::vector<int> moveHistory = j["gameState"]["moveHistory"];
        gameState.loadFromMoves(moveHistory);
        
        return gameState;
        
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<GameHistory> GameSerializer::loadGameHistory(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        json j = json::parse(file);
        
        // Parse configuration
        GameConfig config;
        config.boardWidth = j["board"]["width"];
        config.boardHeight = j["board"]["height"];
        config.winLength = j["board"]["winLength"];
        config.aiDepth = j["ai"]["maxDepth"];
        
        std::string p1Type = j["ai"]["player1Type"];
        std::string p2Type = j["ai"]["player2Type"];
        config.player1Type = (p1Type == "human") ? PlayerType::Human : PlayerType::AI;
        config.player2Type = (p2Type == "human") ? PlayerType::Human : PlayerType::AI;
        
        // Create temporary game state to load moves
        GameState tempGameState(config);
        
        std::vector<int> moves;
        for (const auto& move : j["moves"]) {
            moves.push_back(move["column"]);
        }
        
        tempGameState.loadFromMoves(moves);
        
        // Create history
        GameHistory history(config);
        history.loadFromGameState(tempGameState);
        
        // Load analysis data if available
        size_t moveIndex = 0;
        for (const auto& move : j["moves"]) {
            if (move.contains("evaluation") && move.contains("bestMove")) {
                int eval = move["evaluation"];
                int bestMove = move["bestMove"];
                history.setPositionAnalysis(moveIndex, eval, bestMove);
            }
            moveIndex++;
        }
        
        return history;
        
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::string GameSerializer::getAppDataDirectory() {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\ConnectFour";
    }
    return "ConnectFour";
#elif defined(__APPLE__)
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/Library/Application Support/ConnectFour";
    }
    return "ConnectFour";
#else
    // Linux/Unix - use XDG_DATA_HOME or ~/.local/share
    const char* xdgData = getenv("XDG_DATA_HOME");
    if (xdgData) {
        return std::string(xdgData) + "/connectfour";
    }
    
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/.local/share/connectfour";
    }
    
    return ".connectfour";
#endif
}

std::string GameSerializer::getSaveDirectory() {
#ifdef _WIN32
    const char* docs = getenv("USERPROFILE");
    if (docs) {
        return std::string(docs) + "\\Documents\\ConnectFour";
    }
#else
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/Documents/ConnectFour";
    }
#endif
    return "saves";
}

std::string GameSerializer::getAutosavePath() {
    return getAppDataDirectory() + "/autosave.json";
}

std::string GameSerializer::generateFilename(const std::string& prefix) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << prefix << "_"
       << std::put_time(std::localtime(&time), "%Y-%m-%d_%H%M%S")
       << ".json";
    
    return ss.str();
}

} // namespace ConnectFour
