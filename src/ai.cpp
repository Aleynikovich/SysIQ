#include "ai.hpp"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

// Callback function for libcurl to write response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::string *str = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

namespace AI {

// Function to query the Gemini API and get a response
std::string queryAI(const std::string &prompt, const Config &config) {
    // Get API key from config. Assume load/interactive setup handles this securely.
    const std::string& apiKey = config.ai_api;  // Important: Use const reference!
    std::string apiUrl = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent";

    // Construct the request payload for the Gemini API
    json payload = {
        {"contents", {
            {
                {"parts", {
                    {{"text", prompt}}
                }}
            }
        }},
        {"generationConfig", {
            {"temperature", 0.2}
        }}
    };

    std::string payloadStr = payload.dump();
    std::string responseStr;

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error initializing curl.\n";
        return "";  // Indicate failure
    }

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    //Option 1: Append key to url:
    std::string urlWithKey = apiUrl + "?key=" + apiKey;
    curl_easy_setopt(curl, CURLOPT_URL, urlWithKey.c_str());

    //Option 2: Authorization header. You may need to use OAuth.
    //std::string authHeader = "Authorization: Bearer " + apiKey;
    //headers = curl_slist_append(headers, authHeader.c_str());
    //curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str()); //Remove key from URL in this case.

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

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

    try {
        json result = json::parse(responseStr);
        // Adjust this path based on the actual JSON structure from the Gemini API.
        std::string command = result["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();
        return command;
    } catch (json::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
        std::cerr << "Response: " << responseStr << "\n";
        return "";
    }
}


// Constructs a prompt to ask the AI for the package list needed to answer the user query.
std::string queryPackageList(const Config &config, const json &sysInfo, const std::string &userQuery) {
    // Construct a prompt that includes system information and the user query.
    // Instruct the AI to output ONLY the package names (one per line).
    std::string prompt = "Considering I am using distro: " + config.distro +
                         ", Desktop Environment: " + config.desktop +
                         ", Shell: " + config.shell +
                         ", Terminal: " + config.terminal +
                         ", and system info: " + sysInfo.dump() +
                         ", give me ONLY a list (one per line) of the packages needed to answer the following question: " +
                         userQuery;
    return queryAI(prompt, config);
}

// Constructs a prompt to ask the AI for the command to install a list of missing packages.
std::string queryInstallCommand(const Config &config, const json &sysInfo, const std::string &missingPackages) {
    std::string prompt = "Considering I am using distro: " + config.distro +
                         ", Desktop Environment: " + config.desktop +
                         ", Shell: " + config.shell +
                         ", Terminal: " + config.terminal +
                         ", and system info: " + sysInfo.dump() +
                         ", I am missing the following packages:\n" + missingPackages +
                         "\nProvide ONLY the single-line command to install them (no extra text).";
    return queryAI(prompt, config);
}

// Constructs a prompt to ask the AI for the final command to get the information asked in the user query.
std::string queryFinalCommand(const Config &config, const json &sysInfo, const std::string &userQuery) {
    std::string prompt = "Considering I am using distro: " + config.distro +
                         ", Desktop Environment: " + config.desktop +
                         ", Shell: " + config.shell +
                         ", Terminal: " + config.terminal +
                         ", and system info: " + sysInfo.dump() +
                         ", and that all required packages are installed, what is the single-line command to answer the following question: " +
                         userQuery;
    return queryAI(prompt, config);
}

} // namespace AI