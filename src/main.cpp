#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <curl/curl.h>
#include "ai.hpp"
#include "config.hpp"
#include "json.hpp"
#include <filesystem>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <thread>     // Required for std::this_thread::sleep_for
#include <chrono>     // Required for std::chrono

namespace fs = std::filesystem;
using json = nlohmann::json;

// ANSI escape codes for colors and formatting (as before)
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_BOLD    "\x1b[1m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Helper function for sleep/delay
void sleep_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// Function to display a simple progress bar (modified for smoother animation)
void showProgressBar(float progress, const std::string& message) {
    int barWidth = 50;
    int pos = barWidth * progress;
    std::cout << ANSI_COLOR_YELLOW << message << " [" << ANSI_COLOR_RESET;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << ANSI_COLOR_GREEN << "=" << ANSI_COLOR_RESET;
        else std::cout << " ";
    }
    int percentage = static_cast<int>(progress * 100.0); // Calculate percentage once
    std::cout << ANSI_COLOR_YELLOW << "] " << ANSI_COLOR_RESET << percentage << "%\r";
    std::cout.flush();
}

// Function to clear the terminal screen (as before)
void clearScreen() {
    std::cout << "\033[2J\033[H"; // ANSI escape sequence to clear screen and move cursor to top-left
}

// Function to check if a package is installed (as before - no changes)
bool isPackageInstalled(const std::string& package, const Config& config) {
    std::string command = config.package_manager + " -Qi " + package + " > /dev/null 2>&1";
    return (system(command.c_str()) == 0);
}

// Function to install a package (modified progress bar animation and delays)
std::string installPackage(const Config& config, const std::string& package) {
    std::string installCommand = "sudo " + config.package_manager + " -S " + package;
    std::cout << ANSI_COLOR_GREEN << ANSI_COLOR_BOLD << "Executing install command: " << ANSI_COLOR_RESET << ANSI_COLOR_GREEN << installCommand << ANSI_COLOR_RESET << std::endl;

    // Simulate installation with progress bar (more steps, longer delay)
    for (float progress = 0.0; progress <= 1.0; progress += 0.1) { // Increased steps for smoother bar
        showProgressBar(progress, "Installing"); // Added message to progress bar
        sleep_ms(200); // Longer delay to make progress visible
    }
    std::cout << std::endl; // Newline after progress bar

    int exit_code = system(installCommand.c_str());
    if(exit_code == 0) {
        std::cout << ANSI_COLOR_GREEN << ANSI_COLOR_BOLD << "Installation successful!" << ANSI_COLOR_RESET << std::endl;
        return "Installation successful";
    } else {
        std::cerr << ANSI_COLOR_RED << ANSI_COLOR_BOLD << "Installation failed!" << ANSI_COLOR_RESET << std::endl;
        return "Installation failed";
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << ANSI_COLOR_RED << ANSI_COLOR_BOLD << "Usage: " << ANSI_COLOR_RESET << argv[0] << " <user_query>" << std::endl;
        return 1;
    }

    // Load the configuration
    Config config = Config::load();
    config.save("/home/xyz/.config/sysiq/config.json");

    // Get the API key from the environment variables:
    const char* apiKey = std::getenv("GEMINI_API_KEY");
    if (apiKey == nullptr) {
        std::cerr << ANSI_COLOR_RED << ANSI_COLOR_BOLD << "Error: GEMINI_API_KEY environment variable not set." << ANSI_COLOR_RESET << std::endl;
        return 1;
    }
    std::string apiKeyStr(apiKey);

    // Concatenate command line arguments into a single user query string
    std::stringstream ss;
    for (int i = 1; i < argc; ++i) {
        ss << argv[i] << (i == argc - 1 ? "" : " ");
    }
    std::string userQuery = ss.str();

    // Create a JSON object to hold the configuration data
    json sysInfo = {
      {"distro", config.distro},
      {"desktop", config.desktop},
      {"shell", config.shell},
      {"terminal", config.terminal}
    };

    clearScreen(); // Clear screen before AI query
    for (float progress = 0.1f; progress <= 1.0f; progress += 0.1f) { // Progress bar during AI query
        showProgressBar(progress, "Querying AI for Packages");
        sleep_ms(50); // Delay for AI thinking progress
    }
    std::cout << std::endl; // Newline after progress bar
    //std::cout << ANSI_COLOR_YELLOW << ANSI_COLOR_BOLD << "\n--- Querying AI for Packages ---" << ANSI_COLOR_RESET << std::endl;
    showProgressBar(0.0f, "Thinking"); // Start progress bar at 0%

    AI::PackageListResponse packageListResponse = AI::queryPackageList(config, sysInfo, userQuery, apiKeyStr);

    for (float progress = 0.1f; progress <= 1.0f; progress += 0.1f) { // Progress bar during AI query
        showProgressBar(progress, "Thinking");
        sleep_ms(100); // Delay for AI thinking progress
    }
    std::cout << std::endl; // Newline after progress bar
    clearScreen(); // Clear screen after AI response


    if (packageListResponse.packages.empty()) {
        std::cerr << ANSI_COLOR_RED << ANSI_COLOR_BOLD << "Failed to get the list of required packages." << ANSI_COLOR_RESET << std::endl;
        return 1;
    }

    // 2. Display packages to user and handle installation (as before, but with delay)
    std::cout << ANSI_COLOR_YELLOW << ANSI_COLOR_BOLD << "\n--- Choose Package ---" << ANSI_COLOR_RESET << std::endl;
    for (size_t i = 0; i < packageListResponse.packages.size(); ++i) {
        std::string installedStatus = isPackageInstalled(packageListResponse.packages[i].package_name, config) ? std::string(ANSI_COLOR_GREEN) + "[Installed]" + ANSI_COLOR_RESET : std::string(ANSI_COLOR_RED) + "[Not Installed]" + ANSI_COLOR_RESET;
        std::cout << ANSI_COLOR_YELLOW << ANSI_COLOR_BOLD << i + 1 << ". " << ANSI_COLOR_RESET
                  << packageListResponse.packages[i].package_name << " - Command: " << packageListResponse.packages[i].command << " " << installedStatus << std::endl;
         sleep_ms(50); // Small delay between package list items
    }

    int choice;
    std::cout << ANSI_COLOR_YELLOW << ANSI_COLOR_BOLD << "Select a package number to use (or 0 to skip): " << ANSI_COLOR_RESET;
    std::cin >> choice;

    if (choice > 0 && choice <= packageListResponse.packages.size()) {
        AI::PackageInfo selectedPackage = packageListResponse.packages[choice - 1];
        if (!isPackageInstalled(selectedPackage.package_name, config)) {
            char installChoice;
            std::cout << ANSI_COLOR_YELLOW << ANSI_COLOR_BOLD << "Package '" << selectedPackage.package_name << "' is not installed. Install now? (y/N): " << ANSI_COLOR_RESET;
            std::cin >> installChoice;
            if (installChoice == 'y' || installChoice == 'Y') {
                installPackage(config, selectedPackage.package_name);
            } else {
                std::cout << ANSI_COLOR_YELLOW << ANSI_COLOR_BOLD << "Installation skipped." << ANSI_COLOR_RESET << std::endl;
                return 0; // Exit if installation skipped
            }
        }

        // 3. Execute the command for the chosen package
        clearScreen(); // Clear screen before command execution
        std::cout << ANSI_COLOR_YELLOW << ANSI_COLOR_BOLD << "\n--- Executing Command ---" << ANSI_COLOR_RESET << std::endl;
        std::cout << ANSI_COLOR_GREEN << ANSI_COLOR_BOLD << "Executing command: " << ANSI_COLOR_RESET << ANSI_COLOR_GREEN << selectedPackage.command << ANSI_COLOR_RESET << std::endl;
        system(selectedPackage.command.c_str());


    } else if (choice != 0) {
        std::cerr << ANSI_COLOR_RED << ANSI_COLOR_BOLD << "Invalid choice." << ANSI_COLOR_RESET << std::endl;
        return 1;
    } else {
        std::cout << ANSI_COLOR_YELLOW << ANSI_COLOR_BOLD << "Skipping package selection and command execution." << ANSI_COLOR_RESET << std::endl;
    }


    return 0;
}