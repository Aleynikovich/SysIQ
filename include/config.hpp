#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "json.hpp"  // nlohmann/json single-header library
#include <string>

using json = nlohmann::json;

class Config {
public:
    std::string distro;
    std::string desktop;
    std::string shell;
    std::string terminal;

    // Loads configuration from file; if missing, launches interactive setup.
    static Config load(const std::string &configPath = "/home/xyz/.config/sysiq/config.json");

    // Saves configuration to the given file.
    void save(const std::string &configPath) const;

    // Launch an interactive configurator to create a new config.
    static Config interactiveSetup();
};

#endif // CONFIG_HPP