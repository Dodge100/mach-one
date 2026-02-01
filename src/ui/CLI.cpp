#include "ui/CLI.h"
#include "io/GameSerializer.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>

namespace ConnectFour {

CLI::CLI() : currentMode_(Mode::MainMenu) {
    // Initialize with default configuration
    GameConfig config;
    gameState_ = std::make_unique<GameState>(config);
    
    aiEngine_ = std::make_unique<ParallelMinimax>(config.winLength, config.aiDepth);
    perfEstimator_ = std::make_unique<PerformanceEstimator>();
}

CLI::~CLI() = default;

void CLI::run() {
    while (true) {
        switch (currentMode_) {
            case Mode::MainMenu:
                showMainMenu();
                break;
            case Mode::Playing:
                playGame();
                break;
            case Mode::Replay:
                replayMode();
                break;
            case Mode::Settings:
                settingsMenu();
                break;
        }
    }
}

void CLI::showMainMenu() {
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* RED = "\033[91m";
    const char* YELLOW = "\033[93m";
    const char* CYAN = "\033[96m";
    const char* DIM = "\033[2m";
    const char* BLUE = "\033[94m";
    
    clearScreen();
    
    // ASCII art title
    std::cout << "\n";
    std::cout << "   " << BLUE << "╔════════════════════════════════════════╗" << RESET << "\n";
    std::cout << "   " << BLUE << "║" << RESET << "                                        " << BLUE << "║" << RESET << "\n";
    std::cout << "   " << BLUE << "║" << RESET << "        " << BOLD << RED << "●" << YELLOW << "●" << RED << "●" << YELLOW << "●" << RESET << "  " << BOLD << "CONNECT FOUR" << RESET << "  " << YELLOW << "●" << RED << "●" << YELLOW << "●" << RED << "●" << RESET << "        " << BLUE << "║" << RESET << "\n";
    std::cout << "   " << BLUE << "║" << RESET << "                                        " << BLUE << "║" << RESET << "\n";
    std::cout << "   " << BLUE << "╚════════════════════════════════════════╝" << RESET << "\n";
    std::cout << "\n";
    
    std::cout << "   " << CYAN << "[1]" << RESET << "  " << BOLD << "New Game" << RESET << "\n";
    std::cout << "   " << CYAN << "[2]" << RESET << "  Load Game\n";
    std::cout << "   " << CYAN << "[3]" << RESET << "  Settings\n";
    std::cout << "   " << CYAN << "[4]" << RESET << "  " << DIM << "Exit" << RESET << "\n";
    std::cout << "\n";
    std::cout << "   " << DIM << "Select option:" << RESET << " ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    handleMainMenuChoice(choice);
}

void CLI::handleMainMenuChoice(int choice) {
    switch (choice) {
        case 1:
            gameState_->reset();
            currentMode_ = Mode::Playing;
            break;
        case 2:
            loadGame();
            break;
        case 3:
            currentMode_ = Mode::Settings;
            break;
        case 4:
            std::cout << "Thanks for playing!\n";
            exit(0);
        default:
            std::cout << "Invalid choice!\n";
            waitForEnter();
    }
}

void CLI::playGame() {
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* DIM = "\033[2m";
    const char* RED = "\033[91m";
    const char* YELLOW = "\033[93m";
    const char* GREEN = "\033[92m";
    const char* CYAN = "\033[96m";
    
    clearScreen();
    renderBoard(gameState_->getBoard());
    showGameStatus();
    
    if (gameState_->getStatus() != GameStatus::InProgress) {
        std::cout << "\n";
        
        // Game over message
        GameStatus status = gameState_->getStatus();
        if (status == GameStatus::Player1Won) {
            std::cout << "   " << RED << "╔════════════════════════════╗" << RESET << "\n";
            std::cout << "   " << RED << "║" << RESET << "   " << BOLD << RED << "●" << RESET << " " << BOLD << "RED WINS!" << RESET << "              " << RED << "║" << RESET << "\n";
            std::cout << "   " << RED << "╚════════════════════════════╝" << RESET << "\n";
        } else if (status == GameStatus::Player2Won) {
            std::cout << "   " << YELLOW << "╔════════════════════════════╗" << RESET << "\n";
            std::cout << "   " << YELLOW << "║" << RESET << "   " << BOLD << YELLOW << "●" << RESET << " " << BOLD << "YELLOW WINS!" << RESET << "           " << YELLOW << "║" << RESET << "\n";
            std::cout << "   " << YELLOW << "╚════════════════════════════╝" << RESET << "\n";
        } else {
            std::cout << "   " << DIM << "╔════════════════════════════╗" << RESET << "\n";
            std::cout << "   " << DIM << "║" << RESET << "   " << BOLD << "IT'S A DRAW!" << RESET << "             " << DIM << "║" << RESET << "\n";
            std::cout << "   " << DIM << "╚════════════════════════════╝" << RESET << "\n";
        }
        
        std::cout << "\n";
        std::cout << "   " << CYAN << "[1]" << RESET << " " << BOLD << "Play Again" << RESET << "\n";
        std::cout << "   " << CYAN << "[2]" << RESET << " Analyze Game\n";
        std::cout << "   " << CYAN << "[3]" << RESET << " Save Game\n";
        std::cout << "   " << CYAN << "[4]" << RESET << " " << DIM << "Main Menu" << RESET << "\n";
        std::cout << "\n   " << DIM << "Choice:" << RESET << " ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                gameState_->reset();
                break;
            case 2:
                gameHistory_ = std::make_unique<GameHistory>(gameState_->getConfig());
                gameHistory_->loadFromGameState(*gameState_);
                analyzer_ = std::make_unique<PositionAnalyzer>(*gameHistory_, 
                    gameState_->getConfig().aiDepth);
                std::cout << "\n   " << DIM << "Starting analysis..." << RESET << "\n";
                analyzer_->startBackgroundAnalysis();
                currentMode_ = Mode::Replay;
                break;
            case 3:
                saveGame();
                break;
            case 4:
                currentMode_ = Mode::MainMenu;
                break;
        }
        return;
    }
    
    if (gameState_->isCurrentPlayerAI()) {
        makeAIMove();
    } else {
        int column = getHumanMove();
        if (column >= 0) {
            gameState_->makeMove(column);
        }
    }
}

void CLI::renderBoard(const Board& board) const {
    // ANSI color codes
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* BLUE_BG = "\033[44m";
    const char* WHITE = "\033[97m";
    const char* RED = "\033[91m";
    const char* YELLOW = "\033[93m";
    const char* CYAN = "\033[96m";
    const char* DIM = "\033[2m";
    
    std::cout << "\n";
    
    // Column numbers with cyan color
    std::cout << "   ";
    for (int x = 0; x < board.getWidth(); ++x) {
        std::cout << CYAN << " " << (x + 1) << "  " << RESET;
    }
    std::cout << "\n";
    
    // Top border with box drawing characters
    std::cout << "   " << BLUE_BG << WHITE << "╔";
    for (int x = 0; x < board.getWidth(); ++x) {
        std::cout << "═══";
        if (x < board.getWidth() - 1) std::cout << "╤";
    }
    std::cout << "╗" << RESET << "\n";
    
    // Board rows (top to bottom)
    for (int y = board.getHeight() - 1; y >= 0; --y) {
        std::cout << "   " << BLUE_BG << WHITE << "║" << RESET;
        for (int x = 0; x < board.getWidth(); ++x) {
            std::cout << getPlayerSymbol(board.getCell(x, y));
            if (x < board.getWidth() - 1) {
                std::cout << BLUE_BG << WHITE << "│" << RESET;
            }
        }
        std::cout << BLUE_BG << WHITE << "║" << RESET << "\n";
        
        // Row separator (except after last row)
        if (y > 0) {
            std::cout << "   " << BLUE_BG << WHITE << "╟";
            for (int x = 0; x < board.getWidth(); ++x) {
                std::cout << "───";
                if (x < board.getWidth() - 1) std::cout << "┼";
            }
            std::cout << "╢" << RESET << "\n";
        }
    }
    
    // Bottom border
    std::cout << "   " << BLUE_BG << WHITE << "╚";
    for (int x = 0; x < board.getWidth(); ++x) {
        std::cout << "═══";
        if (x < board.getWidth() - 1) std::cout << "╧";
    }
    std::cout << "╝" << RESET << "\n";
    
    // Column numbers again at bottom for convenience
    std::cout << "   ";
    for (int x = 0; x < board.getWidth(); ++x) {
        std::cout << CYAN << " " << (x + 1) << "  " << RESET;
    }
    std::cout << "\n";
}

void CLI::showGameStatus() const {
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* DIM = "\033[2m";
    const char* GREEN = "\033[92m";
    const char* CYAN = "\033[96m";
    
    std::cout << "\n";
    
    Player current = gameState_->getCurrentPlayer();
    std::cout << "   " << BOLD << "Turn: " << RESET;
    std::cout << getPlayerSymbol(current);
    std::cout << (current == Player::Player1 ? "(Red)" : "(Yellow)");
    
    if (gameState_->isCurrentPlayerAI()) {
        std::cout << " " << DIM << "[AI]" << RESET;
    }
    std::cout << "\n";
    
    const auto& config = gameState_->getConfig();
    std::cout << "   " << DIM << config.boardWidth << "×" << config.boardHeight 
              << " board • Connect " << config.winLength << " to win" << RESET << "\n";
}

int CLI::getHumanMove() {
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* CYAN = "\033[96m";
    const char* RED = "\033[91m";
    
    std::cout << "\n   " << CYAN << "Drop piece in column (1-" << gameState_->getBoard().getWidth() 
              << ")" << RESET << " or " << CYAN << "0" << RESET << " for menu: ";
    
    int input;
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (input == 0) {
        std::cout << "\n   1. Save Game\n   2. Main Menu\n   3. Continue\n   Choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                saveGame();
                break;
            case 2:
                currentMode_ = Mode::MainMenu;
                return -1;
        }
        return -1;
    }
    
    int column = input - 1;  // Convert to 0-based
    
    if (column < 0 || column >= gameState_->getBoard().getWidth()) {
        std::cout << "   " << RED << "Invalid column!" << RESET << "\n";
        waitForEnter();
        return -1;
    }
    
    if (gameState_->getBoard().isColumnFull(column)) {
        std::cout << "   " << RED << "Column is full!" << RESET << "\n";
        waitForEnter();
        return -1;
    }
    
    return column;
}

void CLI::makeAIMove() {
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* DIM = "\033[2m";
    const char* YELLOW = "\033[93m";
    const char* CYAN = "\033[96m";
    
    std::cout << "\n   " << YELLOW << "●" << RESET << " " << DIM << "AI is thinking..." << RESET << std::flush;
    
    // Show performance warning if needed
    if (!perfEstimator_->isCalibrated()) {
        perfEstimator_->calibrate(gameState_->getBoard(), gameState_->getConfig().aiDepth);
    }
    
    double estimatedTime = perfEstimator_->estimateSearchTime(
        gameState_->getBoard().getWidth(), 
        gameState_->getConfig().aiDepth
    );
    
    showPerformanceWarning(estimatedTime);
    
    auto result = aiEngine_->findBestMove(gameState_->getBoard(), gameState_->getCurrentPlayer());
    
    perfEstimator_->updateEstimate(result.nodesSearched, result.timeSeconds);
    
    std::cout << "\r   " << YELLOW << "●" << RESET << " AI plays column " << BOLD << (result.bestMove + 1) << RESET
              << "  " << DIM << "(" << result.nodesSearched << " positions, " 
              << std::fixed << std::setprecision(2) << result.timeSeconds << "s)" << RESET << "\n";
    
    gameState_->makeMove(result.bestMove);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void CLI::showPerformanceWarning(double estimatedTime) {
    auto level = perfEstimator_->getWarningLevel(estimatedTime);
    
    switch (level) {
        case PerformanceEstimator::WarningLevel::Caution:
            std::cout << "⚠ Caution: Estimated thinking time ~" 
                     << std::fixed << std::setprecision(1) << estimatedTime << "s\n";
            break;
        case PerformanceEstimator::WarningLevel::Warning:
            std::cout << "⚠⚠ Warning: Estimated thinking time ~" 
                     << std::fixed << std::setprecision(1) << estimatedTime << "s\n";
            break;
        case PerformanceEstimator::WarningLevel::Critical:
            std::cout << "⚠⚠⚠ CRITICAL: Estimated thinking time ~" 
                     << std::fixed << std::setprecision(0) << estimatedTime 
                     << "s - Consider reducing depth or board size!\n";
            break;
        default:
            break;
    }
}

void CLI::replayMode() {
    if (!gameHistory_) {
        currentMode_ = Mode::MainMenu;
        return;
    }
    
    clearScreen();
    renderBoard(gameHistory_->getCurrentBoard());
    
    std::cout << "\n=== REPLAY MODE ===\n";
    std::cout << "Move: " << gameHistory_->getCurrentMoveIndex() 
              << " / " << gameHistory_->getTotalMoves() << "\n";
    
    if (analyzer_) {
        float progress = analyzer_->getAnalysisProgress();
        std::cout << "Analysis: " << std::fixed << std::setprecision(0) 
                 << (progress * 100) << "%\n";
        
        if (gameHistory_->getCurrentMoveIndex() > 0) {
            showPositionAnalysis(gameHistory_->getCurrentMoveIndex());
        }
    }
    
    showReplayControls();
    
    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    switch (choice) {
        case 'f':
            gameHistory_->stepForward();
            break;
        case 'b':
            gameHistory_->stepBackward();
            break;
        case 's':
            gameHistory_->jumpToStart();
            break;
        case 'e':
            gameHistory_->jumpToEnd();
            break;
        case 'm':
            currentMode_ = Mode::MainMenu;
            break;
        default:
            break;
    }
}

void CLI::showReplayControls() const {
    std::cout << "\nControls: (f)orward | (b)ackward | (s)tart | (e)nd | (m)ain menu\n";
    std::cout << "Choice: ";
}

void CLI::showPositionAnalysis(size_t moveIndex) {
    if (!analyzer_) return;
    
    MoveAnalysis analysis;
    if (analyzer_->getMoveAnalysis(moveIndex, analysis)) {
        std::cout << "\n--- Position Analysis ---\n";
        std::cout << "Evaluation: " << std::showpos << analysis.evaluation << std::noshowpos << "\n";
        std::cout << "Best move: Column " << analysis.bestMove << "\n";
        std::cout << "Actual move: Column " << analysis.actualMove << "\n";
        
        if (analysis.actualMove != analysis.bestMove) {
            std::cout << "Move quality: " << getMoveQualityString(analysis.quality);
            if (analysis.evaluationDelta != 0) {
                std::cout << " (" << std::showpos << analysis.evaluationDelta 
                         << std::noshowpos << " points)";
            }
            std::cout << "\n";
        } else {
            std::cout << "Move quality: " << getMoveQualityString(analysis.quality) << "\n";
        }
    }
}

void CLI::settingsMenu() {
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* DIM = "\033[2m";
    const char* CYAN = "\033[96m";
    const char* BLUE = "\033[94m";
    const char* RED = "\033[91m";
    const char* YELLOW = "\033[93m";
    
    clearScreen();
    
    std::cout << "\n";
    std::cout << "   " << BLUE << "╔═══════════════════════════════════════╗" << RESET << "\n";
    std::cout << "   " << BLUE << "║" << RESET << "            " << BOLD << "⚙  SETTINGS" << RESET << "               " << BLUE << "║" << RESET << "\n";
    std::cout << "   " << BLUE << "╚═══════════════════════════════════════╝" << RESET << "\n";
    std::cout << "\n";
    
    const auto& config = gameState_->getConfig();
    std::cout << "   " << DIM << "Current Configuration:" << RESET << "\n\n";
    std::cout << "      Board Size   " << BOLD << config.boardWidth << "×" << config.boardHeight << RESET << "\n";
    std::cout << "      Win Length   " << BOLD << config.winLength << RESET << " in a row\n";
    std::cout << "      AI Depth     " << BOLD << config.aiDepth << RESET << " moves ahead\n";
    std::cout << "      " << RED << "●" << RESET << " Red        " << BOLD << ((config.player1Type == PlayerType::Human) ? "Human" : "AI") << RESET << "\n";
    std::cout << "      " << YELLOW << "●" << RESET << " Yellow     " << BOLD << ((config.player2Type == PlayerType::Human) ? "Human" : "AI") << RESET << "\n";
    std::cout << "\n";
    
    std::cout << "   " << CYAN << "[1]" << RESET << " Configure Board\n";
    std::cout << "   " << CYAN << "[2]" << RESET << " Configure AI Difficulty\n";
    std::cout << "   " << CYAN << "[3]" << RESET << " Configure Players\n";
    std::cout << "   " << CYAN << "[4]" << RESET << " " << DIM << "Back" << RESET << "\n";
    std::cout << "\n   " << DIM << "Choice:" << RESET << " ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    switch (choice) {
        case 1:
            configureBoard();
            break;
        case 2:
            configureAI();
            break;
        case 3:
            configurePlayers();
            break;
        case 4:
            currentMode_ = Mode::MainMenu;
            break;
    }
}

void CLI::configureBoard() {
    GameConfig config = gameState_->getConfig();
    
    std::cout << "\nEnter board width (5-20): ";
    std::cin >> config.boardWidth;
    
    std::cout << "Enter board height (5-20): ";
    std::cin >> config.boardHeight;
    
    std::cout << "Enter win length (3-" << std::min(config.boardWidth, config.boardHeight) << "): ";
    std::cin >> config.winLength;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    // Validate
    config.boardWidth = std::clamp(config.boardWidth, 5, 20);
    config.boardHeight = std::clamp(config.boardHeight, 5, 20);
    config.winLength = std::clamp(config.winLength, 3, std::min(config.boardWidth, config.boardHeight));
    
    gameState_ = std::make_unique<GameState>(config);
    aiEngine_ = std::make_unique<ParallelMinimax>(config.winLength, config.aiDepth);
    
    std::cout << "Configuration updated!\n";
    waitForEnter();
}

void CLI::configureAI() {
    GameConfig config = gameState_->getConfig();
    
    std::cout << "\nEnter AI search depth (1-20): ";
    std::cin >> config.aiDepth;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    config.aiDepth = std::clamp(config.aiDepth, 1, 20);
    
    gameState_ = std::make_unique<GameState>(config);
    aiEngine_->setMaxDepth(config.aiDepth);
    
    std::cout << "AI depth set to " << config.aiDepth << "\n";
    waitForEnter();
}

void CLI::configurePlayers() {
    GameConfig config = gameState_->getConfig();
    
    std::cout << "\nPlayer 1 (1=Human, 2=AI): ";
    int p1;
    std::cin >> p1;
    config.player1Type = (p1 == 1) ? PlayerType::Human : PlayerType::AI;
    
    std::cout << "Player 2 (1=Human, 2=AI): ";
    int p2;
    std::cin >> p2;
    config.player2Type = (p2 == 1) ? PlayerType::Human : PlayerType::AI;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    gameState_ = std::make_unique<GameState>(config);
    
    std::cout << "Player configuration updated!\n";
    waitForEnter();
}

void CLI::saveGame() {
    std::string filename = GameSerializer::generateFilename("game");
    std::string savePath = GameSerializer::getSaveDirectory() + "/" + filename;
    
    if (GameSerializer::saveGame(*gameState_, savePath)) {
        std::cout << "\nGame saved to: " << savePath << "\n";
    } else {
        std::cout << "\nFailed to save game!\n";
    }
    
    waitForEnter();
}

void CLI::loadGame() {
    std::cout << "\nEnter save file path: ";
    std::string path;
    std::getline(std::cin, path);
    
    auto loadedGame = GameSerializer::loadGame(path);
    if (loadedGame) {
        gameState_ = std::make_unique<GameState>(*loadedGame);
        
        const auto& config = gameState_->getConfig();
        aiEngine_ = std::make_unique<ParallelMinimax>(config.winLength, config.aiDepth);
        
        std::cout << "Game loaded successfully!\n";
        currentMode_ = Mode::Playing;
    } else {
        std::cout << "Failed to load game!\n";
    }
    
    waitForEnter();
}

void CLI::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void CLI::waitForEnter() const {
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

std::string CLI::getPlayerSymbol(Player player) const {
    const char* RESET = "\033[0m";
    const char* RED = "\033[91m";
    const char* YELLOW = "\033[93m";
    const char* DIM = "\033[2m";
    
    switch (player) {
        case Player::Player1: 
            return std::string(RED) + " ● " + RESET;
        case Player::Player2: 
            return std::string(YELLOW) + " ● " + RESET;
        default: 
            return std::string(DIM) + " · " + RESET;
    }
}

std::string CLI::getMoveQualityString(MoveQuality quality) const {
    switch (quality) {
        case MoveQuality::Excellent: return "✓ Excellent";
        case MoveQuality::Good: return "✓ Good";
        case MoveQuality::Inaccuracy: return "? Inaccuracy";
        case MoveQuality::Mistake: return "⚠ Mistake";
        case MoveQuality::Blunder: return "✗ Blunder";
        default: return "Unknown";
    }
}

} // namespace ConnectFour
