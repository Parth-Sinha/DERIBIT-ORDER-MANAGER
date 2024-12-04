#include "DeribitClient.hpp"
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>
#include <nlohmann/json.hpp>  // Include nlohmann::json
#include <Logger.hpp>
#include <iostream>

using json = nlohmann::json;  // Alias for convenience

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);

DeribitClient::DeribitClient(const std::string& clientId, const std::string& clientSecret)
    : clientId(clientId), clientSecret(clientSecret) {
}



std::string DeribitClient::authenticate() {
    // Prepare the authentication URL
    std::string authUrl = baseUrl + "public/auth?client_id=" + clientId +
        "&client_secret=" + clientSecret +
        "&grant_type=client_credentials";

    // Use libcurl to send the request
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set up the curl request
        curl_easy_setopt(curl, CURLOPT_URL, authUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        // Set up callback to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            throw std::runtime_error("Authentication failed: " + std::string(curl_easy_strerror(res)));
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        // Parse the JSON response
        try {
            nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);

            // Extract and store the access token
            if (jsonResponse.contains("result") &&
                jsonResponse["result"].contains("access_token")) {
                accessToken = jsonResponse["result"]["access_token"];
                return accessToken;
            }
            else {
                throw std::runtime_error("Invalid authentication response");
            }
        }
        catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("JSON parsing error: " + std::string(e.what()));
        }
    }
    else {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

// Static helper function for writing curl response
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    }
    catch (std::bad_alloc& e) {
        return 0;
    }
}

std::string DeribitClient::sendRequest(const std::string& endpoint, const std::string& body) {
    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Failed to initialize CURL");

    std::string url = baseUrl + endpoint;
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* ptr, size_t size, size_t nmemb, std::string* data) {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
        });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));

    return response;
}

std::string DeribitClient::placeOrder(const std::string& instrumentName, double amount, double price, const std::string& side, const std::string& type) {
    // First, ensure we have a valid access token
    if (accessToken.empty()) {
        authenticate(); // Automatically authenticate if no token exists
    }

    // Prepare the URL with query parameters
    Logger::log(type + " and " + side);
    std::string endpoint = "private/" + side;
    std::string fullUrl = baseUrl + endpoint +
        "?instrument_name=" + instrumentName +
        "&amount=" + std::to_string(static_cast<int>(amount)) +
        "&type=" + type +
        "&label=order_" + std::to_string(std::time(nullptr)); // Unique label

    // If it's a limit order, add price
    if (type == "limit") {
        fullUrl += "&price=" + std::to_string(price);
    }
    Logger::log(fullUrl);
    // Initialize CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Response buffer
    std::string readBuffer;

    try {
        // Set up the curl request
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        // Set up headers
        struct curl_slist* headers = nullptr;
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up callback to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Clean up headers
        curl_slist_free_all(headers);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error("Order placement failed: " + std::string(curl_easy_strerror(res)));
        }

        // Clean up curl
        curl_easy_cleanup(curl);

        // Parse the JSON response
        nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);

        // Check for successful order placement
        if (jsonResponse.contains("result")) {
            return readBuffer; // Return full JSON response
        }
        else {
            // If there's an error in the response
            throw std::runtime_error("Order placement error: " + readBuffer);
        }
    }
    catch (const std::exception& e) {
        // Ensure curl is cleaned up in case of exception
        curl_easy_cleanup(curl);
        throw;
    }
}

std::string DeribitClient::cancelOrder(const std::string& orderId) {
    if (accessToken.empty()) {
        authenticate(); // Automatically authenticate if no token exists
    }
    std::string endpoint = "/private/cancel?order_id=";
    std::string fullUrl = baseUrl + endpoint + orderId;

    // Initialize CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Response buffer
    std::string readBuffer;

    try {
        // Set up the curl request
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        // Set up headers
        struct curl_slist* headers = nullptr;
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up callback to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Clean up headers
        curl_slist_free_all(headers);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error("Order placement failed: " + std::string(curl_easy_strerror(res)));
        }

        // Clean up curl
        curl_easy_cleanup(curl);

        // Parse the JSON response
        nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);

        // Check for successful order placement
        if (jsonResponse.contains("result")) {
            return readBuffer; // Return full JSON response
        }
        else {
            // If there's an error in the response
            throw std::runtime_error("Order placement error: " + readBuffer);
        }
    }
    catch (const std::exception& e) {
        // Ensure curl is cleaned up in case of exception
        curl_easy_cleanup(curl);
        throw;
    }

}

std::string DeribitClient::fetchOrderBook() {
    if (accessToken.empty()) {
        authenticate(); // Automatically authenticate if no token exists
    }
    std::string endpoint = "private/get_open_orders?";
    std::string fullUrl = baseUrl + endpoint;

    // Initialize CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Response buffer
    std::string readBuffer;

    try {
        // Set up the curl request
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        // Set up headers
        struct curl_slist* headers = nullptr;
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up callback to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Clean up headers
        curl_slist_free_all(headers);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error("Order placement failed: " + std::string(curl_easy_strerror(res)));
        }

        // Clean up curl
        curl_easy_cleanup(curl);

        // Parse the JSON response
        nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);

        // Check for successful order placement
        if (jsonResponse.contains("result")) {
            return readBuffer; // Return full JSON response
        }
        else {
            // If there's an error in the response
            throw std::runtime_error("Order placement error: " + readBuffer);
        }
    }
    catch (const std::exception& e) {
        // Ensure curl is cleaned up in case of exception
        curl_easy_cleanup(curl);
        throw;
    }
}

std::string DeribitClient::modifyOrder(const std::string& adv, const std::string& orderId, double amount, double price, const std::string& type) {
    Logger::log("accesstoken is : "+ accessToken);

    if (accessToken.empty()) {
        authenticate(); // Automatically authenticate if no token exists
        Logger::log("after auth : " + accessToken);
    }
    std::string endpoint = "/private/edit?";
    std::string fullUrl = baseUrl + endpoint + "amount=" + std::to_string(static_cast<int>(amount)) + "&order_id=" + orderId + "&price=" + std::to_string(static_cast<int>(price));
    if (adv == "implv" || adv == "usd") {
        fullUrl += "&advanced" + adv;
    }
    Logger::log(fullUrl);
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Response buffer
    std::string readBuffer;

    try {
        // Set up the curl request
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        // Set up headers
        struct curl_slist* headers = nullptr;
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up callback to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Clean up headers
        curl_slist_free_all(headers);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error("Order modification failed: " + std::string(curl_easy_strerror(res)));
        }

        // Clean up curl
        curl_easy_cleanup(curl);

        // Parse the JSON response
        nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);

        // Check for successful order placement
        if (jsonResponse.contains("result")) {
            return readBuffer; // Return full JSON response
        }
        else {
            // If there's an error in the response
            throw std::runtime_error("Order placement error: " + readBuffer);
        }
    }
    catch (const std::exception& e) {
        // Ensure curl is cleaned up in case of exception
        curl_easy_cleanup(curl);
        throw;
    }

}

//future
//option
//spot
//future_combo
//option_combo

std::string DeribitClient::viewCurrentPositions(const std::string& currency, const std::string& kind) {
    if (accessToken.empty()) {
        authenticate();
    }
    std::string endpoint = "/private/get_positions?";
    std::string fullUrl = baseUrl + endpoint;
    if (currency == "any" || currency == "BTC" || currency == "ETH" || currency == "USDC" || currency == "USDT" || currency == "EURR") {
        fullUrl += "currency=" + currency;
    }
    else {
        //throw error
    }
    if (kind == "future" || kind == "option" || kind == "spot" || kind == "future_combo" || kind == "option_combo") {
        fullUrl += "&kind=" + kind;
    }
    else {
        //throw error
    }
    Logger::log(fullUrl);
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Response buffer
    std::string readBuffer;

    try {
        // Set up the curl request
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        // Set up headers
        struct curl_slist* headers = nullptr;
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up callback to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Clean up headers
        curl_slist_free_all(headers);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error("Getting Portfolio failed: " + std::string(curl_easy_strerror(res)));
        }

        // Clean up curl
        curl_easy_cleanup(curl);

        // Parse the JSON response
        nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);

        // Check for successful order placement
        if (jsonResponse.contains("result")) {
            return readBuffer; // Return full JSON response
        }
        else {
            // If there's an error in the response
            throw std::runtime_error("fetching Portfolio failed: " + readBuffer);
        }
    }
    catch (const std::exception& e) {
        // Ensure curl is cleaned up in case of exception
        curl_easy_cleanup(curl);
        throw;
    }

}