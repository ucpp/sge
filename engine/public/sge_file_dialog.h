#ifndef _SGE_FILE_DIALOG_H_
#define _SGE_FILE_DIALOG_H_

#include "pch.h"
#include <commdlg.h>

namespace SGE
{
    class FileDialog
    {
    public:
        explicit FileDialog(HWND ownerWindow = nullptr);
        bool SaveAs(const std::wstring& defaultExt = L"json", const std::wstring& filter = L"All Files\0*.*\0");
        std::string GetSavePath() const;

    private:
        std::string ConvertWcharToUtf8(const wchar_t* wcharStr) const;

    private:
        HWND m_hwndOwner;
        std::wstring savePathStr;
    };
}

#endif // !_SGE_FILE_DIALOG_H_