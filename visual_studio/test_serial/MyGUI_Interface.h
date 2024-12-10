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
    int Sliderint = 5;
    int DataSettingBaudrate = 14; //Default
    float cellSizeX = 1500.0f; // X ũ��
    float cellSizeY = 780.0f; // Y ũ��


    const char* BaudrateArray[17] = { "110","300","600","1200","2400","4800","9600","14400","19200","38400","57600",
        "115200","230400","460800","921600","1000000","1843200" };
    std::vector<std::string> logs;

    std::string Name = "PortBox";
    std::string target = "USB";

    std::vector<std::shared_ptr<PortBox>> ObjectBox;


    std::vector<serial::PortInfo> PortInfo;
    std::vector<std::string> PortName;
};
