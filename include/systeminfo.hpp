#ifndef SYSTEMINFO_HPP
#define SYSTEMINFO_HPP

#include "json.hpp"
#include "config.hpp"

using json = nlohmann::json;

// Gathers system information based on current environment and config.
json getSystemInfo(const Config &config);

#endif // SYSTEMINFO_HPP
