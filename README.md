      
# SysIQ: Unleash the Power of AI in Your Terminal

[![C++](https://img.shields.io/badge/language-C++-blue.svg)](https://www.cplusplus.com/)
[![AI Powered](https://img.shields.io/badge/Powered%20by-Gemini%20AI-orange.svg)](https://ai.google.dev/models/gemini)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**Tired of endless man pages and cryptic commands?**  SysIQ is your AI-powered command-line companion, designed to bridge the gap between natural language and the powerful world of Linux.  Ask questions in plain English, and SysIQ will intelligently guide you to the commands you need, handling package dependencies along the way.

## 🚀 Features:

*   **Natural Language Command Querying:**  No more memorizing arcane syntax!  Simply ask SysIQ what you want to do in plain English, and let AI figure out the command.
*   **Intelligent Package Management:** SysIQ, powered by Gemini AI, doesn't just give you commands – it understands the underlying system.  It suggests necessary packages and guides you through installation if needed.
*   **Interactive Package Selection:**  Get a curated list of relevant packages, see their install status, and choose the right tool for the job – all within your terminal.
*   **System-Aware Prompts:** SysIQ is context-aware, considering your distro, desktop environment, shell, and terminal for more accurate and relevant command suggestions.
*   **Dynamic Terminal Interface:** Experience a visually engaging terminal output with color-coded information and loading indicators, providing a fluid and responsive user experience. (No cringe delays here, just pure optimized feedback 😉)

## ✨ How to Use:

### Prerequisites:

Before you dive in, ensure you have the following installed:

*   **libcurl:**  For seamless communication with the Gemini API.
    ```bash
    sudo apt-get install libcurl4-openssl-dev  # Debian/Ubuntu
    sudo pacman -S curl                       # Arch Linux
    # ... or your distro's equivalent
    ```
*   **nlohmann_json:**  The blazing-fast JSON library for C++.  (This project uses the single-header version - just include it!)
    [Download nlohmann_json.hpp](https://github.com/nlohmann/json) and place it in your `include` directory.
*   **Gemini API Key:**  You'll need an API key from Google AI Studio.  Set it as an environment variable:
    ```bash
    export GEMINI_API_KEY="YOUR_API_KEY_HERE"
    ```
    **Important Security Note:**  Never hardcode your API key directly in the code! Environment variables are the secure way to go.

### Compilation:

Navigate to the `src` directory in your SysIQ repository and compile using `g++`:

```bash
cd SysIQ/src
g++ -o ../bin/sysiq main.cpp config.cpp systeminfo.cpp ai.cpp utils.cpp -lcurl -std=c++17 -I../include

    

IGNORE_WHEN_COPYING_START
Use code with caution.Markdown
IGNORE_WHEN_COPYING_END

Make sure you have -lcurl to link the curl library and -std=c++17 for C++17 support. Adjust the -Iinclude path if your include directory is located differently.
Running SysIQ:

Simply execute the compiled binary with your query as a command-line argument:

      
./bin/sysiq <your_query>

    

IGNORE_WHEN_COPYING_START
Use code with caution.Bash
IGNORE_WHEN_COPYING_END

Examples:

      
./bin/sysiq what is my ip address
./bin/sysiq how much disk space is free
./bin/sysiq what is the weather in London
./bin/sysiq whats my monitor model

    

IGNORE_WHEN_COPYING_START
Use code with caution.Bash
IGNORE_WHEN_COPYING_END

SysIQ will then:

    Query Gemini AI: Intelligently formulate a request based on your query and system information.

    Suggest Packages: If necessary, SysIQ will suggest a list of packages required to execute the command.

    Interactive Selection: You'll be presented with a numbered list of packages. Choose one to proceed.

    Install (Optional): If the selected package is not installed, SysIQ will prompt you for installation confirmation before proceeding.

    Execute Command: Finally, SysIQ executes the command and displays the output directly in your terminal.

Configuration:

Customize SysIQ by editing the config.json file located at ~/.config/sysiq/config.json. If the file doesn't exist, SysIQ will guide you through an interactive setup on the first run.

Configuration Options:

      
{
  "distro": "Your Linux Distribution",
  "desktop": "Your Desktop Environment",
  "shell": "Your Shell",
  "terminal": "Your Terminal Emulator",
  "ai_api": "YOUR_API_KEY_HERE",  <--  (Loaded securely, don't hardcode here!)
  "package_manager": "Your Package Manager (e.g., yay, apt, dnf)"
}

    

IGNORE_WHEN_COPYING_START
Use code with caution.Json
IGNORE_WHEN_COPYING_END

Security Note: The ai_api field in config.json is automatically loaded if you run the interactive setup. However, for production use, consider loading your API key from environment variables or a more secure secrets management system.
🤝 Contributing

Contributions are welcome! Feel free to fork this repository, submit pull requests, or open issues to suggest improvements or report bugs.
📜 License

This project is licensed under the MIT License - see the LICENSE file for details.
