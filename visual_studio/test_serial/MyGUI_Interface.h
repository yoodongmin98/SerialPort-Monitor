#pragma once
#include "imgui.h"
#include <string>
#include <vector>
#include <serial/serial.h>



class PortBox;
class MyGUI_Interface
{
public:
    static MyGUI_Interface* GUI;
    MyGUI_Interface();
    ~MyGUI_Interface();


    void Instance(ImGuiIO& _io);
    const std::vector<std::string> GetPortName()
    {
        return PortName;
    }
    const void AddLogBoxString(const std::string& message)
    {
        logs.emplace_back(message);
        scrollToBottom = true; // 스크롤을 맨 아래로 내리기 위한 플래그
    }
protected:
    void PortBoxCreate();
    void DrawLine();
    void RadarTypeBox();
    void AllConnectBox(ImGuiIO& _io);

    void Frame_FPSBox(ImGuiIO& _io);
    void LogBox();
    void LogManagementBox();
    void CLIBox();


    void AllConnect();
    void AllDisConnect();
    void ComportReset();
    void LogClear();
    void ButtonRelease();
private:
    bool CreateBool = true;
    bool scrollToBottom = false;


    std::vector<std::string> logs;

    std::string Name = "PortBox";
    std::string target = "USB";

    std::vector<std::shared_ptr<PortBox>> ObjectBox;


    std::vector<serial::PortInfo> PortInfo;
    std::vector<std::string> PortName;
};
