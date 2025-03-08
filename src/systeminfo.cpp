#include "systeminfo.hpp"
#include "utils.hpp"
#include <sstream>
#include <iostream>

json getSystemInfo(const Config &config) {
    json info;
    info["kernel"] = runCommand("uname -r");
    info["uptime"] = runCommand("uptime -p");

    // Instead of hardcoding package commands, we let AI determine it.
    // But as a fallback, if distro contains "Arch", we use pacman.
    if (config.distro.find("Arch") != std::string::npos) {
        info["packages"] = runCommand("pacman -Qq | wc -l");
    } else if (config.distro.find("Ubuntu") != std::string::npos) {
        info["packages"] = runCommand("apt list --installed 2>/dev/null | wc -l");
    } else {
        // For other distros, leave it empty. The AI can help decide later.
        info["packages"] = "Unknown";
    }

    // Get shell version based on the configured shell.
    info["shell_version"] = checkDependency(config.shell) ? runCommand(config.shell + " --version") : "Unknown";

    // Get monitor info (if xrandr exists)
    if (checkDependency("xrandr")) {
        std::string xrandrOut = runCommand("xrandr --query");
        std::istringstream iss(xrandrOut);
        std::string firstLine;
        std::getline(iss, firstLine);
        info["monitor"] = firstLine;
    } else {
        info["monitor"] = "xrandr not installed";
    }
    return info;
}
