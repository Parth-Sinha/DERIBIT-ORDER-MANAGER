// #include "DeribitWebSocketClient.hpp"
//#include "Logger.hpp"
//#include "OrderManager.hpp"
//#include <iostream>
//#include <fstream>
//#include <boost/asio.hpp>
//#include <nlohmann/json.hpp>
//#include <thread>
//
//
//using json = nlohmann::json;
//
//
////int main() {
////    try {
////        std::cout << "HI, I am in main function.\n";
////        // Initialize logger
////        Logger::init("C:/Users/dange/Desktop/OrderManagementSystem/logs/oms.log");
////
////        // Load clientId and clientSecret from config.json
////        std::ifstream configFile("C:/Users/dange/Desktop/OrderManagementSystem/config.json");
////        if (!configFile.is_open()) {
////            Logger::log("Failed to open config.json");
////            std::cerr << "Failed to open config.json\n";
////            return 1;
////        }
////
////        json config;
////        configFile >> config;
////
////        std::string clientId = config["clientId"];
////        std::string clientSecret = config["clientSecret"];
////        std::string deribitUri = "wss://test.deribit.com/ws/api/v2";
////        std::cout << "from the main function clientId : " << clientId << "\n";
////        // Initialize OrderManager with clientId and clientSecret
////        OrderManager orderManager(clientId, clientSecret);
////        Logger::log("OrderManager initialized");
////
////        DeribitWebSocketClient client(
////            "wss://test.deribit.com/ws/api/v2",
////            clientId,
////            clientSecret
////        );
////        client.connect();
////        //client.authenticate();
////
////        // Subscribe to market data
////        client.publicSubscribe({ "ticker.BTC-PERPETUAL.100ms" }, "private");
////
////        // Run the client in a blocking loop
////        client.run();
////
////
////        // Initialize WebSocketServer
////        //WebSocketServer wsServer;
////        /*
////        Logger::log("WebSocketServer initialized");*/
////
////        // Start WebSocket server in a separate thread
////        //std::thread wsThread([&]() {
////        //    wsServer.run();
////        //    });
////        //Logger::log("WebSocket server thread started");
////        //orderManager.modifyOrder("", "ETH-15489835105", 2, 3500, "limit");
////        //orderManager.cancelOrder("ETH-15489835105");
////        //orderManager.fetchOrderBook();
////        //orderManager.viewCurrentPositions("any", "any");
////
////        //try {
////        //    net::io_context ioc;  // Create a single io_context
////        //    WebSocketServer server(ioc, 8080);  // Pass reference to io_context
////        //    server.run();
////        //}
////        //catch (const std::exception& e) {
////        //    std::cerr << "Fatal error: " << e.what() << std::endl;
////        //    return 1;
////        //}
////
////        /*
////
////        // Simulate order management
////        std::string symbol = "ETH-PERPETUAL";
////        int quantity = 1;
////        double price = 3400;
////        std::string side = "buy";  // Order side
////        std::string type = "limit";
////
////        // Place an order using OrderManager
////        Logger::log("Placing order: " + symbol + ", Quantity: " + std::to_string(quantity) + ", Price: " + std::to_string(price)+ " is called from main");
////        orderManager.placeOrder(symbol, quantity, price, side, type);
////        Logger::log("Order placed successfully");
////
////        // Optionally, cancel an order (uncomment and provide a valid order ID if necessary)
////        // std::string orderId = "your_order_id_here";
////        // orderManager.cancelOrder(orderId);
////        // Logger::log("Order canceled: " + orderId);
////
////
////        */
////        // Join the WebSocket thread
////        //wsThread.join();
////        //Logger::log("WebSocket server thread joined");
////
////    }
////    catch (const std::exception& e) {
////        // Handle exceptions and log errors
////        Logger::log("Error: " + std::string(e.what()));
////        std::cerr << "Error: " << e.what() << std::endl;
////    }
////
////    return 0;
////}
//
//
//int main() {
//    try {
//        // Initialize Logger
//        Logger::init("C:/Users/dange/Desktop/OrderManagementSystem/logs/oms.log");
//
//        // Load configuration
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
//        // Start WebSocket client in a separate thread
//        std::thread wsThread([&]() {
//            wsClient.connect();
//            wsClient.run();
//            });
//
//        // Command loop for handling user input
//        std::cout << "Enter commands (e.g., PlaceOrder, CancelOrder, etc.). Type 'exit' to quit.\n";
//        while (true) {
//            std::string command;
//            std::getline(std::cin, command);
//
//            if (command == "exit") {
//                std::cout << "Exiting Order Management System...\n";
//                break;
//            }
//
//            // Parse command and execute appropriate action
//            std::istringstream iss(command);
//            std::string cmd;
//            iss >> cmd;
//
//            if (cmd == "PlaceOrder") {
//                // Format: PlaceOrder symbol quantity price side orderType
//                std::string symbol, side, orderType;
//                int quantity;
//                double price;
//                iss >> symbol >> quantity >> price >> side >> orderType;
//                if (iss) {
//                    orderManager.placeOrder(symbol, quantity, price, side, orderType);
//                }
//                else {
//                    std::cerr << "Invalid PlaceOrder command format.\n";
//                }
//            }
//            else if (cmd == "CancelOrder") {
//                // Format: CancelOrder orderId
//                std::string orderId;
//                iss >> orderId;
//                if (iss) {
//                    orderManager.cancelOrder(orderId);
//                }
//                else {
//                    std::cerr << "Invalid CancelOrder command format.\n";
//                }
//            }
//            else if (cmd == "ModifyOrder") {
//                // Format: ModifyOrder adv orderId amount price type
//                std::string adv, orderId, type;
//                double amount, price;
//                iss >> adv >> orderId >> amount >> price >> type;
//                if (iss) {
//                    orderManager.modifyOrder(adv, orderId, amount, price, type);
//                }
//                else {
//                    std::cerr << "Invalid ModifyOrder command format.\n";
//                }
//            }
//            else if (cmd == "FetchOrderBook") {
//                orderManager.fetchOrderBook();
//            }
//            else if (cmd == "ViewCurrentPositions") {
//                // Format: ViewCurrentPositions currency kind
//                std::string currency, kind;
//                iss >> currency >> kind;
//                if (iss) {
//                    orderManager.viewCurrentPositions(currency, kind);
//                }
//                else {
//                    std::cerr << "Invalid ViewCurrentPositions command format.\n";
//                }
//            }
//            else {
//                std::cerr << "Unknown command: " << cmd << "\n";
//            }
//        }
//
//        // Wait for WebSocket thread to finish
//        wsThread.join();
//    }
//    catch (const std::exception& e) {
//        Logger::log("Error: " + std::string(e.what()));
//        std::cerr << "Error: " << e.what() << std::endl;
//    }
//
//    return 0;
//}

//#include "DeribitWebSocketClient.hpp"
//#include "Logger.hpp"
//#include <iostream>
//#include <fstream>
//#include <nlohmann/json.hpp>
//using json = nlohmann::json;
//
//int main() {
//    try {
//        Logger::init("C:/Users/dange/Desktop/OrderManagementSystem/logs/oms_streaming.log");
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
//        DeribitWebSocketClient wsClient(deribitUri, clientId, clientSecret);
//
//        wsClient.connect();
//        wsClient.run();
//    }
//    catch (const std::exception& e) {
//        Logger::log("Error: " + std::string(e.what()));
//        std::cerr << "Error: " << e.what() << std::endl;
//    }
//
//    return 0;
//}


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
                        Logger::log("Placed order: " + command);
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
                    Logger::log("Fetched order book.");
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

