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

protected:
    std::wstring Quote(const std::wstring& s);
    std::wstring FindCubeProgCLI();
private:
    
    
    std::vector<std::wstring> candidates =
    {
        LR"(C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files\STMicroelectronics\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\Program Files (x86)\STMicroelectronics\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
        LR"(C:\ST\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe)",
    };
};
