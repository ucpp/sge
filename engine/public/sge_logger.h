#ifndef _SGE_LOGGER_H_
#define _SGE_LOGGER_H_

#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <iostream>

namespace SGE
{
    enum class LogLevel
    {
        INFO_LVL,
        WARN_LVL,
        ERROR_LVL
    };

    class Logger final
    {
    public:
        static Logger& Get();

        void SetFileStream(const std::string& fileName = "log.txt");
        void SetConsoleStream();
        void Log(LogLevel level, const std::string& message);

    private:
        Logger();
        ~Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        bool IsStreamValid() const;

    private:
        std::unique_ptr<std::ostream> m_stream;
        std::mutex m_mutex;
    };

#ifdef _DEBUG
    #define LOG_INFO(msg) SGE::Logger::Get().Log(SGE::LogLevel::INFO_LVL, msg)
    #define LOG_WARN(msg) SGE::Logger::Get().Log(SGE::LogLevel::WARN_LVL, msg)
    #define LOG_ERROR(msg) SGE::Logger::Get().Log(SGE::LogLevel::ERROR_LVL, msg)
#else
    #define LOG_INFO(msg) 
    #define LOG_WARN(msg) 
    #define LOG_ERROR(msg) 
#endif
}

#endif // !_SGE_LOGGER_H_