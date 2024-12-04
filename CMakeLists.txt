cmake_minimum_required(VERSION 3.15)
project(OrderManagementSystem)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Add executable
add_executable(oms ${SOURCES} main.cpp)
add_executable(oms_commands ${SOURCES} HandleCommands.cpp)

# Specify Vcpkg toolchain (adjust the path if needed)
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Find Boost and cURL
find_package(Boost REQUIRED COMPONENTS filesystem thread system random date_time)
find_package(CURL REQUIRED)

# Include Boost directories
include_directories(${Boost_INCLUDE_DIRS})

# Include and link directories for cURL
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(oms PRIVATE ${CURL_LIBRARIES})
target_link_libraries(oms_commands PRIVATE ${CURL_LIBRARIES})

# Link Boost libraries
target_link_libraries(oms PRIVATE Boost::filesystem Boost::thread Boost::system Boost::random Boost::date_time)
target_link_libraries(oms_commands PRIVATE Boost::filesystem Boost::thread Boost::system Boost::random Boost::date_time)
find_package(OpenSSL REQUIRED)
target_link_libraries(oms PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(oms_commands PRIVATE ${OPENSSL_LIBRARIES})
# Add nlohmann_json (JSON for Modern C++) library
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(oms PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(oms_commands PRIVATE nlohmann_json::nlohmann_json)

find_package(websocketpp CONFIG REQUIRED)
target_link_libraries(oms PRIVATE websocketpp::websocketpp)
target_link_libraries(oms_commands PRIVATE websocketpp::websocketpp)

# Set Windows target and C++17 compatibility fixes
target_compile_definitions(oms PRIVATE _HAS_CXX17=1 _WIN32_WINNT=0x0A00)
target_compile_definitions(oms_commands PRIVATE _HAS_CXX17=1 _WIN32_WINNT=0x0A00)

# Additional compiler options to suppress warnings (optional)
if (MSVC)
    target_compile_options(oms PRIVATE /W4 /WX-) # Enable warnings, but don't treat them as errors
    target_compile_options(oms_commands PRIVATE /W4 /WX-) # Enable warnings, but don't treat them as errors
else()
    target_compile_options(oms PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)
    target_compile_options(oms_commands PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)
endif()
