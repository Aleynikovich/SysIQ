#include "ai.hpp"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"
#include <sstream>
#include <regex> // Include regex library

using json = nlohmann::json;

// Callback function for libcurl to write response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::string *str = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

namespace AI {
template <>
FinalCommandResponse from_json(const json& j) {
    FinalCommandResponse response;
    response.command = j.at("command").get<std::string>();
    return response;
}

template <>
PackageListResponse from_json(const json& j) {
    PackageListResponse response;
    response.packages = j.at("packages").get<std::vector<std::string>>();
    return response;
}

template <>
InstallCommandResponse from_json(const json& j) {
    InstallCommandResponse response;
    response.install_command = j.at("install_command").get<std::string>();
    return response;
}

// Safely parse a JSON string, handling errors
nlohmann::json safe_parse(const std::string& str) {
    try {
        return json::parse(str);
    } catch (json::parse_error& e) {
        std::cerr << "JSON Parse error: " << e.what() << std::endl;
        return {}; // Return an empty JSON object to indicate failure
    }
}

// Function to query the Gemini API and get a response
std::string queryAI(const std::string &prompt, const std::string &apiKey) {
    std::string modelName = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent";

    json payload = {
        {"contents", {
            {
                {"parts", {
                    {{"text", prompt}}
                }}
            }
        }},
        {"generationConfig", {
            {"temperature", 0.0} // Use temperature 0 for deterministic output,
        }}
    };
    std::string payloadStr = payload.dump();
    std::string responseStr;

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error initializing curl.\n";
        return "";
    }

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string urlWithKey = modelName + "?key=" + apiKey;

    curl_easy_setopt(curl, CURLOPT_URL, urlWithKey.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
    curl_easy_setopt(curl, CURLOPT_POST, 1L); // This is Required

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Curl error: " << curl_easy_strerror(res) << "\n";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return "";
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
      std::cerr << "HTTP error: " << http_code << "\n";
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
      return "";
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    std::cout << "Raw AI response:\n" << responseStr << "\n";
    return responseStr;
}

// Constructs a prompt to ask the AI for the final command to get the information asked in the user query.
FinalCommandResponse queryFinalCommand(const Config &config, const json &sysInfo, const std::string &userQuery, const std::string &apiKey) {
    std::string prompt = "Hello, Gemini! Can you provide me with the command i need in " + config.distro + " " + config.desktop + " " + config.shell + " " + config.terminal + " to be able to " + userQuery + "? I need your response to ONLY be a json structured text that I can parse to know which command to run. Do not include any \\n or '' just a plain json text using this JSON schema: package = {\"command\": str}";

    std::cout << "Prompt being sent to AI:\n" << prompt << std::endl; // Added logging

    std::string response = queryAI(prompt, apiKey);
    if (response.empty()) return {};

    try {
        json rawResult = json::parse(response);
        std::string jsonText = rawResult["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();

        // Clean the string to extract the JSON object
        std::regex pattern("^.*?(\\{.*\\}).*$");
        std::smatch match;
        if (std::regex_search(jsonText, match, pattern) && match.size() > 1) {
            jsonText = match[1].str();
        } else {
            std::cerr << "Could not find JSON object in response." << std::endl;
            return {};
        }

        json result = safe_parse(jsonText);
        FinalCommandResponse finalCommandResponse = from_json<FinalCommandResponse>(result);
        return finalCommandResponse;

    } catch (json::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
        return {};
    }
}

PackageListResponse queryPackageList(const Config &config, const json &sysInfo, const std::string &userQuery, const std::string &apiKey) {
    std::string prompt = "Hello, Gemini! Can you provide me with a list of packages i need in " + config.distro + " " + config.desktop + " " + config.shell + " " + config.terminal + " to be able to " + userQuery + "? I need your response to ONLY be a json structured text that I can parse to know which packages to install. Do not include any \\n or '' just a plain json text using this JSON schema: package = {\\\"package_name\\\": str}";

    std::cout << "Prompt being sent to AI:\n" << prompt << std::endl; // Added logging

    std::string response = queryAI(prompt, apiKey);
    if (response.empty()) return {};

    try {
        json rawResult = json::parse(response);
        std::string jsonText = rawResult["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();

        // Clean the string to extract the JSON object
        std::regex pattern("^.*?(\\{.*\\}).*$");
        std::smatch match;
        if (std::regex_search(jsonText, match, pattern) && match.size() > 1) {
            jsonText = match[1].str();
        } else {
            std::cerr << "Could not find JSON object in response." << std::endl;
            return {};
        }

        json result = safe_parse(jsonText);
        PackageListResponse packageListResponse = from_json<PackageListResponse>(result);
        return packageListResponse;
    } catch (json::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
        return {};
    }
}

InstallCommandResponse queryInstallCommand(const Config &config, const json &sysInfo, const std::string &missingPackages, const std::string &apiKey) {
    std::string prompt = "What command installs these packages" + missingPackages + "? "
                         "Respond with JSON: {\"install_command\": \"the command to execute\"}";

    std::cout << "Prompt being sent to AI:\n" << prompt << std::endl; // Added logging

    std::string response = queryAI(prompt, apiKey);
        if (response.empty()) return {};

    try {
        json rawResult = json::parse(response);
         std::string jsonText = rawResult["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();

        // Clean the string to extract the JSON object
        std::regex pattern("^.*?(\\{.*\\}).*$");
        std::smatch match;
        if (std::regex_search(jsonText, match, pattern) && match.size() > 1) {
            jsonText = match[1].str();
        } else {
            std::cerr << "Could not find JSON object in response." << std::endl;
            return {};
        }

        json result = safe_parse(jsonText);
        InstallCommandResponse installCommandResponse = from_json<InstallCommandResponse>(result);
        return installCommandResponse;
    } catch (json::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
        return {};
    }
}


} // namespace AI