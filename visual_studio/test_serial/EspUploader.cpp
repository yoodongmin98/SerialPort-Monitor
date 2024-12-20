#include "EspUploader.h"
#include <filesystem>
#include <iostream>
#include "imgui.h"





EspUploader::EspUploader()
{

}

EspUploader::~EspUploader()
{

}




void EspUploader::Instance(std::string& _PortNum, std::vector<std::string>& _FileName)
{
    FlashLog.clear();
    
    std::string systemcmd = "esptool --port " + _PortNum + " erase_flash";
    if (std::system(systemcmd.c_str()) == 0)
    {
        FlashLog.push_back(_PortNum + " 의 Erase가 완료되었습니다.");
        scrollToBottom = true;
    }
    else
    {
        FlashLog.push_back(_PortNum + " 의 Erase에 실패했습니다");
        scrollToBottom = true;
        return;
    }

    std::filesystem::path exePath = std::filesystem::current_path();

    for (auto i = 0; i < MemoryAddress.size(); ++i)
    {
        std::string exePaths = exePath.string();
        std::string command = "esptool --chip esp32 --port "+ _PortNum +" --baud 921600 write_flash " + MemoryAddress[i] + " " + exePaths.c_str() + "\\" + _FileName[i];

        int result = std::system(command.c_str());
        if (result == 0)
        {
            std::cout << "Flash성공!" << std::endl;
        }
        else
        {
            std::cout << "Flash실패!" << std::endl;
        }
    }



}