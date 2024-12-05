#include "DeribitWebSocketClient.hpp"
#include "OrderManager.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <windows.h>  // For named pipes on Windows
#include <string>
#include <vector>
#include <sstream>
#include <thread>
using json = nlohmann::json;

int main() {
    try {
        Logger::init("C:/Users/dange/Desktop/OrderManagementSystem/logs/oms_streaming.log");

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
        std::string deribitUri = "wss://test.deribit.com/ws/api/v2";

        // Initialize WebSocket client and OrderManager
        DeribitWebSocketClient wsClient(deribitUri, clientId, clientSecret);
        OrderManager orderManager(clientId, clientSecret);

        // Create a named pipe to receive commands
        HANDLE pipe = CreateNamedPipe(
            "\\\\.\\pipe\\OrderStreamPipe", PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1, 0, 0, 0, NULL);

        if (pipe == INVALID_HANDLE_VALUE) {
            Logger::log("Failed to create named pipe.");
            std::cerr << "Failed to create named pipe.\n";
            return 1;
        }

        // Wait for the command terminal to connect
        std::cout << "Waiting for commands from the order terminal...\n";
        ConnectNamedPipe(pipe, NULL);

        // Start WebSocket client in a separate thread
        std::thread wsThread([&]() {
            wsClient.connect();
            wsClient.run();
            });

        // Listen for commands via named pipe
        char buffer[1024];
        while (true) {
            DWORD bytesRead;
            if (ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
                buffer[bytesRead] = '\0'; // Properly null-terminate the string
                std::string command(buffer);

                std::istringstream iss(command);
                std::string cmd;
                iss >> cmd;

                if (cmd == "Subscribe") {
                    std::string scope;
                    std::vector<std::string> channels;

                    iss >> scope;
                    std::string channel;
                    while (iss >> channel) channels.push_back(channel);

                    if (!channels.empty()) {
                        if (scope == "public") {
                            wsClient.publicSubscribe(channels);
                            Logger::log("Subscribed to public channels: " + command);
                        }
                        else if (scope == "private") {
                            wsClient.privateSubscribe(channels);
                            Logger::log("Subscribed to private channels: " + command);
                        }
                        else {
                            std::cerr << "Invalid subscription scope.\n";
                        }
                    }
                    else {
                        std::cerr << "No channels provided for subscription.\n";
                    }
                }
                else if (cmd == "Unsubscribe") {
                    std::string scope;
                    std::vector<std::string> channels;

                    iss >> scope;
                    std::string channel;
                    while (iss >> channel) channels.push_back(channel);

                    if (!channels.empty()) {
                        if (scope == "public") {
                            wsClient.publicUnsubscribe(channels);
                            Logger::log("Subscribed to public channels: " + command);
                        }
                        else if (scope == "private") {
                            wsClient.privateUnsubscribe(channels);
                            Logger::log("Subscribed to private channels: " + command);
                        }
                        else {
                            std::cerr << "Invalid subscription scope.\n";
                        }
                    }
                }
                else if (cmd == "PlaceOrder") {
                    std::string symbol, side, type;
                    int quantity;
                    double price;

                    iss >> symbol >> quantity >> price >> side >> type;
                    if (iss) {
                        orderManager.placeOrder(symbol, quantity, price, side, type);
                    }
                    else {
                        std::cerr << "Invalid PlaceOrder command format.\n";
                    }
                }
                else if (cmd == "ModifyOrder") {
                    std::string adv,orderId, type;
                    double amount, price;

                    iss >> adv>>orderId >> amount >> price >> type;
                    if (iss) {
                        orderManager.modifyOrder(adv, orderId, amount, price, type);
                        Logger::log("Modified order: " + command);
                    }
                    else {
                        std::cerr << "Invalid ModifyOrder command format.\n";
                    }
                }
                else if (cmd == "CancelOrder") {
                    std::string orderId;

                    iss >> orderId;
                    if (iss) {
                        orderManager.cancelOrder(orderId);
                        Logger::log("Canceled order: " + command);
                    }
                    else {
                        std::cerr << "Invalid CancelOrder command format.\n";
                    }
                }
                else if (cmd == "FetchOrderBook") {
                    orderManager.fetchOrderBook();
                }
                else {
                    std::cerr << "Unknown command: " << cmd << "\n";
                }
            }
        }

        CloseHandle(pipe);
        if (wsThread.joinable()) {
            wsThread.join();
        }

    }
    catch (const std::exception& e) {
        Logger::log("Error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

