#pragma once
#include <fstream>
#include <string>

class Logger {
public:
    enum Level { DEBUG, INFO, WARN, ERROR };
    Logger(const std::string &filepath, Level minLevel = INFO);
    ~Logger();
    void debug(const std::string &msg);
    void info(const std::string &msg);
    void warn(const std::string &msg);
    void error(const std::string &msg);

private:
    void log(const std::string &level, const std::string &msg);
    std::ofstream m_file;
    Level m_minLevel;
};
