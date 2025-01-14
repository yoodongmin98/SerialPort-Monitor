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
    const int GetDataSettingBaudrate()
    {
        return std::stoi(BaudrateArray[DataSettingBaudrate]);
    }
    const int GetDataSettingDatabit()
    {
        return std::stoi(DatabitArray[DataSettingDatabit]);
    }
    const int GetDataSettingStopbit()
    {
        return std::stoi(StopbitArray[DataSettingStopbit]);
    }
    //패리티는 잠깐 보류

    char* GetBootDetectionCharacter()
    {
        return BootDetection;
    }
    
    const bool GetViewBool()
    {
        return ViewBool;
    }

    std::string executeCommand(std::string command);

    const void SetUIAble()
    {
        UICount++;
        if (UICount == PortName.size())
        {
            UIdisabled = false;
            UICount = 0;
        }
    }
protected:
    void PortBoxCreate();
    void DrawLine();


    void AllConnectBox(ImGuiIO& _io);
    bool SelectMode();

    void FlashBox();
    void SystemPathSetting();
    void FlashSettings();
    char Text1[50] = "bootloader.bin";
    char Text2[50] = "partitions.bin";
    char Text3[50] = "boot_app0.bin";
    char Text4[50] = "firmware.bin";
 
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



    void ScreenRelease();
private:
    bool UIdisabled = false;
    int UICount = 0;

    bool CreateBool = false;
    bool scrollToBottom = false;
    
    bool UIVisible = false;

    bool LogBoxs = false;
    bool PortRawData = false;

    bool ClickASCII = false;
    bool ClickHEX = false;
    
    bool SelectModes = true;
    bool ViewBool = true;
    bool FlashBool = false;

    bool PythonCheck = true;
    bool UpgradePython = false;
    bool SystemPath = true;
    bool EspCheck = true;
    bool Installesptool = false;


    int USBinfo = 0;
    int Bluetoothinfo = 0;
    int ETCinfo = 0;
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

    int LogBoxYSize = 230;

    float cellSizeX = 1200.0f; // X 크기
    float cellSizeY = 780.0f; // Y 크기

    float WinSizeX = 1500.0f; //Window X크기
    float WinSizeY = 820.0f; //Window Y크기

    const char* BaudrateArray[17] = { "110","300","600","1200","2400","4800","9600","14400","19200","38400","57600",
        "115200","230400","460800","921600","1000000","1843200" };
    const char* DatabitArray[2] = { "7","8" };
    const char* StopbitArray[2] = { "1","2" };
    const char* ParityArray[5] = { "NONE","Odd","Even","Mark","Space"};
    std::vector<std::string> logs;

    char BootDetection[128] = "START";
    char exceptiontarget[128] = "Enhanced COM";
    std::string Name = "PortBox";
    std::string target = "USB";
    std::string commandOutput;
    std::string EsptoolCommand;

    std::vector<std::shared_ptr<PortBox>> ObjectBox;


    std::vector<serial::PortInfo> PortInfo;
    std::vector<std::string> PortName;


    std::vector<std::string> FlashFileName;
};
