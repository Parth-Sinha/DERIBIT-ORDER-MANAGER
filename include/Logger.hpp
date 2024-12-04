#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <Logger.hpp>


class Logger {
public:
    static void init(const std::string& filepath); // Initialize the logger
    static void log(const std::string& message);  // Log a message
    static void shutdown();                      // Properly close the log file

private:
    static std::ofstream logFile; // The log file stream
    static std::mutex logMutex;   // Mutex to ensure thread-safe logging
};

#endif // LOGGER_HPP
