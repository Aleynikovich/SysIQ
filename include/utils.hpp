#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

// Runs a shell command and returns its output as a string.
std::string runCommand(const std::string &command);

// Checks if a command (dependency) is available using "which".
bool checkDependency(const std::string &cmd);

#endif // UTILS_HPP
