#include "Logger.h"
#include <ctime>
#include <iomanip>

Logger::Logger(const std::string &filepath, Level minLevel)
    : m_minLevel(minLevel) {
    m_file.open(filepath, std::ios::app);
}

Logger::~Logger() { if (m_file.is_open()) m_file.close(); }

void Logger::log(const std::string &level, const std::string &msg) {
    if (!m_file.is_open()) return;
    auto t = std::time(nullptr);
    m_file << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
           << " [" << level << "] " << msg << std::endl;
}

void Logger::debug(const std::string &msg) { if (m_minLevel <= DEBUG) log("DEBUG", msg); }
void Logger::info(const std::string &msg)  { if (m_minLevel <= INFO)  log("INFO", msg); }
void Logger::warn(const std::string &msg)  { if (m_minLevel <= WARN)  log("WARN", msg); }
void Logger::error(const std::string &msg) { log("ERROR", msg); }
