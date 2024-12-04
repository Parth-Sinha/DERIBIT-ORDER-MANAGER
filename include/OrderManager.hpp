#ifndef ORDER_MANAGER_HPP
#define ORDER_MANAGER_HPP

#include "DeribitClient.hpp"
#include "OrderBook.hpp"
#include <string>

class OrderManager {
public:
    // Constructor that initializes the DeribitClient with clientId and clientSecret
    OrderManager(const std::string& clientId, const std::string& clientSecret);

    // Places an order on Deribit (buy/sell)
    void placeOrder(const std::string& symbol, int quantity, double price, const std::string& side, const std::string& type);

    // Cancels an order on Deribit
    void cancelOrder(const std::string& orderId);

    // Fetches the order book for a specific instrument
    void fetchOrderBook();
    void modifyOrder(const std::string& adv, const std::string& orderId, double amount, double price, const std::string& type);
    void viewCurrentPositions(const std::string& currency, const std::string& kind);

private:
    DeribitClient deribitClient;  // DeribitClient instance for making API calls
    OrderBook orderBook;          // OrderBook instance for managing local orders
};

#endif // ORDER_MANAGER_HPP
