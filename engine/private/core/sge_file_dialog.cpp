#include "core/sge_file_dialog.h"

namespace SGE
{
    FileDialog::FileDialog(HWND ownerWindow) : m_hwndOwner(ownerWindow) {}

    bool FileDialog::SaveAs(const LPCWSTR& defaultExt, const LPCWSTR& filter)
    {
        wchar_t savePath[MAX_PATH] = {};
        OPENFILENAME ofn = {};
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = m_hwndOwner;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = savePath;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = defaultExt;

        if (GetSaveFileName(&ofn))
        {
            savePathStr = savePath;
            return true;
        }

        return false;
    }
    
    std::string FileDialog::GetSavePath() const
    {
        return ConvertWcharToUtf8(savePathStr.c_str());
    }
    
    std::string FileDialog::ConvertWcharToUtf8(const wchar_t* wcharStr) const
    {
        if (!wcharStr)
        { 
            return "";
        }

        int32 sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, nullptr, 0, nullptr, nullptr);

        if (sizeNeeded == 0)
        {
            return "";
        }

        std::vector<char> utf8Str(sizeNeeded);
        WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, utf8Str.data(), sizeNeeded, nullptr, nullptr);
        std::string result(utf8Str.begin(), utf8Str.end());
        result.erase(result.find_last_not_of('\0') + 1);

        return result;
    }
}