#include "liveTextify/core/Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/cfg/env.h>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>

#include <iostream>

std::shared_ptr<spdlog::logger> Logger::mLogger = nullptr;

void Logger::initialize(const QString& logFilePath, const QString& appName){
    QFileInfo fileInfo(logFilePath);
   
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            info("Failed to create log directory: " + dir.absolutePath());
            return;
        }
    }

    // Create sinks
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.toStdString(), true);

    // Create logger
    mLogger = std::make_shared<spdlog::logger>(appName.toStdString(),spdlog::sinks_init_list{consoleSink, fileSink});
    
    spdlog::set_default_logger(mLogger);
    spdlog::cfg::load_env_levels();
    
    info("=== " + appName + " Starting ===");
    info("Log file created at: " + logFilePath);
}


void Logger::debug(const QString& message){
    mLogger ? mLogger->debug(message.toStdString()) : fallbackLog("DEBUG", message);
}

void Logger::info(const QString& message){
    mLogger ? mLogger->info(message.toStdString()) : fallbackLog("INFO", message);
}

void Logger::warning(const QString& message){
    mLogger ? mLogger->warn(message.toStdString()) : fallbackLog("WARNING", message);
}

void Logger::error(const QString& message){
    mLogger ? mLogger->error(message.toStdString()) : fallbackLog("ERROR", message);
}

void Logger::critical(const QString& message){
    mLogger ? mLogger->critical(message.toStdString()) : fallbackLog("CRITICAL", message);
}

void Logger::setLevel(const QString& level){
    if (!mLogger) return;
    
    spdlog::level::level_enum spdLevel;
    
    if (level == "debug") spdLevel = spdlog::level::debug;
    else if (level == "info") spdLevel = spdlog::level::info;
    else if (level == "warning") spdLevel = spdlog::level::warn;
    else if (level == "error") spdLevel = spdlog::level::err;
    else if (level == "critical") spdLevel = spdlog::level::critical;
    else spdLevel = spdlog::level::info;
    
    mLogger->set_level(spdLevel);
}

QString Logger::level(){
    if (!mLogger) return "info";
    
    auto level = mLogger->level();
    
    switch (level) {
        case spdlog::level::debug: return "debug";
        case spdlog::level::info: return "info";
        case spdlog::level::warn: return "warning";
        case spdlog::level::err: return "error";
        case spdlog::level::critical: return "critical";
        default: return "info";
    }
}

void Logger::fallbackLog(const QString& level, const QString& message) {
    std::cerr << "[" << level.toStdString() << "] " << message.toStdString() << std::endl;
}