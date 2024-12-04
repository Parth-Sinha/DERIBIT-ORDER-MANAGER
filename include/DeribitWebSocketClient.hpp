#ifndef DERIBITWEBSOCKETCLIENT_HPP
#define DERIBITWEBSOCKETCLIENT_HPP

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

class DeribitWebSocketClient {
public:
    using client = websocketpp::client<websocketpp::config::asio_tls_client>;
    using connection_hdl = websocketpp::connection_hdl;
    using context_ptr = std::shared_ptr<boost::asio::ssl::context>;

    // Constructor
    DeribitWebSocketClient(const std::string& uri,
        const std::string& client_id,
        const std::string& client_secret);

    // Public Methods
    void connect();
    void authenticate();
    void publicSubscribe(const std::vector<std::string>& channels);
    void privateSubscribe(const std::vector<std::string>& channels);
    void publicUnsubscribe(const std::vector<std::string>& channels);
    void privateUnsubscribe(const std::vector<std::string>& channels);
    void run();

    // Callback setters
    void setMessageCallback(std::function<void(const nlohmann::json&)> callback);

private:
    // Private Methods
    context_ptr onTLSInit(connection_hdl hdl);
    void onOpen(connection_hdl hdl);
    void onClose(connection_hdl hdl);
    void onFail(connection_hdl hdl);
    void onMessage(connection_hdl hdl, client::message_ptr msg);
    void send(const nlohmann::json& payload);

    // Helpers
    void processMethod(const nlohmann::json& msg);
    void processResult(const nlohmann::json& msg);
    void handleSubscriptionData(const nlohmann::json& msg);
    void logError(const std::string& context, const std::string& error);

    // Members
    std::string m_uri;
    std::string m_client_id;
    std::string m_client_secret;
    client m_client;
    connection_hdl m_hdl;
    std::function<void(const nlohmann::json&)> m_messageCallback;
    bool m_isConnected = false;  // Track connection state
    bool m_isAuthenticated = false;  // Track auth state
    std::unique_ptr<nlohmann::json> m_queuedPayload = nullptr;  // Queue message if not connected
};

#endif // DERIBITWEBSOCKETCLIENT_HPP
