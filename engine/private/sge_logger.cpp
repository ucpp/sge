#include "sge_logger.h"

namespace SGE
{
    const std::string InfoPrefix = "[INFO]";
    const std::string WarningPrefix = "[WARN]";
    const std::string ErrorPrefix = "[ERROR]";

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

    void Logger::LogMessage(LogLevel level, const std::string& message)
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
        case LogLevel::Info:
            levelStr = InfoPrefix;
            break;
        case LogLevel::Warn:
            levelStr = WarningPrefix;
            break;
        case LogLevel::Error:
            levelStr = ErrorPrefix;
            break;
        }

        *m_stream << timeStr << " " << levelStr << " " << message << std::endl;
    }

    bool Logger::IsStreamValid() const
    {
        return m_stream && m_stream->good();
    }
}