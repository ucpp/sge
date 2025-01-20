#ifndef _SGE_DIRECTORY_MONITOR_H_
#define _SGE_DIRECTORY_MONITOR_H_

#include "pch.h"
#include "sge_action.h"
#include <thread>
#include <atomic>

namespace SGE
{
    class DirectoryMonitor
    {
    public:
        explicit DirectoryMonitor(const std::string& directory);
        virtual ~DirectoryMonitor();

        void Start();
        void Stop();

        Action<>& OnDirectoryChanged() { return m_directoryChangedEvent; }
    
    private:
        void Monitor();

    private:
        std::string m_directory;
        std::thread m_thread;
        std::atomic<bool> m_isRunning;
        Action<> m_directoryChangedEvent;
        HANDLE m_hDir;

        std::chrono::steady_clock::time_point m_lastModifiedTime;
        const std::chrono::milliseconds m_debounceTime = std::chrono::milliseconds(1000);
    };
}

#endif // !_SGE_DIRECTORY_MONITOR_H_
