#pragma once

#include <windows.h>
#include <string>

inline std::wstring ToWString(const std::string& str, UINT codePage = CP_ACP)
{
    if (str.empty()) return std::wstring();

    int size = MultiByteToWideChar(codePage, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(codePage, 0, str.c_str(), (int)str.size(), &result[0], size);
    return result;
}

inline std::string ToString(const std::wstring& str, UINT codePage = CP_ACP)
{
    if (str.empty()) return std::string();
    int size = WideCharToMultiByte(codePage, 0, str.c_str(), (int)str.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size, 0);
    WideCharToMultiByte(codePage, 0, str.c_str(), (int)str.size(), &result[0], size, nullptr, nullptr);
    return result;
}


inline std::wstring ToBig(const std::wstring& str)
{
    std::wstring result = str;
    for (size_t i = 0; i < str.size(); i++)
    {
        wchar_t s = str[i];
        s = wchar_t((int)s - 32);
        result[i] = s;
    }

    return result;
}