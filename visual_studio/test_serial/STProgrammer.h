#pragma once

#include <string>
#include <vector>

class STProgrammer
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
    void CreateRawTextBox();
    void CreateButtonSetBox();
    void Send(std::wstring _CMD);
    void LogScrollCheck();
    std::wstring Quote(const std::wstring& s);
    std::wstring FindCubeProgCLI();

private:
    bool scrollToBottom = false;
    //STM의 경로를 탐색할 C드라이브 List
    std::vector<std::wstring> candidates =
    {
        LR"(C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files\STMicroelectronics\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files (x86)\STMicroelectronics\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\ST\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
    };

    std::vector<std::string> DebugLog;
    std::wstring PATH;
    std::wstring args = L"";
 
};
