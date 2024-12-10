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
        
	const ImVec2 GetcellSize()
	{
        return ImVec2{ cellSizeX ,cellSizeY };
	}

    const int& GetASCIIButton()
    {
        return ASCII_Button;
    }
    const int& GetMaxPortCount()
    {
        return MaxPortCount;
    }
protected:
    void PortBoxCreate();
    void DrawLine();


    void AllConnectBox(ImGuiIO& _io);
    void AllConnect();
    void AllDisConnect();
    void ComportReset();
    void ComPortDataSetting();
    void CLIBox();
    void RadarTypeBox();
    void ASCIILineMode();
    void HEXLineMode();
    void Frame_FPSBox(ImGuiIO& _io);

    void BoxInstance();

    void LogBox();
    void LogManagementBox();
    void LogClear();
    void LogFileCreateSelect();
    void DataSetting();
    void LineModeReset(int _PortCount, float _SizeX, float _SizeY);



    void ButtonRelease();
private:
    bool CreateBool = true;
    bool scrollToBottom = false;
    
    int ASCII_Button = 0;
    int HEX_Button = -1;
    int MaxPortCount = 36;
    int BaudRate = 921600; //Defalut
    float cellSizeX = 1500.0f; // X 크기
    float cellSizeY = 780.0f; // Y 크기


    std::vector<std::string> logs;

    std::string Name = "PortBox";
    std::string target = "USB";

    std::vector<std::shared_ptr<PortBox>> ObjectBox;


    std::vector<serial::PortInfo> PortInfo;
    std::vector<std::string> PortName;
};
