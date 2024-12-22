#ifndef _SGE_LOGGER_H_
#define _SGE_LOGGER_H_

#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <iostream>
#include "sge_singleton.h"

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
        void Log(LogLevel level, const std::string& message);

    private:
        Logger();
        bool IsStreamValid() const;

    private:
        std::unique_ptr<std::ostream> m_stream;
        std::mutex m_mutex;
    };

#ifdef _DEBUG
    #define LOG_INFO(msg) SGE::Logger::Get().Log(SGE::LogLevel::Info, msg)
    #define LOG_WARN(msg) SGE::Logger::Get().Log(SGE::LogLevel::Warn, msg)
    #define LOG_ERROR(msg) SGE::Logger::Get().Log(SGE::LogLevel::Error, msg)
#else
    #define LOG_INFO(msg) 
    #define LOG_WARN(msg) 
    #define LOG_ERROR(msg) 
#endif
}

#endif // !_SGE_LOGGER_H_