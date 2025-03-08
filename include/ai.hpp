#ifndef AI_HPP
#define AI_HPP

#include <string>
#include "config.hpp"
#include "json.hpp"

namespace AI {

std::string queryAI(const std::string &prompt, const Config &config);
std::string queryPackageList(const Config &config, const nlohmann::json &sysInfo, const std::string &userQuery);
std::string queryInstallCommand(const Config &config, const nlohmann::json &sysInfo, const std::string &missingPackages);
std::string queryFinalCommand(const Config &config, const nlohmann::json &sysInfo, const std::string &userQuery);

} // namespace AI

#endif // AI_HPP