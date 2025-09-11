#pragma once
#include "STProgrammer.h"
#include <windows.h>
#include <filesystem>
#include <string>
#include <vector>


STProgrammer::STProgrammer()
{
}

STProgrammer::~STProgrammer()
{
}

std::wstring STProgrammer::Quote(const std::wstring& s)
{
    if (s.empty())
        return L"";
    if (s.front() == L'"' && s.back() == L'"') 
        return s;

    return L"\"" + s + L"\"";
}

std::wstring STProgrammer::FindCubeProgCLI()
{
    // PATH 검색
    wchar_t buf[MAX_PATH]{};
    DWORD n = SearchPathW(nullptr, L"STM32_Programmer_CLI.exe", nullptr, MAX_PATH, buf, nullptr);
    if (n && n < MAX_PATH) return buf;

    for (const auto& p : candidates)
        if (std::filesystem::exists(p)) return p;

    // 3) 느리지만 확실한 fallback
    for (const auto& base : 
        {
        LR"(C:\Program Files\STMicroelectronics)",
        LR"(C:\Program Files (x86)\STMicroelectronics)"
        }) {
        std::error_code ec;
        for (auto it = std::filesystem::recursive_directory_iterator(
            base, std::filesystem::directory_options::skip_permission_denied, ec);
            it != std::filesystem::recursive_directory_iterator(); ++it)
        {
            if (ec) continue;
            if (it->is_regular_file(ec) &&
                _wcsicmp(it->path().filename().c_str(), L"STM32_Programmer_CLI.exe") == 0)
                return it->path().wstring();
        }
    }

    return L""; // 못 찾음
}