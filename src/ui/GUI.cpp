#include "ui/GUI.h"
#include <iostream>

namespace ConnectFour {

GUI::GUI() 
    : window_(nullptr)
    , renderer_(nullptr)
    , running_(false)
    , currentMode_(Mode::Playing) {
}

GUI::~GUI() {
    cleanup();
}

bool GUI::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << "\n";
        return false;
    }
    
    window_ = SDL_CreateWindow(
        "Connect Four",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window_) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        return false;
    }
    
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        return false;
    }
    
    // Initialize game state with default configuration
    GameConfig config;
    gameState_ = std::make_unique<GameState>(config);
    
    aiEngine_ = std::make_unique<ParallelMinimax>(config.winLength, config.aiDepth);
    perfEstimator_ = std::make_unique<PerformanceEstimator>();
    
    return true;
}

void GUI::cleanup() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    SDL_Quit();
}

void GUI::run() {
    if (!initialize()) {
        return;
    }
    
    running_ = true;
    
    while (running_) {
        handleEvents();
        update();
        render();
        
        SDL_Delay(16);  // ~60 FPS
    }
}

void GUI::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running_ = false;
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    handleMouseClick(event.button.x, event.button.y);
                }
                break;
                
            case SDL_KEYDOWN:
                handleKeyPress(event.key.keysym.sym);
                break;
        }
    }
}

void GUI::handleMouseClick(int x, int y) {
    if (currentMode_ != Mode::Playing) {
        return;
    }
    
    if (gameState_->getStatus() != GameStatus::InProgress) {
        return;
    }
    
    if (gameState_->isCurrentPlayerAI()) {
        return;  // Don't allow clicks during AI turn
    }
    
    int column = getColumnFromMouseX(x);
    if (column >= 0 && column < gameState_->getBoard().getWidth()) {
        if (!gameState_->getBoard().isColumnFull(column)) {
            gameState_->makeMove(column);
        }
    }
}

void GUI::handleKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_ESCAPE:
            running_ = false;
            break;
        case SDLK_n:
            gameState_->reset();
            currentMode_ = Mode::Playing;
            break;
    }
}

void GUI::update() {
    if (currentMode_ == Mode::Playing) {
        if (gameState_->getStatus() == GameStatus::InProgress && gameState_->isCurrentPlayerAI()) {
            makeAIMove();
        }
    }
}

void GUI::render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer_, 240, 240, 240, 255);
    SDL_RenderClear(renderer_);
    
    switch (currentMode_) {
        case Mode::Playing:
            renderBoard(gameState_->getBoard());
            renderUI();
            break;
        case Mode::Replay:
            if (gameHistory_) {
                renderBoard(gameHistory_->getCurrentBoard());
                renderReplayControls();
            }
            break;
        default:
            // TODO: Implement other modes
            break;
    }
    
    SDL_RenderPresent(renderer_);
}

void GUI::renderBoard(const Board& board) {
    // Draw board background
    SDL_Color boardColor = {30, 80, 150, 255};
    drawRect(BOARD_OFFSET_X - 10, BOARD_OFFSET_Y - 10, 
             board.getWidth() * CELL_SIZE + 20, 
             board.getHeight() * CELL_SIZE + 20, 
             boardColor);
    
    // Draw cells
    for (int y = 0; y < board.getHeight(); ++y) {
        for (int x = 0; x < board.getWidth(); ++x) {
            renderCell(x, y, board.getCell(x, y));
        }
    }
}

void GUI::renderCell(int x, int y, Player player) {
    int screenX = BOARD_OFFSET_X + x * CELL_SIZE;
    int screenY = BOARD_OFFSET_Y + (gameState_->getBoard().getHeight() - 1 - y) * CELL_SIZE;
    
    // Draw cell background
    SDL_Color bgColor = {220, 220, 220, 255};
    drawCircle(screenX + CELL_SIZE / 2, screenY + CELL_SIZE / 2, CELL_SIZE / 2 - 5, bgColor);
    
    // Draw piece if present
    if (player != Player::None) {
        SDL_Color pieceColor = (player == Player::Player1) ? 
            SDL_Color{255, 60, 60, 255} : SDL_Color{255, 220, 60, 255};
        drawCircle(screenX + CELL_SIZE / 2, screenY + CELL_SIZE / 2, CELL_SIZE / 2 - 10, pieceColor);
    }
}

void GUI::renderUI() {
    // TODO: Render status text, buttons, etc.
    // For now, just a placeholder
}

void GUI::renderReplayControls() {
    // TODO: Render replay navigation controls
}

void GUI::makeAIMove() {
    auto result = aiEngine_->findBestMove(gameState_->getBoard(), gameState_->getCurrentPlayer());
    gameState_->makeMove(result.bestMove);
}

int GUI::getColumnFromMouseX(int mouseX) const {
    if (mouseX < BOARD_OFFSET_X) return -1;
    
    int column = (mouseX - BOARD_OFFSET_X) / CELL_SIZE;
    return column;
}

void GUI::drawRect(int x, int y, int w, int h, SDL_Color color, bool filled) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    
    if (filled) {
        SDL_RenderFillRect(renderer_, &rect);
    } else {
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void GUI::drawCircle(int centerX, int centerY, int radius, SDL_Color color, bool filled) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    
    // Simple circle drawing algorithm
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                if (filled || (dx*dx + dy*dy) >= ((radius - 2) * (radius - 2))) {
                    SDL_RenderDrawPoint(renderer_, centerX + dx, centerY + dy);
                }
            }
        }
    }
}

void GUI::drawText(const std::string& text, int x, int y, SDL_Color color) {
    // TODO: Implement text rendering with SDL_ttf
    // For basic version, text rendering is optional
}

} // namespace ConnectFour
