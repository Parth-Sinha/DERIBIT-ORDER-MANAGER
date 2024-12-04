#include "Logger.hpp"
#include <iostream>
#include <ctime>
#include <iomanip> // Required for std::put_time
#include <stdexcept> // For exceptions

std::ofstream Logger::logFile;
std::mutex Logger::logMutex;

void Logger::init(const std::string& filepath) {
    std::cout << "I am in Logger::init start \n";
    std::lock_guard<std::mutex> lock(logMutex); // Ensure thread safety during initialization
    std::cout << " Hi \n";
    if (logFile.is_open()) {
        std::cout << " Logfile was opened and now closed \n";
        logFile.close(); // Close any previously opened file
    }
    logFile.open(filepath, std::ios::out | std::ios::app);
    std::cout << "Logfile must have been opened at filepath << " << filepath << "\n";
    if (!logFile.is_open()) {
        std::cout << "I am here \n";
        throw std::runtime_error("Failed to open log file: " + filepath);
    }
    std::cout << "Ending init \n";
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (!logFile.is_open()) {
        throw std::runtime_error("Log file is not open. Did you call Logger::init?");
    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    logFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;

    if (!logFile) {
        throw std::runtime_error("Failed to write to log file.");
    }
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.close();
    }
}
