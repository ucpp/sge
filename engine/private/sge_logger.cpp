#include "sge_logger.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>

namespace SGE
{
    const std::string INFO_STR = "[INFO]";
    const std::string WARN_STR = "[WARN]";
    const std::string ERROR_STR = "[ERROR]";

    Logger& Logger::Get()
    {
        static Logger instance;
        return instance;
    }

    Logger::Logger() : m_stream(std::make_unique<std::ofstream>("log.txt"))
    {
        if (!IsStreamValid())
        {
            m_stream = std::make_unique<std::ostream>(std::cout.rdbuf());
        }
    }

    void Logger::SetFileStream(const std::string& fileName)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stream = std::make_unique<std::ofstream>(fileName);
        
        if (!IsStreamValid())
        {
            m_stream = std::make_unique<std::ostream>(std::cout.rdbuf());
        }
    }

    void Logger::SetConsoleStream()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stream = std::make_unique<std::ostream>(std::cout.rdbuf());
    }

    void Logger::Log(LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!IsStreamValid())
        {
            std::cerr << "[ERROR] Failed to write log message. Stream is not valid." << std::endl;
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::tm localTime = {};
        
#ifdef _MSC_VER // MSVC-specific block
        localtime_s(&localTime, &time);
#else
        std::tm* tmPtr = std::localtime(&time);
        if (tmPtr == nullptr)
        {
            std::cerr << "[ERROR] Failed to get local time." << std::endl;
            return;
        }
        localTime = *tmPtr;
#endif

        std::ostringstream timeStream;
        timeStream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        std::string timeStr = timeStream.str();

        std::string levelStr;
        switch (level)
        {
        case LogLevel::INFO_LVL:
            levelStr = INFO_STR;
            break;
        case LogLevel::WARN_LVL:
            levelStr = WARN_STR;
            break;
        case LogLevel::ERROR_LVL:
            levelStr = ERROR_STR;
            break;
        }

        *m_stream << timeStr << " " << levelStr << " " << message << std::endl;
    }

    bool Logger::IsStreamValid() const
    {
        return m_stream && m_stream->good();
    }
}
