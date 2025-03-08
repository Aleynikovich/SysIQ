#include "config.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

Config Config::interactiveSetup() {
    Config cfg;
    std::string input;
    std::cout << "=== SysIQ Configurator ===\n";

    std::cout << "Enter your distro (default: Arch Linux): ";
    std::getline(std::cin, input);
    cfg.distro = input.empty() ? "Arch Linux" : input;

    std::cout << "Enter your Desktop Environment/WM (default: Hyprland): ";
    std::getline(std::cin, input);
    cfg.desktop = input.empty() ? "Hyprland" : input;

    std::cout << "Enter your shell (default: fish): ";
    std::getline(std::cin, input);
    cfg.shell = input.empty() ? "fish" : input;

    std::cout << "Enter your terminal emulator (default: foot): ";
    std::getline(std::cin, input);
    cfg.terminal = input.empty() ? "foot" : input;

    std::cout << "Enter your preferred AI API (default: Gemini): ";
    std::getline(std::cin, input);
    cfg.ai_api = input.empty() ? "Gemini" : input;

    return cfg;
}

Config Config::load(const std::string &configPath) {
    Config cfg;
    if (!fs::exists(configPath)) {
        std::cout << "Config file not found at " << configPath << ". Create one? (y/n): ";
        std::string answer;
        std::getline(std::cin, answer);
        if (answer == "y" || answer == "Y") {
            cfg = Config::interactiveSetup();
            fs::create_directories(fs::path(configPath).parent_path());
            cfg.save(configPath);
        } else {
            std::cout << "Exiting without configuration.\n";
            exit(1);
        }
    } else {
        std::ifstream inFile(configPath);
        json j;
        inFile >> j;
        cfg.distro = j.value("distro", "Arch Linux");
        cfg.desktop = j.value("desktop", "Hyprland");
        cfg.shell = j.value("shell", "fish");
        cfg.terminal = j.value("terminal", "foot");
        cfg.ai_api = j.value("ai_api", "Gemini");
    }
    return cfg;
}

void Config::save(const std::string &configPath) const {
    json j;
    j["distro"] = distro;
    j["desktop"] = desktop;
    j["shell"] = shell;
    j["terminal"] = terminal;
    j["ai_api"] = ai_api;

    std::ofstream outFile(configPath);
    if (!outFile) {
        std::cerr << "Error writing configuration file.\n";
        exit(1);
    }
    outFile << j.dump(4);
    std::cout << "Configuration saved to " << configPath << "\n\n";
}
