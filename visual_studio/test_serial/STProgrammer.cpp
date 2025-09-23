#pragma once
#include "STProgrammer.h"
#include "DataFile.h"
#include <windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include <array>
#include "ThreadPool.h"


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

    {
        AutoRunBox();
        CreateButtonSetBox();
        CreateRawTextBox();
    }

}

//자동감지 Box생성
void STProgrammer::AutoRunBox()
{
    ImGui::Checkbox("Auto Run on connect", &autoRun);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::InputInt("Interval(ms)", &poll_interval_ms_);
    //최소 0.2 m/s까지만 자동인식 가능(Polling문제 방지)
    if (poll_interval_ms_ < 200)
    {
        poll_interval_ms_ = 200;
    }

    //비동기로 Polling에 작업인식
    if (autoRun && !polling_.load())
    {
        StartPolling(poll_interval_ms_, true);
    }
    if (!autoRun && polling_.load())
    {
        StopPolling();
    }
}

// 폴링 시작 (연결되면 1회 실행. 재연결 시 재실행하려면 armed_ 리셋)
void STProgrammer::StartPolling(int interval_ms /*=1000*/, bool rerun_on_reconnect /*=true*/)
{
    if (polling_.exchange(true)) return;
    poll_interval_ms_ = interval_ms;
    armed_.store(true);

    poll_thread_ = std::thread([this, rerun_on_reconnect] {
        bool last = false;
        while (polling_.load()) {
            bool now = CheckConnected();

            // 막 붙은 순간 && 실행 허용 && 현재 실행 중 아님
            if (now && !last && armed_.load() && !running_.load()) {
                RunSequenceAsync();
                armed_.store(false);       // 1회만
            }
            // 재연결 시 다시 한 번 실행하려면: 끊겼다가 붙을 때 arm
            if (rerun_on_reconnect && !now && last) {
                armed_.store(true);
            }

            last = now;
            std::this_thread::sleep_for(std::chrono::milliseconds(poll_interval_ms_));
        }
        });
}

// 폴링 중지
void STProgrammer::StopPolling()
{
    if (!polling_.exchange(false))
    {
        return;
    }
    if (poll_thread_.joinable())
    {
        poll_thread_.join();
    }
}

bool STProgrammer::CheckConnected()
{
    if (PATH.empty())
    {
        return false;
    }
    //HotPlug명령어 넣어보고 바로 close해서 내부 버퍼가 쌓여있는지 확인한 뒤
    //내부 버퍼에 데이터가 있다면 연결을 인식하고 true반환 아닐시 false
    const std::wstring cmd = L"\"" + PATH + L"\" -c port=SWD mode=HOTPLUG -q 2>&1"; 
    if (FILE* p = _wpopen(cmd.c_str(), L"rt")) 
    {
        const int ec = _pclose(p);         // 0이면 연결 OK
        return (ec == 0);
    }
    return false;
}

void STProgrammer::RunSequenceAsync()
{
    if (running_.exchange(true))
    {
        return;   // 이미 실행 중이면 무시
    }

    //프레임마다 Instancing되는 불안정한 this의 포인터 손실 방지
    auto self = shared_from_this();
    std::thread([self]
        {
            self->RunSequence();               // 동기 실행 (아래 RunSequence 수정)
            self->running_.store(false);
        }
    ).detach();
}

//내부 vector(rows)에 담긴 cmd 순차적 실행
bool STProgrammer::RunSequence()
{
    for (size_t i = 0; i < rows.size(); ++i)
    {
        if (!rows[i].use)
        {
            continue;
        }

        std::wstring wargs = ToW(rows[i].args);
        Send(wargs);                        // 블로킹 호출 → 순차 실행
        if (stopOnError)
        {
            // 실패 감지 로직 있으면 여기서 break/return false 때에 따라 맞춰 넣으면됨(터트리진 말기)
        }
    }
    return true;
}

// _popen(shell) 명령어 전송 및 DebugText Push_Back
// 제발 락을 빼지 말아주세요
void STProgrammer::Send(std::wstring _CMD)
{
    std::array<char, 128> buf{};
    std::wstring cmd = L"\"" + PATH + L"\" " + _CMD + L" 2>&1";
    FILE* pipe = _wpopen(cmd.c_str(), L"rt");

    if (!pipe)
    {
        return;
    }

    while (fgets(buf.data(), buf.size(), pipe))
    {
        std::lock_guard<std::mutex> lk(log_mtx);
        DebugLog.emplace_back(buf.data());
        scrollToBottom = true;
    }

    _pclose(pipe);
}


//세팅되는 ST_CLI버튼 Table Setting
void STProgrammer::CreateButtonSetBox()
{
    ImGui::SeparatorText("STM32 Programmer - Command Table");

    // 옵션
    ImGui::Checkbox("Stop on error", &stopOnError);
    ImGui::SameLine();
    if (ImGui::SmallButton("+ Add Row")) 
    {
        rows.push_back(CmdRow{ true, "New", "-c port=SWD" });
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear All")) 
    {
        rows.clear();
    }

    // Table
    if (ImGui::BeginTable("st_cmd_table", 6,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Use");
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 250.0f);
        ImGui::TableSetupColumn("Args", ImGuiTableColumnFlags_WidthFixed, 450.0f);
        ImGui::TableSetupColumn("Up");
        ImGui::TableSetupColumn("Down");
        ImGui::TableSetupColumn("Del");
        ImGui::TableHeadersRow();

        for (int i = 0; i < (int)rows.size(); ++i)
        {
            ImGui::TableNextRow();

            // Use
            ImGui::TableSetColumnIndex(0);
            ImGui::Checkbox(("##use" + std::to_string(i)).c_str(), &rows[i].use);

            // Label
            ImGui::TableSetColumnIndex(1);
            char lblBuf[128];
            std::snprintf(lblBuf, sizeof(lblBuf), "%s", rows[i].label.c_str());
            if (ImGui::InputText(("##lbl" + std::to_string(i)).c_str(), lblBuf, sizeof(lblBuf)))
                rows[i].label = lblBuf;

            // Args (UTF-8)
            ImGui::TableSetColumnIndex(2);
            
            std::snprintf(argBuf, sizeof(argBuf), "%s", rows[i].args.c_str());
            if (ImGui::InputText(("##args" + std::to_string(i)).c_str(), argBuf, sizeof(argBuf)))
            {
                rows[i].args = argBuf;
            }

            // Up
            ImGui::TableSetColumnIndex(3);
            if (ImGui::SmallButton(("^##" + std::to_string(i)).c_str()))
            {
                MoveRow(rows, i, i - 1);
            }

            // Down
            ImGui::TableSetColumnIndex(4);
            if (ImGui::SmallButton(("v##" + std::to_string(i)).c_str()))
            {
                MoveRow(rows, i, i + 1);
            }

            // Delete
            ImGui::TableSetColumnIndex(5);
            if (ImGui::SmallButton(("X##" + std::to_string(i)).c_str()))
            {
                rows.erase(rows.begin() + i);
                --i;
                continue;
            }

            // 행 선택
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
            {
                selected = i;
            }

        }
        ImGui::EndTable();
    }

    // 실행 버튼
    ImGui::Spacing();
    if (ImGui::Button("Run Sequence", ImVec2(200, 0)) || ImGui::IsKeyPressed(ImGuiKey_Space))
    {
        RunSequence();
    }
}

//DebugText LogBox
void STProgrammer::CreateRawTextBox()
{
    ImGuiBeginChild C("RawText(shell)", { 700,550 }, true);
    {
        std::lock_guard<std::mutex> lk(log_mtx);
        for (auto& DebugText : DebugLog)
        {
            ImGui::Text(DebugText.c_str());
            scrollToBottom = true;
            LogScrollCheck();
        }
    }
}



//DebugLog에 데이토거 들어와서 scrollToBottom에 값이 인식되면 아래로 내림
void STProgrammer::LogScrollCheck()
{
    float scrollY = ImGui::GetScrollY();      // 현재 스크롤 위치
    float scrollMaxY = ImGui::GetScrollMaxY();// 스크롤 가능한 최대 위치
    bool isAtBottom = (scrollY >= scrollMaxY - 25);// 현재 스크롤이 맨 아래인지 확인 여유값은 맘대로(현재25pixel)


    if ((scrollToBottom && isAtBottom) || (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)))
    {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }
}




///////////////////////HELPER///////////////////////
std::wstring STProgrammer::Quote(const std::wstring& s)
{
    if (s.empty())
    {
        return L"";
    }

    if (s.front() == L'"' && s.back() == L'"')
    {
        return s;
    }

    return L"\"" + s + L"\"";
}


std::wstring STProgrammer::FindCubeProgCLI()
{
    // PATH 검색
    wchar_t buf[MAX_PATH]{};
    DWORD n = SearchPathW(nullptr, L"STM32_Programmer_CLI.exe", nullptr, MAX_PATH, buf, nullptr);
    if (n && n < MAX_PATH) return buf;

    for (const auto& p : candidates)
    {
        if (std::filesystem::exists(p))
        {
            return p;
        }
    }

    // 느리지만 확실한 fallback. 추후 문제생기면 시스템 환경변수설정으로 바꿔야함
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
            if (ec)
            {
                continue;
            }

            if (it->is_regular_file(ec) && _wcsicmp(it->path().filename().c_str(), L"STM32_Programmer_CLI.exe") == 0)
            {
                return it->path().wstring();
            }
        }
    }

    return L""; // 못 찾았을 때
}


std::wstring STProgrammer::ToW(const std::string& s) 
{
    if (s.empty())
    {
        return L"";
    }

    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.data(), len);
    return w;
}

void STProgrammer::MoveRow(std::vector<CmdRow>& v, int from, int to) 
{
    if (from < 0 || to < 0 || from >= (int)v.size() || to >= (int)v.size() || from == to)
    {
        return;
    }

    auto row = v[from];
    v.erase(v.begin() + from);
    v.insert(v.begin() + to, row);
}




