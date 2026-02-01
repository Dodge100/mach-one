#pragma once

#include "game/GameState.h"
#include "replay/GameHistory.h"
#include <string>
#include <optional>

namespace ConnectFour {

class GameSerializer {
public:
    // Save game to JSON file
    static bool saveGame(const GameState& gameState, const std::string& filepath);
    
    // Save game history with analysis to JSON file
    static bool saveGameWithAnalysis(const GameHistory& history, const std::string& filepath);
    
    // Load game from JSON file
    static std::optional<GameState> loadGame(const std::string& filepath);
    
    // Load game history from JSON file
    static std::optional<GameHistory> loadGameHistory(const std::string& filepath);
    
    // Get platform-specific save directory
    static std::string getSaveDirectory();
    
    // Get autosave path
    static std::string getAutosavePath();
    
    // Generate timestamped filename
    static std::string generateFilename(const std::string& prefix = "game");
    
private:
    static std::string getAppDataDirectory();
};

} // namespace ConnectFour
