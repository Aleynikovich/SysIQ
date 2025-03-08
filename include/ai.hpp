#ifndef AI_HPP
#define AI_HPP

#include <string>
#include "json.hpp"
#include "config.hpp"

using json = nlohmann::json;

// Sends a query to the AI API and returns the response as a string.
// The prompt should be constructed based on context (e.g. system info, user query, etc.).
std::string queryAI(const std::string &prompt);

// Convenience functions for our multi‑step process.
std::string queryPackageList(const Config &config, const json &sysInfo, const std::string &userQuery);
std::string queryInstallCommand(const Config &config, const json &sysInfo, const std::string &missingPackages);
std::string queryFinalCommand(const Config &config, const json &sysInfo, const std::string &userQuery);

#endif // AI_HPP
