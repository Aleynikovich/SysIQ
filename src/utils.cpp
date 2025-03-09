#include "utils.hpp"
#include <cstdio>
#include <array>
#include <algorithm> // Required for std::remove

std::string runCommand(const std::string &command) {
    std::array<char, 256> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "ERROR: Could not run command.";
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);

    // Remove trailing newline characters (CRITICAL FIX)
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end()); // Also remove carriage returns just in case

    return result;
}

bool checkDependency(const std::string &cmd) {
    std::string result = runCommand("which " + cmd);
    return !result.empty();
}