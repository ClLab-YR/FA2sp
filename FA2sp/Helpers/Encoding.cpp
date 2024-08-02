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

    bool isASCII = true;

    auto pre = [](unsigned char byte) -> int
    {
        unsigned char mask = 0x80;
        int num = 0;
        for (int i = 0; i < 8; i++) {   // O(n) per bit
            if ((byte & mask) == mask) {
                mask = mask >> 1;
                num++;
            }
            else break;
        }
        return num;
    };

    for (int num = 0, i = 0; i < str.length(); )  // O(n) per byte
    {
        if ((str[i] & 0x80) == 0) {
            i++;
            continue;
        }
        isASCII = false;
        if ((num = pre(str[i])) > 2) {
            i++;
            for (int j = 0; j < num - 1; j++) {
                if ((str[i] & 0xc0) != 0x80)
                    return false;
                i++;
            }
        }
        else return false;
    }
    return !isASCII;
}
