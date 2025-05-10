#include "ChatBot.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <stdexcept>

using json = nlohmann::json;

// Callback function for curl
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string ChatBot::makeApiCall(const std::string& input, int retryCount) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    try {
        // Set up the API request
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer YOUR_API_KEY_HERE");

        json requestBody = {
            {"model", "gpt-3.5-turbo"},
            {"messages", {{
                {"role", "user"},
                {"content", input}
            }}}
        };

        std::string requestStr = requestBody.dump();

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            if (retryCount < maxRetries) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return makeApiCall(input, retryCount + 1);
            }
            throw std::runtime_error("Failed to make API call after max retries");
        }

        json responseJson = json::parse(response);
        return responseJson["choices"][0]["message"]["content"];

    } catch (const std::exception& e) {
        if (retryCount < maxRetries) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return makeApiCall(input, retryCount + 1);
        }
        throw;
    }
}

bool ChatBot::validateInput(const std::string& input) const {
    if (input.empty()) {
        return false;
    }
    if (input.length() > MAX_INPUT_LENGTH) {
        return false;
    }
    return true;
}

void ChatBot::addToHistory(const std::string& input, const std::string& response, double responseTime) {
    history.push_back({input, response, responseTime});
    totalResponseTime += responseTime;
}

std::string ChatBot::getResponse(const std::string& input) {
    if (!validateInput(input)) {
        throw std::invalid_argument("Invalid input: Input must not be empty and must be less than 500 characters");
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::string response = makeApiCall(input);
    auto end = std::chrono::high_resolution_clock::now();

    double responseTime = std::chrono::duration<double, std::milli>(end - start).count();
    addToHistory(input, response, responseTime);

    return response;
}

void ChatBot::displayHistory() const {
    std::cout << "\nConversation History:\n";
    for (size_t i = 0; i < history.size(); ++i) {
        std::cout << "\nInteraction #" << (i + 1) << ":\n";
        std::cout << "User: " << history[i].userInput << "\n";
        std::cout << "Bot: " << history[i].botResponse << "\n";
        std::cout << "Response time: " << history[i].responseTime << "ms\n";
    }
}

int ChatBot::getInteractionCount() const {
    return static_cast<int>(history.size());
}

double ChatBot::getAverageResponseTime() const {
    if (history.empty()) {
        return 0.0;
    }
    return totalResponseTime / history.size();
}
