#include "ai.hpp"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"
#include <sstream>
#include <regex> // Include regex library
#include <iomanip> // For std::quoted

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
    PackageListResponse from_json(const json& j) {
        PackageListResponse response;
        std::vector<PackageInfo> packages;
        for (const auto& element : j) { // Iterate directly over the array!
            PackageInfo packageInfo;
            packageInfo.package_name = element["package_name"].get<std::string>();
            packageInfo.command = element["command"].get<std::string>();
            packages.push_back(packageInfo);
        }
        response.packages = packages;
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
    std::string modelName = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent";

    json payload = {
        {"contents", {
            {
                {"parts", {
                    {{"text", prompt}}
                }}
            }
        }},
        {"generationConfig", {
             {"response_mime_type", "application/json"}
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

    // Construct the curl command string for logging - NOW MATCHING EXAMPLE EXACTLY
    std::stringstream curlCommand;
    curlCommand << "curl "
                << std::quoted(urlWithKey) << " \\\n"  // URL first
                << "-H " << std::quoted("Content-Type: application/json") << " \\\n" // Headers next
                << "-d " << std::quoted(payloadStr); // Data last

    std::cout << "Executing curl command:\n" << curlCommand.str() << "\n" << std::endl; // Log the full curl command

    curl_easy_setopt(curl, CURLOPT_URL, urlWithKey.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str()); // Attach JSON payload
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
    curl_easy_setopt(curl, CURLOPT_POST, 1L); // Still use POST option for libcurl

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

PackageListResponse queryPackageList(const Config &config, const json &sysInfo, const std::string &userQuery, const std::string &apiKey) {
    std::string prompt = "package and what syntax (the whole string to copy paste) to run with that package  to resolve this query: " + userQuery  + "considering im using " + config.distro + " " +
                         config.desktop + " " + config.shell + " " + config.terminal +
                         + " using this JSON schema: packages = {\"package_name\": str, \"command\":str} Return: list[packages]. Your reply will be parsed with the nlohmann/json library so the format must be correct!";
    std::cout << "Prompt being sent to AI:\n" << prompt << std::endl; // Added logging

    std::string response = queryAI(prompt, apiKey);
    if (response.empty()) return {};

    try {
        json rawResult = json::parse(response);
        std::string jsonText = rawResult["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();
        json result = safe_parse(jsonText);
        std::cout << "Parsed JSON response:\n" << result.dump(4) << "\n"; // Added logging
        PackageListResponse packageListResponse = from_json<PackageListResponse>(result);
        return packageListResponse;
    } catch (json::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
        return {};
    }
}

} // namespace AI