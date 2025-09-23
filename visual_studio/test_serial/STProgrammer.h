#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>


class CmdRow
{
public:
    bool use = true;
    std::string label;
    std::string args;
};

class STProgrammer : public std::enable_shared_from_this<STProgrammer>
{
public:
    STProgrammer();
    ~STProgrammer();

    STProgrammer(const STProgrammer&) = delete;
    STProgrammer(STProgrammer&&) = delete;
    STProgrammer& operator=(const STProgrammer&) = delete;
    STProgrammer& operator=(STProgrammer&&) = delete;

    void Instance(const float _X, const float _Y);
protected:
    void CreateButtonSetBox();
    void CreateRawTextBox();
    void Send(std::wstring _CMD);
    void LogScrollCheck();
    std::wstring Quote(const std::wstring& s);
    std::wstring FindCubeProgCLI();


    //비동기 연결확인
    void AutoRunBox();
    void StartPolling(int interval_ms /*=1000*/, bool rerun_on_reconnect /*=true*/);
    void StopPolling();
    bool CheckConnected();
    void RunSequenceAsync();
    bool RunSequence();
    void MoveRow(std::vector<CmdRow>& v, int from, int to);
    std::wstring ToW(const std::string& s);
private:
    bool stopOnError = true;
    bool scrollToBottom = false;
    bool autoRun = false;
    std::atomic<bool> polling_{ false };     // 폴링 루프 on/off
    std::atomic<bool> running_{ false };     // RunSequence 실행 중일 때
    std::atomic<bool> armed_{ true };        // 연결되면 실행해도 되는 상태

    int  selected = -1;
    int poll_interval_ms_ = 1000;

    std::wstring PATH;
    std::wstring args = L"";

    std::mutex log_mtx;
    std::thread poll_thread_;
    inline static thread_local char argBuf[512];

    //Debug창에 띄울 Log들
    std::vector<std::string> DebugLog;

    //STProgrammerCLI에 넣어줄 명령어 Table
    std::vector<CmdRow> rows = 
    {
      {true,  "Change Option Byte(AA)", "-c port=SWD -ob RDP=0xAA"},
      {true, "Erase All",               "-c port=SWD -e all"},
      {true, "Write Firmware",          "-c port=SWD -d MDR.bin 0x08000000"},
      {true, "Change Option Byte(BB)",  "-c port=SWD -ob RDP=0xBB"},
    };

    //STM의 경로를 탐색할 C드라이브 List
    std::vector<std::wstring> candidates =
    {
        LR"(C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files\STMicroelectronics\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files (x86)\STMicroelectronics\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\ST\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
    };
};
