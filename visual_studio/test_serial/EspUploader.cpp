#include "EspUploader.h"
#include <filesystem>
#include <iostream>
#include "imgui.h"

#include "MyGUI_Interface.h"



EspUploader::EspUploader()
{

}

EspUploader::~EspUploader()
{

}


void EspUploader::Instance(std::string& _PortNum, std::vector<std::string>& _FileName)
{ 
    {
        std::unique_lock<std::mutex>(EspMutex);
        std::string systemcmd = "esptool --port " + _PortNum + " erase_flash";
        if (std::system(systemcmd.c_str()) == 0)
        {
            TriggerEvent("Erase of " + _PortNum + " is complete");
            scrollToBottom = true;
        }
        else
        {
            TriggerEvent("Erase of " + _PortNum + " failed ");
            scrollToBottom = true;
            return;
        }


        std::filesystem::path exePath = std::filesystem::current_path();
        for (size_t i = 0; i < MemoryAddress.size(); ++i)
        {
            std::filesystem::path filePath = exePath / _FileName[i];
            std::string writeCommand = "esptool --chip esp32 --port " + _PortNum + " --baud 921600 write_flash " + MemoryAddress[i] + " " + filePath.string();
            TriggerEvent(writeCommand);
            int result = std::system(writeCommand.c_str());
            if (result == 0)
            {
                TriggerEvent("Flash Complete!" + filePath.string());
            }
            else
            {
                TriggerEvent("Flash error!");
            }
        }
    }
}