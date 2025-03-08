#include <iostream>
#include <string>
#include <algorithm>
#include "config.hpp"
#include "json.hpp"
#include "systeminfo.hpp"
#include "ai.hpp"
#include "utils.hpp"

using json = nlohmann::json;

int main() {
    // Determine configuration file path: ~/.config/sysiq/config.json
    std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
    std::string configPath = home + "/.config/sysiq/config.json";

    // Load (or interactively create) the configuration.
    Config config = Config::load(configPath);
    std::cout << "\nCurrent Configuration:\n";
    std::cout << "Distro: " << config.distro << "\n"
              << "Desktop: " << config.desktop << "\n"
              << "Shell: " << config.shell << "\n"
              << "Terminal: " << config.terminal << "\n"
              << "AI API: " << config.ai_api << "\n\n";

    // Gather system information.
    json sysInfo = getSystemInfo(config);
    std::cout << "--- System Information ---\n";
    std::cout << sysInfo.dump(4) << "\n\n";

    // Main query process.
    std::string userQuery;
    std::cout << "Enter your CLI query (or type 'exit' to quit): ";
    std::getline(std::cin, userQuery);
    if (userQuery == "exit" || userQuery == "quit")
        return 0;

    // Step 1: Ask AI for package list needed to answer the query.
    std::cout << "\nQuerying AI for required packages...\n";
    std::string packageList = queryPackageList(config, sysInfo, userQuery);
    std::cout << "AI package list:\n" << packageList << "\n\n";

    // Step 2: Compare with installed packages.
    // (For simplicity, we assume package names are one per line in the AI response.)
    std::istringstream iss(packageList);
    std::string pkg;
    std::string missingPackages;
    while (std::getline(iss, pkg)) {
        pkg.erase(std::remove(pkg.begin(), pkg.end(), '\r'), pkg.end());
        if (!checkDependency(pkg)) {
            missingPackages += pkg + "\n";
        }
    }

    if (!missingPackages.empty()) {
        std::cout << "Missing packages:\n" << missingPackages << "\n";
        std::cout << "Querying AI for the install command...\n";
        std::string installCmd = queryInstallCommand(config, sysInfo, missingPackages);
        std::cout << "AI suggests the following install command:\n" << installCmd << "\n";
        std::cout << "Do you want to execute the install command? (y/n): ";
        std::string answer;
        std::getline(std::cin, answer);
        if (answer == "y" || answer == "Y") {
            std::string installOutput = runCommand(installCmd);
            std::cout << "Installation command output:\n" << installOutput << "\n";
        } else {
            std::cout << "Installation command execution cancelled.\n";
        }
    } else {
        std::cout << "All required packages are already installed.\n";
    }

    // Step 3: With all packages installed, ask AI for the final command.
    std::cout << "\nQuerying AI for the final command to answer your query...\n";
    std::string finalCmd = queryFinalCommand(config, sysInfo, userQuery);
    std::cout << "AI suggests the following command:\n" << finalCmd << "\n";
    std::cout << "Do you want to execute this command? (y/n): ";
    std::string execAnswer;
    std::getline(std::cin, execAnswer);
    if (execAnswer == "y" || execAnswer == "Y") {
        std::string finalOutput = runCommand(finalCmd);
        std::cout << "Command output:\n" << finalOutput << "\n";
    } else {
        std::cout << "Final command execution cancelled.\n";
    }

    std::cout << "Exiting SysIQ. Goodbye!\n";
    return 0;
}
