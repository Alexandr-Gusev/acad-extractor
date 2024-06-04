#include "utils.h"

std::string utf8(const std::wstring& s)
{
    std::string res;
    unsigned length = WideCharToMultiByte
    (
        CP_UTF8, // CodePage
        0, // dwFlags
        s.c_str(), // lpWideCharStr
        -1, // cchWideChar
        0, // lpMultiByteStr
        0, // cchMultiByte
        0, // lpDefaultChar
        0 // lpUsedDefaultChar
    );
    char* buffer = new char[length];
    if
    (
        WideCharToMultiByte
        (
            CP_UTF8, // CodePage
            0, // dwFlags
            s.c_str(), // lpWideCharStr
            -1, // cchWideChar
            buffer, // lpMultiByteStr
            length, // cchMultiByte
            0, // lpDefaultChar
            0 // lpUsedDefaultChar
        )
    )
        res = buffer;
    delete [] buffer;
    return res;
}
