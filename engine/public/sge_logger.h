#ifndef _SGE_LOGGER_H_
#define _SGE_LOGGER_H_

#include "pch.h"
#include "sge_singleton.h"
#include <sstream>

namespace SGE
{
    enum class LogLevel
    {
        Info,
        Warn,
        Error
    };

    class Logger final : public Singleton<Logger>
    {
        friend class Singleton<Logger>;

    public:
        void SetFileStream(const std::string& fileName = "log.txt");
        void SetConsoleStream();

        template <typename... Args>
        void Log(LogLevel level, const std::string& format, Args... args)
        {
            std::ostringstream messageStream;
            FormatMessage(messageStream, format, args...);
            LogMessage(level, messageStream.str());
        }

    private:
        Logger();
        bool IsStreamValid() const;
        void LogMessage(LogLevel level, const std::string& message);

        template <typename T, typename... Args>
        void FormatMessage(std::ostringstream& stream, const std::string& format, T value, Args... args)
        {
            size_t placeholderPos = format.find("{}");
            if (placeholderPos == std::string::npos)
            {
                stream << format;
                return;
            }

            stream << format.substr(0, placeholderPos);
            stream << value;

            FormatMessage(stream, format.substr(placeholderPos + 2), args...);
        }

        void FormatMessage(std::ostringstream& stream, const std::string& format)
        {
            stream << format;
        }

    private:
        std::unique_ptr<std::ostream> m_stream;
        std::mutex m_mutex;
    };

#ifdef _DEBUG
    #define LOG_INFO(msg, ...) SGE::Logger::Get().Log(SGE::LogLevel::Info, msg, ##__VA_ARGS__)
    #define LOG_WARN(msg, ...) SGE::Logger::Get().Log(SGE::LogLevel::Warn, msg, ##__VA_ARGS__)
    #define LOG_ERROR(msg, ...) SGE::Logger::Get().Log(SGE::LogLevel::Error, msg, ##__VA_ARGS__)
#else
    #define LOG_INFO(msg, ...)
    #define LOG_WARN(msg, ...)
    #define LOG_ERROR(msg, ...)
#endif
}

#endif // !_SGE_LOGGER_H_