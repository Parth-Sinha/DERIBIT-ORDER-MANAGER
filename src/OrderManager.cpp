#include "OrderManager.hpp"
#include "DeribitClient.hpp"
#include "Logger.hpp"
#include <iostream>
#include <chrono>

OrderManager::OrderManager(const std::string& clientId, const std::string& clientSecret)
    : deribitClient(clientId, clientSecret) {
}

void OrderManager::placeOrder(const std::string& symbol, int quantity, double price, const std::string& side, const std::string& orderType) {
    Logger::log("Placing order: " + symbol + ", Quantity: " + std::to_string(quantity) + ", Price: " + std::to_string(price) + ", Side: " + side);

    // Add the order to the local order book
    orderBook.addOrder(symbol, quantity, price, side);
    auto start = std::chrono::steady_clock::now();

    try {
       

        // Place the order on the exchange
        std::string response = deribitClient.placeOrder(
            symbol,       // instrument name (e.g., "ETH-PERPETUAL")
            quantity,     // amount
            price,        // price (optional, depends on order type)
            side,
            orderType     // order type
        );
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        Logger::log("Order placed successfully in " + std::to_string(duration.count()) + " ms: " + response);
        Logger::log("Order placed successfully: " + response);
    }
    catch (const std::exception& e) {
        Logger::log("Error placing order: " + std::string(e.what()));
    }
}

void OrderManager::cancelOrder(const std::string& orderId) {
    Logger::log("Cancelling order: " + orderId);
    auto start = std::chrono::steady_clock::now();

    try {
        std::string response = deribitClient.cancelOrder(orderId);
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        Logger::log("Cancelled Order successfully in " + std::to_string(duration.count()) + " ms: " + response);
        Logger::log("Order canceled successfully: " + response);
    }
    catch (const std::exception& e) {
        Logger::log("Error cancelling order: " + std::string(e.what()));
    }
}

void OrderManager::fetchOrderBook() {
    Logger::log("Fetching order book ...");

    try {
        auto start = std::chrono::steady_clock::now();
        std::string response = deribitClient.fetchOrderBook();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        Logger::log("fetching successfully in " + std::to_string(duration.count()) + " ms: " + response);
        Logger::log("Order book fetched successfully: " + response);
    }
    catch (const std::exception& e) {
        Logger::log("Error fetching order book: " + std::string(e.what()));
    }
}

void OrderManager::modifyOrder(const std::string& adv, const std::string& orderId, double amount, double price, const std::string& type) {
    Logger::log("Modifying order...");
    try {
        std::string response = deribitClient.modifyOrder(adv, orderId, amount, price, type);
        Logger::log("Order modified successfully");
    }
    catch (const std::exception& e) {
        Logger::log("Error modifying order ... " + std::string(e.what()));
    }
}

void OrderManager::viewCurrentPositions(const std::string& currency, const std::string& kind) {
    Logger::log("fetching portfolio....");
    try {
        std::string response = deribitClient.viewCurrentPositions(currency, kind);
        Logger::log("fetched successfully...  " + response);
    }
    catch (const std::exception& e) {
        Logger::log("Error modifying order ... " + std::string(e.what()));
    }
}
