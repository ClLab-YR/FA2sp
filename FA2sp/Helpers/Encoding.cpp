#include "Encoding.h"

#include <fstream>

std::string Encoding::Conversion(std::string& in, int fromCodePage, int toCodePage)
{
    // basically decode source string with specific code page, make it wchar_t
    // and then encode wchar_t with the code page we need, make it string again.
    if (in.empty())
        return in;

    int len = MultiByteToWideChar(fromCodePage, 0, in.c_str(), -1, NULL, 0);
    std::wstring wstr(len + 1, '\0');
    MultiByteToWideChar(fromCodePage, 0, in.c_str(), -1, wstr.data(), len);

    len = WideCharToMultiByte(toCodePage, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(len + 1, '\0');
    WideCharToMultiByte(toCodePage, 0, wstr.c_str(), -1, str.data(), len, NULL, NULL);

    if (auto nil = str.find('\0'))
        str.resize(nil);
    return str;
}

// Note: The quantity of times calling this function should be under control,
// otherwise the IO would be very slow.
bool Encoding::IsTextUTF8(const std::string& str)
{
    if (str.empty())
        return false;

    int cnt = 0;
    bool isASCII = true;

    for (int i = 0; i < str.length(); i++)
    {
        int chr = str[i];

        if ((chr & 0x80) == 0)
            continue;
        isASCII = false;

        if (!cnt)
        {
            if ((chr >> 5) == 0b110)        cnt = 1;
            else if ((chr >> 4) == 0b1110)  cnt = 2;
            else if ((chr >> 3) == 0b11110) cnt = 3;
            else if ((chr >> 7) != 0)       return false;
            else {
                if ((chr >> 6) != 0b10)     return false;
                cnt--;
            }
        }
    }

    return !isASCII && cnt == 0;
}

int CINIExt::ParseINI_UTF8(const char* filename)
{
    if (*filename == NULL)
        return 1;

    std::ifstream file(filename, std::ios::in);
    if (!file.good())
        return 2;

    std::string cSec, cLine;
    const auto npos = std::string::npos;
    INISection* curSec = nullptr;

    while (!file.eof())
    {
        std::getline(file, cLine);
        // strip to left side of newline or comment
        cLine.erase(std::find_if(cLine.begin(), cLine.end(), [](const char c) { return c == '\r' || c == '\n' || c == ';'; }), cLine.end());

        if (cLine.empty())
            continue;

        const auto left = cLine.find('[');
        const auto right = cLine.find(']');
        const auto equals = cLine.find('=');

        if (left != npos && right != npos && left < right && (equals == npos || equals > left))
        {
            cSec = cLine.substr(left + 1, right - left - 1);
            if (Encoding::IsTextUTF8(cSec.c_str()))
                cSec = Encoding::Conversion(cSec, CP_UTF8, CP_ACP);
            curSec = this->AddSection(cSec.c_str());
        }
        else if (equals != npos && !cSec.empty())
        {
            ppmfc::CString key = cLine.substr(0, equals).c_str();
            ppmfc::CString val = cLine.substr(equals + 1, cLine.size() - equals - 1).c_str();

            key.Trim();
            val.Trim();

            if ((key == "Name" || cSec == "Triggers" || cSec == "Tags" || cSec == "AITriggerTypes") && Encoding::IsTextUTF8(val.m_pchData))
                val = Encoding::Conversion(val, CP_UTF8, CP_ACP).c_str();

            this->InsertPair(curSec->GetEntities(), key, val);
        }
    }

    file.close();
    return 0;
}