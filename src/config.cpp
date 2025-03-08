#include "config.hpp"
#include <fstream>
#include <iostream>
#include <filesystem> // Necessary for creating directories

namespace fs = std::filesystem;

Config Config::load(const std::string &configPath) {
    Config config;
    std::ifstream configFile(configPath);

    if (!configFile.is_open()) {
        // Check if the directory exists, and create it if it doesn't
        fs::path dirPath = fs::path(configPath).parent_path();
        if (!fs::exists(dirPath)) {
            std::cout << "Creating directory: " << dirPath << std::endl;
            try {
                fs::create_directories(dirPath); // Create parent directories
            } catch (const std::exception& e) {
                std::cerr << "Error creating directory: " << e.what() << std::endl;
                std::cerr << "Config file not found, and unable to create directory. Launching interactive setup." << std::endl;
                return interactiveSetup();
            }
        }

        std::cerr << "Config file not found at " << configPath << ". Launching interactive setup." << std::endl;
        return interactiveSetup();
    }

    try {
        json configJson;
        configFile >> configJson;

        config.distro = configJson.value("distro", "");
        config.desktop = configJson.value("desktop", "");
        config.shell = configJson.value("shell", "");
        config.terminal = configJson.value("terminal", "");
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file: " << e.what() << ". Using interactive setup." << std::endl;
        return interactiveSetup();
    }

    return config;
}

void Config::save(const std::string &configPath) const {
    json configJson;
    configJson["distro"] = distro;
    configJson["desktop"] = desktop;
    configJson["shell"] = shell;
    configJson["terminal"] = terminal;

    std::ofstream configFile(configPath);
    if (configFile.is_open()) {
        configFile << configJson.dump(4); // Use dump(4) for pretty printing
        if(configFile.good()){
          std::cout << "Configuration saved to " << configPath << std::endl;
        }else{
          std::cerr << "Error writing to config file!" << std::endl;
        }
    } else {
        std::cerr << "Error opening config file for writing!" << std::endl;
    }
}


Config Config::interactiveSetup() {
    Config config;

    std::cout << "Starting interactive configuration setup:" << std::endl;

    std::cout << "Enter your Linux distribution: ";
    std::getline(std::cin, config.distro);

    std::cout << "Enter your desktop environment: ";
    std::getline(std::cin, config.desktop);

    std::cout << "Enter your shell: ";
    std::getline(std::cin, config.shell);

    std::cout << "Enter your terminal: ";
    std::getline(std::cin, config.terminal);

    std::cout << "Interactive configuration complete." << std::endl;
    return config;
}