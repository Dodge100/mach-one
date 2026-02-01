#include "ui/CLI.h"
#include "ui/GUI.h"
#include <iostream>
#include <string>

void printUsage() {
    std::cout << "Connect Four - Optimal Bot with Configurable Board\n";
    std::cout << "=================================================\n\n";
    std::cout << "Usage:\n";
    std::cout << "  connectfour [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --cli         Launch in CLI (text) mode\n";
    std::cout << "  --gui         Launch in GUI (graphical) mode\n";
    std::cout << "  --help, -h    Show this help message\n\n";
    std::cout << "If no option is specified, you will be prompted to choose.\n";
}

int main(int argc, char* argv[]) {
    enum class InterfaceMode {
        None,
        CLI,
        GUI
    };
    
    InterfaceMode mode = InterfaceMode::None;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--cli") {
            mode = InterfaceMode::CLI;
        } else if (arg == "--gui") {
            mode = InterfaceMode::GUI;
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << "\n\n";
            printUsage();
            return 1;
        }
    }
    
    // If no mode specified, ask the user
    if (mode == InterfaceMode::None) {
        std::cout << "Connect Four - Select Interface Mode\n";
        std::cout << "=====================================\n\n";
        std::cout << "1. CLI (Text-based interface)\n";
        std::cout << "2. GUI (Graphical interface)\n\n";
        std::cout << "Choice (1-2): ";
        
        int choice;
        std::cin >> choice;
        
        if (choice == 1) {
            mode = InterfaceMode::CLI;
        } else if (choice == 2) {
            mode = InterfaceMode::GUI;
        } else {
            std::cerr << "Invalid choice!\n";
            return 1;
        }
    }
    
    // Launch the selected interface
    try {
        if (mode == InterfaceMode::CLI) {
            ConnectFour::CLI cli;
            cli.run();
        } else if (mode == InterfaceMode::GUI) {
            ConnectFour::GUI gui;
            gui.run();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
