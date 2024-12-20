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


void EspUploader::PrintLog()
{
   
    for (const auto& log : FlashLog)
    {
        ImGui::Text("%s", log.c_str());
    }
    if (scrollToBottom)
    {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }
}


void EspUploader::Instance(std::string& _PortNum, std::vector<std::string>& _FileName)
{
    FlashLog.clear();
    
    std::string systemcmd = "esptool --port " + _PortNum + " erase_flash";
    if (std::system(systemcmd.c_str()) == 0)
    {
        FlashLog.push_back(_PortNum + " �� Erase�� �Ϸ�Ǿ����ϴ�.");
        scrollToBottom = true;
    }
    else
    {
        FlashLog.push_back(_PortNum + " �� Erase�� �����߽��ϴ�");
        scrollToBottom = true;
        return;
    }

    std::filesystem::path exePath = std::filesystem::current_path();
    std::string writeCommand = "esptool --chip esp32 --port " + _PortNum + " --baud 921600 --after no_reset write_flash";
    for (size_t i = 0; i < MemoryAddress.size(); ++i)
    {
        std::filesystem::path filePath = exePath / _FileName[i];
        writeCommand += " " + MemoryAddress[i] + " " + filePath.string();
    }

    for (auto i = 0; i < MemoryAddress.size(); ++i)
    {
       

        int result = std::system(writeCommand.c_str());
        if (result == 0)
        {
            FlashLog.push_back("Flash����!");
        }
        else
        {
            FlashLog.push_back("Flash����!");
        }
    }

    std::string resetCommand = "esptool --port " + _PortNum + " run";
    int result = std::system(resetCommand.c_str());
    if (result == 0)
    {
        FlashLog.push_back("reset����!");
    }
    else
    {
        FlashLog.push_back("reset����!");
    }


}