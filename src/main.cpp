#include <iostream>
#include <string>
#include <sstream> // Include sstream for string concatenation
#include "ai.hpp"
#include "config.hpp"
#include "json.hpp"
#include <filesystem>
#include <cstdlib> // Required for getenv
#include <algorithm> // Needed for std::find
#include <stdexcept>

namespace fs = std::filesystem;

using json = nlohmann::json;

// Function to check if a package is installed (replace with your system's method)
bool isPackageInstalled(const std::string& package) {
    // This is a placeholder - implement the actual check for your system
    // Example (for Debian/Ubuntu):
    // std::string command = "dpkg -s " + package + " > /dev/null 2>&1";
    // return (system(command.c_str()) == 0); // Returns 0 if package is installed

    // Placeholder: Always returns false for now
    return false;
}

// Function to install a package (replace with your system's method)
std::string installPackage(const Config& config, const std::string& installCommand) {
    json sysInfo = {
      {"distro", config.distro},
      {"desktop", config.desktop},
      {"shell", config.shell},
      {"terminal", config.terminal}
    };
    std::cout << "Command: " << installCommand << std::endl;
    int exit_code = system(installCommand.c_str());
    if(exit_code == 0)
    {
      std::cout << "Installed fine" << std::endl;
    }else{
      std::cout << "Could not install" << std::endl;
    }
    //IMPLEMENT ME
    return "";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <user_query>" << std::endl;
        return 1;
    }

    // Load the configuration
    Config config = Config::load();

    // Get the API key from the environment variables:
    const char* apiKey = std::getenv("GEMINI_API_KEY");
    if (apiKey == nullptr) {
        std::cerr << "Error: GEMINI_API_KEY environment variable not set." << std::endl;
        return 1;
    }
    std::string apiKeyStr(apiKey);

    // Concatenate command line arguments into a single user query string
    std::stringstream ss;
    for (int i = 1; i < argc; ++i) {
        ss << argv[i] << (i == argc - 1 ? "" : " ");  // Add space between arguments
    }
    std::string userQuery = ss.str();

    // Create a JSON object to hold the configuration data
    json sysInfo = {
      {"distro", config.distro},
      {"desktop", config.desktop},
      {"shell", config.shell},
      {"terminal", config.terminal}
    };

    // 1. Get the list of required packages
    AI::PackageListResponse packageListResponse = AI::queryPackageList(config, sysInfo, userQuery, apiKeyStr);
    if (packageListResponse.packages.empty()) {
        std::cerr << "Failed to get the list of required packages." << std::endl;
        return 1;
    }

    // 2. Install missing packages
    std::vector<AI::PackageInfo> missingPackages;
    for (const auto& package : packageListResponse.packages) {
        if (!isPackageInstalled(package.package_name)) {
            missingPackages.push_back(package);
        }
    }

    if (!missingPackages.empty()) {
        std::cout << "Installing missing packages:" << std::endl;
        //Get command to install packages
        //Run Command to install

        std::string installResult = installPackage(config, missingPackages[0].command);

    } else {
        std::cout << "All required packages are already installed." << std::endl;
    }

    // 3. Get the final command to execute
    // Execute the command (replace with your preferred method)
    return 0;
}