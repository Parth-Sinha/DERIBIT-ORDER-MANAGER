#pragma once
#include <map>
#include <string>
#include <vector>
#include <mutex>

struct Order {
    int id;
    std::string side; // "buy" or "sell"
    int quantity;
    double price;
};

class OrderBook {
public:
    void addOrder(const std::string& symbol, int quantity, double price, const std::string& side);
    void removeOrder(int orderId);
    void display(const std::string& symbol);

private:
    // Buy orders: Descending price
    std::map<double, std::vector<Order>, std::greater<>> buyOrders;
    // Sell orders: Ascending price
    std::map<double, std::vector<Order>> sellOrders;
    std::mutex orderBookMutex;
};
