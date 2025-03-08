#include "ai.hpp"
#include <iostream>
#include <cstdlib>
#include <curl/curl.h>

// Helper callback for libcurl.
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::string* s = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    s->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Sends a POST request to the AI API (Gemini in this example) and returns the response.
std::string queryAI(const std::string &prompt) {
    // Build JSON payload.
    json payload = {
        {"model", "gemini"},
        {"messages", json::array({ { {"role", "user"}, {"content", prompt} } })},
        {"temperature", 0.2}
    };

    std::string payloadStr = payload.dump();
    std::string responseStr;

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error initializing curl.\n";
        return "";
    }

    // Get API key from environment.
    const char *apiKey = std::getenv("GEMINI_API_KEY");
    if (!apiKey) {
        std::cerr << "GEMINI_API_KEY not set in environment.\n";
        return "";
    }

    // Setup headers.
    struct curl_slist *headers = nullptr;
    std::string authHeader = "Authorization: Bearer " + std::string(apiKey);
    headers = curl_slist_append(headers, authHeader.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Set endpoint – for example, "https://gemini.ai/api/chat"
    std::string url = "https://gemini.ai/api/chat";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Curl error: " << curl_easy_strerror(res) << "\n";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return "";
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    try {
        json result = json::parse(responseStr);
        // We assume the response format is:
        // { "choices": [ { "message": { "content": "..." } } ] }
        return result["choices"][0]["message"]["content"].get<std::string>();
    } catch (json::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << "\nResponse: " << responseStr << "\n";
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
    return queryAI(prompt);
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
    return queryAI(prompt);
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
    return queryAI(prompt);
}
