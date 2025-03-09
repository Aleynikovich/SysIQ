      
# SysIQ: AI-Powered Command Line Utility

[![C++](https://img.shields.io/badge/language-C++-blue.svg)](https://www.cplusplus.com/)
[![AI Powered](https://img.shields.io/badge/Powered%20by-Gemini%20AI-orange.svg)](https://ai.google.dev/models/gemini)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

SysIQ is a command-line utility that leverages AI to assist users in finding and executing commands on Linux systems. It interprets natural language queries, identifies necessary packages, and provides the commands to achieve the desired task.

## Features:

*   **Natural Language Queries:**  Translate plain English questions into Linux commands.
*   **Intelligent Package Resolution:**  AI-driven identification of required packages for specific tasks.
*   **Interactive Package Handling:** Presents a list of suggested packages with installation status and allows user selection.
*   **System Context Aware:** Considers distro, desktop environment, shell, and terminal for relevant command suggestions.
*   **Formatted Terminal Output:** Provides a structured and colorized terminal interface for improved readability.

## Usage:

### Prerequisites:

*   **libcurl:**  For HTTP requests to the Gemini API.
    ```bash
    sudo apt-get install libcurl4-openssl-dev  # Debian/Ubuntu
    sudo pacman -S curl                       # Arch Linux
    # ... or equivalent for your distro
    ```
*   **nlohmann_json:**  Single-header JSON library for C++.
    [Download nlohmann_json.hpp](https://github.com/nlohmann/json) and place in your `include` directory.
*   **Gemini API Key:** Obtain an API key from Google AI Studio and set it as an environment variable:
    ```bash
    export GEMINI_API_KEY="YOUR_API_KEY_HERE"
    ```
    **Security:** Store your API key securely and avoid hardcoding it directly in the code.

### Compilation:

From the `src` directory:

```bash
cd SysIQ/src
g++ -o ../bin/sysiq main.cpp config.cpp systeminfo.cpp ai.cpp utils.cpp -lcurl -std=c++17 -I../include

Ensure -lcurl and -std=c++17 are included in your compilation command. Adjust -I../include if necessary.
Execution:

Run the compiled binary with your query as an argument:

      
./bin/sysiq <query>


Example Queries:

./bin/sysiq get my ip address
./bin/sysiq check free disk space
./bin/sysiq current weather in London
./bin/sysiq monitor model na

SysIQ will:

    Query Gemini AI: Send a request based on your query and system information.

    Suggest Packages: Output a list of packages potentially required for the task.

    User Selection: Present a numbered list, allowing you to choose a package.

    Installation Prompt: Prompt for installation if the selected package is not detected.

    Execute Command: Run the command associated with the chosen package and display the result.

Configuration:

Configuration is managed through ~/.config/sysiq/config.json. The file is created on first run if it does not exist, and an interactive setup will guide you through initial configuration.

Config File (config.json) Structure:

      
{
  "distro": "Your Linux Distribution",
  "desktop": "Your Desktop Environment",
  "shell": "Your Shell",
  "terminal": "Your Terminal Emulator",
  "ai_api": "YOUR_API_KEY_HERE",
  "package_manager": "Your Package Manager (e.g., yay, apt, dnf)"
}


API Key Security: Store your ai_api key securely. Environment variables are recommended over direct inclusion in the configuration file for sensitive credentials.
Contributing

Contributions are welcome. Fork the repository, submit pull requests, or create issues for bug reports and feature suggestions.
License

MIT License

Command line assistance, powered by AI.
