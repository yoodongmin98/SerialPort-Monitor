#pragma once
#include "STProgrammer.h"
#include "DataFile.h"
#include <windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include <array>


STProgrammer::STProgrammer()
{
    PATH = Quote(FindCubeProgCLI());
    int a = 0;
}

STProgrammer::~STProgrammer()
{
}



void STProgrammer::Instance(const float _X , const float _Y)
{
    MyWindow().Pos(ImVec2{ 0,0 }).Size(ImVec2{ _X ,_Y });
    MyStyle K;
    K.PushColor(ImGuiCol_WindowBg, Im4Indigo)
        .PushColor(ImGuiCol_Border, Im4White)
        .PushColor(ImGuiCol_Separator, Im4White);
    ImGuiBegin A("##Begins");
    ImGui::SeparatorText("STM Flash Mode");
    CreateRawTextBox();
    CreateButtonSetBox();
    LogScrollCheck();
}


//DebugText LogBox
void STProgrammer::CreateRawTextBox()
{
    ImGuiBeginChild C("RawText(shell)", { 500,700 }, true);
    for (auto& DebugText : DebugLog)
    {
        ImGui::Text(DebugText.c_str());
    }
}


//버튼 및 명령어 전송 Setting Box
void STProgrammer::CreateButtonSetBox()
{
    if (ImGui::Button("Test"))
    {
        args = L"-c port=SWD -ob RDP=0xAA";
        Send(args);
    }
}

//_popen(shell) 명령어 전송 및 DebugText Push_Back
void STProgrammer::Send(std::wstring _CMD)
{
    std::array<char, 128> Erasebuffer;
    std::wstring cmd = L"\"" + PATH + L"\" " + _CMD + L" 2>&1";
    FILE* pipe = _wpopen(cmd.c_str(), L"r");
    while (fgets(Erasebuffer.data(), Erasebuffer.size(), pipe) != nullptr)
    {
        DebugLog.push_back(Erasebuffer.data());
    }
    scrollToBottom = true;
    if (pipe != nullptr)
    {
    	_pclose(pipe);
    }
}

void STProgrammer::LogScrollCheck()
{
    float scrollY = ImGui::GetScrollY();      // 현재 스크롤 위치
    float scrollMaxY = ImGui::GetScrollMaxY();// 스크롤 가능한 최대 위치
    bool isAtBottom = (scrollY >= scrollMaxY - 25);// 현재 스크롤이 맨 아래인지 확인


    if ((scrollToBottom && isAtBottom) || (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)))
    {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }
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
