#include "OrderBook.hpp"
#include <iostream>

void OrderBook::addOrder(const std::string& symbol, int quantity, double price, const std::string& side) {
    std::lock_guard<std::mutex> lock(orderBookMutex);
    Order order = { static_cast<int>(std::rand()), side, quantity, price };

    if (side == "buy") {
        buyOrders[price].push_back(order);
    }
    else if (side == "sell") {
        sellOrders[price].push_back(order);
    }
}

void OrderBook::removeOrder(int orderId) {
    std::lock_guard<std::mutex> lock(orderBookMutex);

    for (auto& [price, orders] : buyOrders) {
        orders.erase(std::remove_if(orders.begin(), orders.end(), [&](const Order& o) { return o.id == orderId; }), orders.end());
    }

    for (auto& [price, orders] : sellOrders) {
        orders.erase(std::remove_if(orders.begin(), orders.end(), [&](const Order& o) { return o.id == orderId; }), orders.end());
    }
}

void OrderBook::display(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(orderBookMutex);

    std::cout << "Order Book for: " << symbol << std::endl;
    std::cout << "--- BUY ---" << std::endl;
    for (const auto& [price, orders] : buyOrders) {
        for (const auto& order : orders) {
            std::cout << "ID: " << order.id << " Qty: " << order.quantity << " Price: " << price << std::endl;
        }
    }

    std::cout << "--- SELL ---" << std::endl;
    for (const auto& [price, orders] : sellOrders) {
        for (const auto& order : orders) {
            std::cout << "ID: " << order.id << " Qty: " << order.quantity << " Price: " << price << std::endl;
        }
    }
}
