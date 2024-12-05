#include "OrderManager.hpp"
#include "DeribitWebSocketClient.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>  // For named pipes on Windows
using json = nlohmann::json;

int main() {
    try {
        Logger::init("C:/Users/dange/Desktop/OrderManagementSystem/logs/oms_orders.log");

        std::ifstream configFile("C:/Users/dange/Desktop/OrderManagementSystem/config.json");
        if (!configFile.is_open()) {
            Logger::log("Failed to open config.json");
            std::cerr << "Failed to open config.json\n";
            return 1;
        }

        json config;
        configFile >> config;

        std::string clientId = config["clientId"];
        std::string clientSecret = config["clientSecret"];

        // Create named pipe to send commands to streaming terminal
        HANDLE pipe = CreateFile(
            "\\\\.\\pipe\\OrderStreamPipe", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

        if (pipe == INVALID_HANDLE_VALUE) {
            Logger::log("Failed to open named pipe.");
            std::cerr << "Failed to open named pipe.\n";
            return 1;
        }

        // Command loop for user input
        std::cout << "Order Terminal: Enter commands (e.g., PlaceOrder, ModifyOrder, CancelOrder, FetchOrderBook, Subscribe, Unsubscribe, etc.). Type 'exit' to quit.\n";
        std::cout << "List of supported Commands\n";
        std::cout << "PlaceOrder <symbol> <quantity> <price> <side> <type> \n";
        std::cout << "ModifyOder <adv> <orderId> <amount> <price> <type> \n";
        std::cout << "CancelOrder <orderId> \n";
        std::cout << "Subscribe <mode> {space separated channels} \n";
        std::cout << "Unsubscribe <mode> {space separated channels} \n";
        std::cout << "FetchOrderBook \n\n";
        std::cout << "-------------------------------------------------\n";
        std::cout << "Enter the commands here :- \n";

        while (true) {
            std::string command;
            std::getline(std::cin, command);

            if (command == "exit") {
                std::cout << "Exiting Order Terminal...\n";
                break;
            }

            if (!command.empty()) {
                // Send command to the named pipe
                DWORD bytesWritten;
                if (!WriteFile(pipe, command.c_str(), command.size() + 1, &bytesWritten, NULL)) {
                    std::cerr << "Failed to send command to streaming terminal.\n";
                }
                else {
                    std::cout << "Command sent to streaming terminal.\n";
                }
            }
        }

        CloseHandle(pipe);
    }
    catch (const std::exception& e) {
        Logger::log("Error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
