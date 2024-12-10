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
protected:
    void PortBoxCreate();
    void DrawLine();
    void RadarTypeBox();
    void AllConnectBox(ImGuiIO& _io);
    void BoxInstance();

    void Frame_FPSBox(ImGuiIO& _io);
    void LogBox();
    void LogManagementBox();
    void CLIBox();
    void ASCIILineMode();
    void HEXLineMode();
    void LineModeReset(int _PortCount, float _SizeX, float _SizeY);

    void AllConnect();
    void AllDisConnect();
    void ComportReset();
    void LogClear();
    void ButtonRelease();
private:
    bool CreateBool = true;
    bool scrollToBottom = false;
    
    int ASCII_Button = 0;
    int HEX_Button = -1;
    int MaxPortCount = 36;
    float cellSizeX = 1500.0f; // X ũ��
    float cellSizeY = 780.0f; // Y ũ��


    std::vector<std::string> logs;

    std::string Name = "PortBox";
    std::string target = "USB";

    std::vector<std::shared_ptr<PortBox>> ObjectBox;


    std::vector<serial::PortInfo> PortInfo;
    std::vector<std::string> PortName;
};
