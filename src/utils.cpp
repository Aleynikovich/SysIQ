#include "utils.hpp"
#include <cstdio>
#include <array>

std::string runCommand(const std::string &command) {
    std::array<char, 256> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "ERROR: Could not run command.";
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

bool checkDependency(const std::string &cmd) {
    std::string result = runCommand("which " + cmd);
    return !result.empty();
}
