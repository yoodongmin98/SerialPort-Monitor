#include "EspUploader.h"
#include <filesystem>
#include <iostream>
#include "imgui.h"
#include <array>
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
        std::array<char, 128> Erasebuffer;
        std::string systemcmd = "esptool --port " + _PortNum + " erase_flash";
        FILE* pipes = _popen(systemcmd.c_str(), "r");
        while (fgets(Erasebuffer.data(), Erasebuffer.size(), pipes) != nullptr)
        {
            TriggerEvent(Erasebuffer.data());
        }
        std::filesystem::path exePath = std::filesystem::current_path();
        for (size_t i = 0; i < MemoryAddress.size(); ++i)
        {
            std::array<char, 128> buffer;
            std::filesystem::path filePath = exePath / _FileName[i];
            std::string writeCommand = "esptool --chip esp32 --port " + _PortNum + " --baud 921600 write_flash " + MemoryAddress[i] + " " + filePath.string();
            FILE* pipe = _popen(writeCommand.c_str(), "r");
           
            while (true)
            {
                //이거 flash예외처리
                std::string Strings = buffer.data();
             
                if (fgets(buffer.data(), buffer.size(), pipe) == nullptr)
                {
                    TriggerEvent("Flash Failed");
                    MyGUI_Interface::GUI->SetUIAble();
                    return;
                }
                /////////////////////////////////////////////////해야함
                TriggerEvent(Strings);
                if (Strings.find("Hash of data") != std::string::npos && i == 3)
                {
                    TriggerEvent("Flash Complete");
                    MyGUI_Interface::GUI->SetUIAble();
                    return;
                }
            }
        }
    }
}