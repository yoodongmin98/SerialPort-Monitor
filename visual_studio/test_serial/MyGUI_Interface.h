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
        scrollToBottom = true; // ��ũ���� �� �Ʒ��� ������ ���� �÷���
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
    int GetDataSettingDatabit()
    {
        return std::stoi(DatabitArray[DataSettingDatabit]);
    }
    int GetDataSettingStopbit()
    {
        return std::stoi(StopbitArray[DataSettingStopbit]);
    }
    //�и�Ƽ�� ��� ����

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
    void WindowDrawLineSet();
    void WindowSizeSet();
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

    bool LogBoxs = false;
    bool PortRawData = false;

    
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

    int AllBoxXSize = 294;
    float cellSizeX = 1200.0f; // X ũ��
    float cellSizeY = 780.0f; // Y ũ��

    int WinSizeX = 1500; //Window Xũ��
    int WinSizeY = 820; //Window Yũ��

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
