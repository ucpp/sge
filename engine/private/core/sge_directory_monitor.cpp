#include "core/sge_directory_monitor.h"
#include "core/sge_logger.h"

namespace SGE
{
    DirectoryMonitor::DirectoryMonitor(const std::string& directory)
    : m_directory(directory)
    , m_thread()
    , m_isRunning(false)
    , m_hDir(INVALID_HANDLE_VALUE)
    {
        if (directory.empty())
        {
            LOG_ERROR("Provided directory path is empty.");
        }
    }


    DirectoryMonitor::~DirectoryMonitor()
    {
        Stop();
    }

    void DirectoryMonitor::Start()
    {
        m_isRunning = true;
        m_thread = std::thread(&DirectoryMonitor::Monitor, this);
    }

    void DirectoryMonitor::Stop()
    {
        if (!m_isRunning)
        {
            return;
        }

        m_isRunning = false;

        if (m_hDir != INVALID_HANDLE_VALUE)
        {
            CancelIoEx(m_hDir, nullptr);
            CloseHandle(m_hDir);
            m_hDir = INVALID_HANDLE_VALUE;
        }

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }


    void DirectoryMonitor::Monitor()
    {
        std::wstring w_directory(m_directory.begin(), m_directory.end());
        m_hDir = CreateFile(w_directory.c_str(),
                            FILE_LIST_DIRECTORY,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            nullptr,
                            OPEN_EXISTING,
                            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                            nullptr);

        if (m_hDir == INVALID_HANDLE_VALUE)
        {
            LOG_ERROR("Failed to open directory for monitoring: {}", m_directory);
            return;
        }

        BYTE buffer[1024]{0};
        DWORD bytesReturned{0};
        OVERLAPPED overlapped = {0};
        overlapped.hEvent = CreateEvent(nullptr, true, false, nullptr);

        if (overlapped.hEvent == nullptr)
        {
            LOG_ERROR("Failed to create event: {}", GetLastError());
            CloseHandle(m_hDir);
            return;
        }

        while (m_isRunning)
        {
            if (!ReadDirectoryChangesW(m_hDir, buffer, sizeof(buffer), true, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, &overlapped, nullptr))
            {
                DWORD dwError = GetLastError();
                if (dwError != ERROR_IO_PENDING)
                {
                    LOG_ERROR("ReadDirectoryChangesW failed with error: {}", dwError);
                    continue;
                }
            }

            DWORD dwWaitStatus = WaitForSingleObject(overlapped.hEvent, INFINITE);
            if (dwWaitStatus != WAIT_OBJECT_0)
            {
                LOG_ERROR("WaitForSingleObject failed with error: {}", GetLastError());
                continue;
            }

            if (!GetOverlappedResult(m_hDir, &overlapped, &bytesReturned, FALSE))
            {
                DWORD dwError = GetLastError();
                if (dwError == ERROR_OPERATION_ABORTED)
                {
                    continue;
                }
                LOG_ERROR("GetOverlappedResult failed with error: {}", dwError);
                continue;
            }

            if (bytesReturned == 0)
            {
                continue;
            }

            if (bytesReturned > sizeof(buffer))
            {
                LOG_WARN("Buffer size exceeded. Consider increasing buffer size.");
                continue;
            }

            FILE_NOTIFY_INFORMATION* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
            
            while (info)
            {
                if (info->Action == FILE_ACTION_MODIFIED)
                {
                    auto now = std::chrono::steady_clock::now();
                    if (now - m_lastModifiedTime > m_debounceTime)
                    {
                        m_directoryChangedEvent.Invoke();
                        m_lastModifiedTime = now;
                    }
                }

                if (info->NextEntryOffset == 0)
                {
                    break;
                }

                info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(info) + info->NextEntryOffset);

                if ((uintptr_t)info >= (uintptr_t)(buffer + sizeof(buffer)))
                {
                    break;
                }
            }
        }

        CloseHandle(overlapped.hEvent);
    }
}
