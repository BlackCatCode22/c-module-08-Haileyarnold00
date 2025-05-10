#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <utility>

class ChatBot {
private:
    struct Conversation {
        std::string userInput;
        std::string botResponse;
        double responseTime;  // in milliseconds
    };

    std::vector<Conversation> history;
    int maxRetries = 3;
    double totalResponseTime = 0.0;

    // Helper functions
    std::string makeApiCall(const std::string& input, int retryCount = 0);
    bool validateInput(const std::string& input) const;
    void addToHistory(const std::string& input, const std::string& response, double responseTime);

public:
    ChatBot() = default;
    
    // Main interface methods
    std::string getResponse(const std::string& input);
    void displayHistory() const;
    int getInteractionCount() const;
    double getAverageResponseTime() const;
    
    // Constants
    static constexpr size_t MAX_INPUT_LENGTH = 500;
};
