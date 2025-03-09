#ifndef AI_HPP
#define AI_HPP

#include <string>
#include "config.hpp"
#include "json.hpp"
#include <vector> //Include vector

namespace AI {

// Structs to represent the function call arguments (for parsing)
struct PackageInfo {
    std::string package_name;
    std::string command;
};

struct PackageListResponse {
    std::vector<PackageInfo> packages;
};

PackageListResponse queryPackageList(const Config &config, const nlohmann::json &sysInfo, const std::string &userQuery, const std::string &apiKey);

//JSON conversion helper function:
template <typename T>
T from_json(const nlohmann::json& j);
template <typename T>
nlohmann::json safe_parse(const std::string& str);

} // namespace AI

#endif // AI_HPP