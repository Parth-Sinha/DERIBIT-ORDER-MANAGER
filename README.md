# Deribit Order Manager

## Overview
**Deribit Order Manager** is a C++ application built for cryptocurrency trading on the **Deribit Testnet API**. It enables users to manage orders, subscribe to real-time market data, and interact with WebSocket streams efficiently.

---

## Features

### **Order Management**
1. Place orders (buy/sell) for specified symbols.
2. Cancel orders by their unique ID.
3. Modify existing orders (change price, quantity, or type).
4. Fetch the current order book for any trading pair.
5. View open trading positions.

### **Real-Time Market Data**
- Subscribe to public or private WebSocket feeds.
- Stream continuous updates for order book data or user-specific events.
- Unsubscribe from feeds dynamically.

### **WebSocket Server**
- Allows external clients to connect via WebSocket for live market updates.
- Streams continuous order book updates for subscribed symbols.

---

## Commands

### **Order Management Commands**
- **Place Order**:
  - Command: `PlaceOrder <symbol> <quantity> <price> <side> <type>`
  - Example:
    ```
    PlaceOrder ETH-PERPETUAL 1 3124 buy limit
    ```

- **Cancel Order**:
  - Command: `CancelOrder <orderId>`


https://github.com/user-attachments/assets/aed21bac-1e12-4a4a-b558-c67ba66d0bc0



- **Modify Order**:
  - Command: `ModifyOrder <orderId> <amount> <price> <type>`
  - Example:
    ```
    ModifyOrder 12345 2 31000 limit
    ```

- **Fetch Order Book**:
  - Command: `FetchOrderBook`
  - Example:
    ```
    FetchOrderBook
    ```





https://github.com/user-attachments/assets/98f034cf-36a2-4664-b8d3-36f5af946ce2



### **WebSocket Commands**
- **Subscribe to Market Data**:
  - Command: `Subscribe <scope> <channel1> <channel2> ...`
  - Example:
    ```
    Subscribe public ticker.BTC-PERPETUAL.100ms ticker.ETH-PERPETUAL.100ms
    ```

- **Unsubscribe from Market Data**:
  - Command: `Unsubscribe <scope> <channel1> <channel2> ...`
  - Example:
    ```
    Unsubscribe private user.orders.BTC-PERPETUAL.raw
    ```

---

## File Structure
```
deribit_order_manager/
│
├── build/                       # Build output directory
├── include/                     # Header files
│   ├── DeribitClient.hpp        # HTTP client logic
│   ├── DeribitWebSocketClient.hpp # WebSocket client logic
│   ├── Logger.hpp               # Logging utilities
│   ├── orderBook.hpp            # Order book management
│   ├── orderManager.hpp         # Order management functions
│
├── logs/                        # Log files
│   └── *.log                    # Runtime logs
│
├── src/                         # Source files for core functionality
│   ├── DeribitClient.cpp        # Implements HTTP client logic
│   ├── DeribitWebSocketClient.cpp # Implements WebSocket client
│   ├── Logger.cpp               # Implements logging utilities
│   ├── orderBook.cpp            # Implements order book management
│   ├── orderManager.cpp         # Implements order management
│
├── CMakeLists.txt               # CMake build configuration
├── main.cpp                     # Entry point for the application
├── HandleCommands.cpp           # Command processing logic
├── vcpkg.json                   # Dependency configuration
├── config.json                  # API keys and application configuration
```




---

## Tech Stack

1. **C++17**: Core application logic with modern language features.
2. **cURL**: HTTP communication for interacting with the Deribit REST API.
3. **WebSocketPP**: WebSocket implementation for real-time data streaming.
4. **Boost**: Utility library for threading, networking, and other system functions.
5. **nlohmann/json**: Lightweight and efficient JSON parsing for API responses.

---

# Setup Guide for Deribit Order Manager

## Prerequisites

1. **Compiler**:
   - Ensure you have a C++ compiler with **C++17** support:
     - GCC 9+ / Clang 9+ / MSVC 2019+.

2. **Build System**:
   - Install **CMake** (version 3.10 or later).

3. **Dependency Manager**:
   - The project uses `vcpkg` for managing dependencies.

4. **Network Access**:
   - An internet connection is required to interact with the Deribit Testnet API and download dependencies.

---

## Step-by-Step Installation

### 1. Clone the Repository
Download the project repository from GitHub:
```bash
git clone https://github.com/yourusername/deribit_order_manager.git
cd deribit_order_manager
```
### 2. Install Dependencies Using vcpkg
The project includes a vcpkg.json file to streamline dependency installation. Follow these steps:

Bootstrap vcpkg: If vcpkg is not already installed, follow these commands:

```bash

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
```
Install Required Dependencies: Run the following command in the deribit_order_manager root directory to automatically install dependencies defined in vcpkg.json:

```bash

./vcpkg/vcpkg install
Integrate vcpkg with CMake: Add the vcpkg toolchain to your CMake configuration:
```
```bash

export VCPKG_ROOT=path_to_vcpkg_directory
```
### 3. Configure the Project
Create a build directory:

```bash

mkdir build
cd build
Run the CMake configuration command:
```
```bash

cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
```
### 4. Build the Application
Compile the project using the following:
```bash

cmake --build . --config Release
```
### 5. Configure API Keys
Before running the application, you need to provide your Deribit Testnet API credentials.

Open the config.json file located in the project root.
Add your credentials:
```json

{
    "clientId": "your_client_id_here",
    "clientSecret": "your_client_secret_here"
}
```
### 6. Run the Application
Navigate to the build directory:

```bash

cd build
Execute the application:
```
```bash

./deribit_order_manager
```
### CODE WALKTHROUGH


https://github.com/user-attachments/assets/38de3cc2-3fc9-465b-8eb4-dd3431806726


