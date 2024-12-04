//#include "OrderManager.hpp"
//#include "DeribitWebSocketClient.hpp"
//#include "Logger.hpp"
//#include <iostream>
//#include <fstream>
//#include <nlohmann/json.hpp>
//#include <sstream>
//#include <string>
//#include <vector>
//using json = nlohmann::json;
//
//int main() {
//    try {
//        Logger::init("C:/Users/dange/Desktop/OrderManagementSystem/logs/oms_orders.log");
//
//        std::ifstream configFile("C:/Users/dange/Desktop/OrderManagementSystem/config.json");
//        if (!configFile.is_open()) {
//            Logger::log("Failed to open config.json");
//            std::cerr << "Failed to open config.json\n";
//            return 1;
//        }
//
//        json config;
//        configFile >> config;
//
//        std::string clientId = config["clientId"];
//        std::string clientSecret = config["clientSecret"];
//        std::string deribitUri = "wss://test.deribit.com/ws/api/v2";
//
//        // Initialize OrderManager and WebSocket client
//        OrderManager orderManager(clientId, clientSecret);
//        DeribitWebSocketClient wsClient(deribitUri, clientId, clientSecret);
//
//        // Connect WebSocket in a non-blocking manner
//        wsClient.connect();
//        // Start WebSocket client in a separate thread
//        std::thread wsThread([&]() {
//            wsClient.run();  // This runs the WebSocket client loop
//            });
//        std::cout << "Order Terminal: Enter commands (e.g., PlaceOrder, CancelOrder, Subscribe, Unsubscribe, etc.). Type 'exit' to quit.\n";
//        while (true) {
//            std::string command;
//            std::getline(std::cin, command);
//
//            if (command == "exit") {
//                std::cout << "Exiting Order Terminal...\n";
//                break;
//            }
//
//            std::istringstream iss(command);
//            std::string cmd;
//            iss >> cmd;
//
//            if (cmd == "PlaceOrder") {
//                std::string symbol, side, orderType;
//                int quantity;
//                double price;
//                iss >> symbol >> quantity >> price >> side >> orderType;
//                if (iss) orderManager.placeOrder(symbol, quantity, price, side, orderType);
//                else std::cerr << "Invalid PlaceOrder command format.\n";
//            }
//            else if (cmd == "CancelOrder") {
//                std::string orderId;
//                iss >> orderId;
//                if (iss) orderManager.cancelOrder(orderId);
//                else std::cerr << "Invalid CancelOrder command format.\n";
//            }
//            else if (cmd == "ModifyOrder") {
//                std::string adv, orderId, type;
//                double amount, price;
//                iss >> adv >> orderId >> amount >> price >> type;
//                if (iss) orderManager.modifyOrder(adv, orderId, amount, price, type);
//                else std::cerr << "Invalid ModifyOrder command format.\n";
//            }
//            else if (cmd == "Subscribe") {
//                std::string scope;
//                std::vector<std::string> channels;
//
//                iss >> scope;
//                if (scope != "public" && scope != "private") {
//                    std::cerr << "Invalid scope for Subscribe. Must be 'public' or 'private'.\n";
//                    continue;
//                }
//
//                std::string channel;
//                while (iss >> channel) channels.push_back(channel);
//                for (auto s : channels) std::cout << s << "\n";
//                std::cout << "scope is : " << scope << std::endl;
//                if (!channels.empty()) {
//                    if (scope == "public") {
//                        wsClient.publicSubscribe(channels);
//                        std::cout << "Subscribed to: ";
//                        for (const auto& ch : channels) std::cout << ch << " ";
//                        std::cout << "\n";
//                    }
//                    else {
//                        wsClient.privateSubscribe(channels);
//                    }
//                }
//                else {
//                    std::cerr << "Invalid Subscribe command format. Provide at least one channel.\n";
//                }
//            }
//            else if (cmd == "Unsubscribe") {
//                std::string scope;
//                std::vector<std::string> channels;
//
//                iss >> scope;
//                if (scope != "public" && scope != "private") {
//                    std::cerr << "Invalid scope for Unsubscribe. Must be 'public' or 'private'.\n";
//                    continue;
//                }
//
//                std::string channel;
//                while (iss >> channel) channels.push_back(channel);
//                for (auto s : channels) std::cout << s << "\n";
//                std::cout << "scope is : " << scope << std::endl;
//                if (!channels.empty()) {
//                    wsClient.unsubscribe(channels, scope);
//                    std::cout << "Unsubscribed from: ";
//                    for (const auto& ch : channels) std::cout << ch << " ";
//                    std::cout << "\n";
//                }
//                else {
//                    std::cerr << "Invalid Unsubscribe command format. Provide at least one channel.\n";
//                }
//            }
//            else {
//                std::cerr << "Unknown command: " << cmd << "\n";
//            }
//
//        }
//        if (wsThread.joinable()) {
//            wsThread.join();
//        }
//    }
//    catch (const std::exception& e) {
//        Logger::log("Error: " + std::string(e.what()));
//        std::cerr << "Error: " << e.what() << std::endl;
//    }
//
//    return 0;
//}


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
        std::cout << "Order Terminal: Enter commands (e.g., Subscribe, Unsubscribe, etc.). Type 'exit' to quit.\n";
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
