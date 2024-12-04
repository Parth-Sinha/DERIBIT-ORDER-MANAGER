#ifndef DERIBIT_CLIENT_HPP
#define DERIBIT_CLIENT_HPP

#include <string>

class DeribitClient {
public:
    DeribitClient(const std::string& clientId, const std::string& clientSecret);

    std::string authenticate();
    std::string sendRequest(const std::string& endpoint, const std::string& body);

    std::string placeOrder(const std::string& instrumentName, double amount, double price, const std::string& side, const std::string& type);
    std::string cancelOrder(const std::string& orderId);
    std::string modifyOrder(const std::string& adv, const std::string& orderId, double amount, double price, const std::string& type);   
    std::string viewCurrentPositions(const std::string& currency, const std::string& kind);
    std::string fetchOrderBook();

private:
    std::string clientId;
    std::string clientSecret;
    std::string accessToken;
    const std::string baseUrl = "https://test.deribit.com/api/v2/";
};

#endif // DERIBIT_CLIENT_HPP
