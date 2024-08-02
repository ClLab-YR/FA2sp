#pragma once

/*
 * ChlorideP:
 *      I'd only like to support UTF-8 -> GBK -> UTF-8 conversion.
 *      Since the default code page of Windows is ANSI (GBK in Chinese Mainland), It may not be easy to just show you utf-8 strings,
 *      so in my case I would firstly convert UTF-8 into GBK. Those originally GBK maps would just read normally.
 * 
 *      However I wrote this in order to always get UTF-8 map, which is more friendly with modern text editors, and Git.
 *      So the saving process would always convert GBK into UTF-8.
 * 
 *      As for Big5 in TaiWan, China and Shift-JIS in Japan, and many other encodings, maybe the macro "CP_ACP" would work, or may not.
 *      Plz forgive me for my laziness (
 */
#include <string>

#include <CINI.h>

namespace Encoding {
    std::string Conversion(std::string& cppstring, int fromCodePage, int toCodePage);

    inline std::string Conversion(const char* cstring, int fromCodePage, int toCodePage) {
        std::string str(cstring);
        return Conversion(str, fromCodePage, toCodePage);
    }

    inline std::string Conversion(ppmfc::CString& ppmfcstring, int fromCodePage, int toCodePage) {
        return Conversion(ppmfcstring.m_pchData, fromCodePage, toCodePage);
    }

    inline std::string Conversion(CString& mfcstring, int fromCodePage, int toCodePage) {
        return Conversion((const char*)mfcstring, fromCodePage, toCodePage);
    }

    bool IsTextUTF8(const std::string& str);
}
