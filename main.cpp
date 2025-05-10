#include "ChatBot.h"
#include <iostream>
#include <string>

int main() {
    ChatBot bot;
    std::string input;

    std::cout << "Welcome to the OpenAI ChatBot!\n";
    std::cout << "Type 'exit' to quit, 'history' to view conversation history.\n\n";

    while (true) {
        std::cout << "\nYou (" << bot.getInteractionCount() + 1 << "): ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        if (input == "history") {
            bot.displayHistory();
            std::cout << "\nAverage response time: " << bot.getAverageResponseTime() << "ms\n";
            continue;
        }

        try {
            std::string response = bot.getResponse(input);
            std::cout << "\nBot: " << response << "\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "\nFinal Statistics:\n";
    std::cout << "Total interactions: " << bot.getInteractionCount() << "\n";
    std::cout << "Average response time: " << bot.getAverageResponseTime() << "ms\n";

    return 0;
}
