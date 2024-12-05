#include "DeribitWebSocketClient.hpp"
#include <websocketpp/common/thread.hpp>
#include <Logger.hpp>
#include <thread>
#include <chrono>
#include <stdexcept>

DeribitWebSocketClient::DeribitWebSocketClient(
    const std::string& uri,
    const std::string& client_id,
    const std::string& client_secret
) :
    m_uri(uri),
    m_client_id(client_id),
    m_client_secret(client_secret)
{
    try {
        // Configure WebSocket client
        m_client.set_access_channels(websocketpp::log::alevel::none);
        m_client.set_error_channels(websocketpp::log::elevel::all);

        // Initialize ASIO
        m_client.init_asio();

        // Set TLS initialization handler
        m_client.set_tls_init_handler(std::bind(&DeribitWebSocketClient::onTLSInit, this, std::placeholders::_1));

        // Bind event handlers
        m_client.set_open_handler(std::bind(&DeribitWebSocketClient::onOpen, this, std::placeholders::_1));
        m_client.set_close_handler(std::bind(&DeribitWebSocketClient::onClose, this, std::placeholders::_1));
        m_client.set_fail_handler(std::bind(&DeribitWebSocketClient::onFail, this, std::placeholders::_1));
        m_client.set_message_handler(std::bind(&DeribitWebSocketClient::onMessage, this, std::placeholders::_1, std::placeholders::_2));

        // Start the ASIO loop
        m_client.start_perpetual();
    }
    catch (const std::exception& e) {
        logError("Initialization", e.what());
    }
}

void DeribitWebSocketClient::connect() {
    websocketpp::lib::error_code ec;
    client::connection_ptr con = m_client.get_connection(m_uri, ec);
    if (ec) {
        logError("Connection Creation", ec.message());
        return;
    }
    m_hdl = con->get_handle(); // Store the connection handle
    m_client.connect(con);     // Start the connection
}

void DeribitWebSocketClient::authenticate() {
    using json = nlohmann::json;

    json auth_payload = {
        {"jsonrpc", "2.0"},
        {"method", "public/auth"},
        {"id", 1},
        {"params", {
            {"grant_type", "client_credentials"},
            {"client_id", m_client_id},
            {"client_secret", m_client_secret}
        }}
    };

    send(auth_payload);
}

void DeribitWebSocketClient::publicSubscribe(const std::vector<std::string>& channels) {
    using json = nlohmann::json;
    json subscribe_payload = {
        {"jsonrpc", "2.0"},
        {"method", "public/subscribe"},
        {"id", 42},
        {"params", {
            {"channels", channels}
        }}
    };

    send(subscribe_payload);
}

void DeribitWebSocketClient::privateSubscribe(const std::vector<std::string>& channels) {

    try {
        // Step 1: Ensure WebSocket is open
        if (!m_isConnected) {
            throw std::runtime_error("WebSocket is not connected. Cannot subscribe to private channels.");
        }

        // Step 2: Authenticate if not already authenticated
        if (!m_isAuthenticated) {
            Logger::log("Authenticating WebSocket session...");
            authenticate();  // Assumes this method sends an authentication request and updates internal state
        }
        // Create the JSON request for private subscription
        nlohmann::json request ={
            {"jsonrpc", "2.0"},
            {"method", "private/subscribe"},
            {"id", 41},
            {"params", {
            {"channels", channels}
            }}
        };

        

        // Log and send the request
        send(request);

    }
    catch (const std::exception& e) {
        Logger::log("Error in privateSubscribe: " + std::string(e.what()));
        std::cerr << "Error in privateSubscribe: " << e.what() << std::endl;
    }
}



void DeribitWebSocketClient::publicUnsubscribe(const std::vector<std::string>& channels) {
    try {
        if (!m_isConnected) {
            throw std::runtime_error("WebSocket is not connected. Cannot subscribe to private channels.");
        }

        // Step 2: Authenticate if not already authenticated
        if (!m_isAuthenticated) {
            Logger::log("Authenticating WebSocket session...");
            authenticate();  // Assumes this method sends an authentication request and updates internal state
        }
        nlohmann::json request = {
            {"jsonrpc", "2.0"},
            {"method", "public/unsubscribe"},
            {"id", 22},
            {"params", {
            {"channels", channels}
            }}
        };

        
        send(request);
    }
    catch (const std::exception& e) {
        Logger::log("Error in unsubscribe: " + std::string(e.what()));
        std::cerr << "Error in unsubscribe: " << e.what() << std::endl;
    }
}

void DeribitWebSocketClient::privateUnsubscribe(const std::vector<std::string>& channels) {
    try {
        if (!m_isConnected) {
            throw std::runtime_error("WebSocket is not connected. Cannot subscribe to private channels.");
        }

        // Step 2: Authenticate if not already authenticated
        if (!m_isAuthenticated) {
            Logger::log("Authenticating WebSocket session...");
            authenticate();  // Assumes this method sends an authentication request and updates internal state
        }
        nlohmann::json request = {
            {"jsonrpc", "2.0"},
            {"method", "private/unsubscribe"},
            {"id", 21},
            {"params", {
            {"channels", channels}
            }}
        };


        send(request);
    }
    catch (const std::exception& e) {
        Logger::log("Error in unsubscribe: " + std::string(e.what()));
        std::cerr << "Error in unsubscribe: " << e.what() << std::endl;
    }
}

void DeribitWebSocketClient::run() {
    m_client.run();
}

void DeribitWebSocketClient::onOpen(connection_hdl hdl) {
    // Set the connection flag to true once the connection is opened
    m_isConnected = true;

    // Send any messages that may have been queued
    if (m_queuedPayload) {
        send(*m_queuedPayload);
        m_queuedPayload.reset(); // Clear the queued message after sending
    }

    authenticate(); // Authenticate after the connection is open
}

void DeribitWebSocketClient::onClose(connection_hdl hdl) {
    // Retrieve the connection object to check for close reason
    websocketpp::client<websocketpp::config::asio_tls_client>::connection_ptr con = m_client.get_con_from_hdl(hdl);

    m_isConnected = false;
    m_isAuthenticated = false;

    // Get the close status code and reason
    websocketpp::close::status::value close_code = con->get_remote_close_code();
    std::string close_reason = con->get_remote_close_reason();

    // Check if there is a close code or reason and log it
    if (close_code != websocketpp::close::status::normal) {
        std::cout << "Close code: " << close_code << ", Reason: " << close_reason << std::endl;
    }
    else {
        std::cout << "Normal closure." << std::endl;
    }
}


void DeribitWebSocketClient::onFail(connection_hdl hdl) {
    // Retrieve the connection object to check for failure reason
    websocketpp::client<websocketpp::config::asio_tls_client>::connection_ptr con = m_client.get_con_from_hdl(hdl);

    // You can check the failure reason through the connection object
    std::string error_msg = con->get_ec().message();

    // Log the specific error message if available
    std::cout << "Error message: " << error_msg << std::endl;
}


void DeribitWebSocketClient::onMessage(connection_hdl hdl, client::message_ptr msg) {
    using json = nlohmann::json;
    try {
        // Parse incoming payload
        auto start = std::chrono::high_resolution_clock::now();
        json parsed_msg = json::parse(msg->get_payload());

        // Handle error field if present
        if (parsed_msg.contains("error")) {
            if (parsed_msg["error"].is_object()) {
                std::string error_details = parsed_msg["error"].dump(4);
                logError("WebSocket Error", error_details);
            }
            else if (parsed_msg["error"].is_string()) {
                logError("WebSocket Error", parsed_msg["error"].get<std::string>());
            }
            else {
                logError("WebSocket Error", "Unexpected error format");
            }
            return;
        }

        // Process "method" field
        if (parsed_msg.contains("method") && parsed_msg["method"].is_string()) {
            processMethod(parsed_msg);
        }
        // Process "result" field
        else if (parsed_msg.contains("result")) {
            processResult(parsed_msg);
        }
        // Unrecognized message
        else {
            Logger::log("Unknown message structure: " + parsed_msg.dump(4));
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        Logger::log("Market data processed in " + std::to_string(duration) + " microseconds.");
    }
    catch (const nlohmann::json::exception& e) {
        logError("Message Processing", e.what());
    }
    catch (const std::exception& e) {
        logError("Message Processing", e.what());
    }
}


DeribitWebSocketClient::context_ptr DeribitWebSocketClient::onTLSInit(connection_hdl hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(
        boost::asio::ssl::context::tlsv12_client
    );

    try {
        ctx->set_verify_mode(boost::asio::ssl::verify_none);
        ctx->set_default_verify_paths();

        ctx->set_verify_callback([](bool preverified, boost::asio::ssl::verify_context& ctx) {
            char subject_name[256];
            X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
            X509_NAME_oneline(X509_get_subject_name(cert), subject_name, sizeof(subject_name));
            return preverified; // Return true to accept, false to reject
            });
    }
    catch (const std::exception& e) {
        logError("TLS Initialization", e.what());
    }

    return ctx;
}

void DeribitWebSocketClient::send(const nlohmann::json& payload) {
    websocketpp::lib::error_code ec;

    // If the connection is not yet open, queue the message
    if (!m_isConnected) {
        std::cout << "Connection not yet open, queuing message...\n";
        m_queuedPayload = std::make_unique<nlohmann::json>(payload);
        return; // Return and don't attempt to send the message yet
    }

    // Send the payload when the connection is open
    client::connection_ptr con = m_client.get_con_from_hdl(m_hdl, ec);

    if (ec) {
        logError("Error retrieving connection", ec.message());
        return;
    }

    if (con->get_state() != websocketpp::session::state::open) {
        std::cerr << "Error: Attempted to send message when WebSocket is not open." << std::endl;
        return;
    }

    m_client.send(m_hdl, payload.dump(), websocketpp::frame::opcode::text, ec);

    if (ec) {
        logError("Message Send", ec.message());
    }
    else {
        std::cout << "Message sent successfully." << std::endl;
    }
}



void DeribitWebSocketClient::processMethod(const nlohmann::json& msg) {
    if (msg["method"] == "subscription") {
        handleSubscriptionData(msg);
    }
}

void DeribitWebSocketClient::processResult(const nlohmann::json& msg) {
    if (msg.contains("id") && msg["id"] == 1) { // Match the ID of the authentication request
        if (msg.contains("result")) {
            std::cout << "Authentication Successful!" << std::endl;
            m_isAuthenticated = true;
        }
        else {
            std::cerr << "Authentication Failed!" << std::endl;
        }
    }
}


void DeribitWebSocketClient::handleSubscriptionData(const nlohmann::json& msg) {
    if (msg.contains("params") && msg["params"].contains("data")) {
        nlohmann::json market_data = msg["params"]["data"];
        std::cout << "Market Update: " << market_data.dump(4) << std::endl;
        Logger::log("Market Update: " + market_data.dump(4) + "\n");
    }
}

void DeribitWebSocketClient::logError(const std::string& context, const std::string& error) {
    std::cerr << "[" << context << "] Error: " << error << std::endl;
}
