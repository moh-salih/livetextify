#pragma once

#include <QString>
#include <memory>
#include <spdlog/spdlog.h>

class Logger {
public:
    // Initialize logger (call once at startup)
    static void initialize(const QString& logFilePath, const QString& appName = "liveTextify");
    
    // Logging methods (static)
    static void debug(const QString& message);
    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message);
    static void critical(const QString& message);
    
    // Set log level
    static void setLevel(const QString& level);
    static QString level();
    
private:
    Logger() = delete;
    ~Logger() = delete;
    
    static std::shared_ptr<spdlog::logger> mLogger;

    // Helper to use std::cerr if logger isn't initialized
    static void fallbackLog(const QString& level, const QString& message);
};
