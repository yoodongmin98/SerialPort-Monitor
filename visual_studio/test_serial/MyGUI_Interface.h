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
    const int GetSliderInt()
    {
        return Sliderint;
    }
    int GetDataSettingBaudrate()
    {
        return std::stoi(BaudrateArray[DataSettingBaudrate]);
    }
    char* GetBootDetectionCharacter()
    {
        return BootDetection;
    }
protected:
    void PortBoxCreate();
    void DrawLine();

    void VisibleUI(ImGuiIO& _io);

    void AllConnectBox(ImGuiIO& _io);
    void WindowMode();
    void LogBoxOnOff();
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
    bool CreateBool = false;
    bool scrollToBottom = false;
    
    bool UIVisible = false;
    
    int Window_Button = 0;
    int ASCII_Button = 0;
    int HEX_Button = -1;
    int MaxPortCount = 1;
    int Sliderint = 5;

    int LineSwapSize = 1;

    int DataSettingBaudrate = 14; //Default
    int DataSettingDatabit = 1; //Default
    int DataSettingStopbit = 0; //Default
    int DataSettingParity = 0; //Default

    int AllBoxXSize = 284;
    float cellSizeX = 1200.0f; // X 크기
    float cellSizeY = 780.0f; // Y 크기

    float WinSizeX = 0.0f;
    float WinSizeY = 0.0f;

    const char* BaudrateArray[17] = { "110","300","600","1200","2400","4800","9600","14400","19200","38400","57600",
        "115200","230400","460800","921600","1000000","1843200" };
    const char* DatabitArray[2] = { "7","8" };
    const char* StopbitArray[2] = { "1","2" };
    const char* ParityArray[5] = { "NONE","Odd","Even","Mark","Space"};
    std::vector<std::string> logs;

    char BootDetection[128] = "START";
    std::string Name = "PortBox";
    std::string target = "USB";

    std::vector<std::shared_ptr<PortBox>> ObjectBox;


    std::vector<serial::PortInfo> PortInfo;
    std::vector<std::string> PortName;
};
