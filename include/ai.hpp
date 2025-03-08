#ifndef AI_HPP
#define AI_HPP

#include <string>
#include "config.hpp"
#include "json.hpp"
#include <vector> //Include vector

namespace AI {

// Structs to define the expected JSON formats for each query type
struct FinalCommandResponse {
    std::string command;
};

struct PackageListResponse {
    std::vector<std::string> packages;
};

struct InstallCommandResponse {
    std::string install_command;
};


std::string queryAI(const std::string &prompt, const std::string &apiKey);
FinalCommandResponse queryFinalCommand(const Config &config, const nlohmann::json &sysInfo, const std::string &userQuery, const std::string &apiKey);
PackageListResponse queryPackageList(const Config &config, const nlohmann::json &sysInfo, const std::string &userQuery, const std::string &apiKey);
InstallCommandResponse queryInstallCommand(const Config &config, const nlohmann::json &sysInfo, const std::string &missingPackages, const std::string &apiKey);


//JSON conversion helper function:
template <typename T>
T from_json(const nlohmann::json& j);
template <typename T>
nlohmann::json safe_parse(const std::string& str);

} // namespace AI

#endif