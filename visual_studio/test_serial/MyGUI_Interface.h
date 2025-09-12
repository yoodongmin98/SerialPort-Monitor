#pragma once
#include "imgui.h"

#include <Windows.h>
#include <string>
#include <vector>
#include <serial/serial.h>
#include <fstream>
#include <mutex>
#include "DataFile.h"


enum class WinMode : uint8_t
{
    _1500x820,
    _1800x820,
    _1800x1050
};
enum class UIMode : uint8_t
{
    View = 0,
    Flash_ESP = 1,
    Flash_STM = 2,
    Export = 3,
};

class LineModePreset 
{
public:
    const char* label;
    int lineSwap;
    int ports; 
};

class MyWinSize
{
public:
    float x, y;
    bool Visible;
};


class STProgrammer;
class CLI_Window;
class PortBox;
class MyGUI_Interface
{
public:
    static MyGUI_Interface* GUI;
    MyGUI_Interface();
    ~MyGUI_Interface();

    const std::vector<std::string> GetPortName()
    {
        return PortName;
    }

    const std::vector<const char*> GetUSBPortInfo()
    {
        return USBSerialCount;
    }

    const void AddLogBoxString(const std::string& message)
    {
        std::lock_guard<std::mutex> lock(logmutex); // deadlock test
        logs.emplace_back(message);
        scrollToBottom = true; // 스크롤을 맨 아래로 내리기 위한 플래그
    }

    const ImVec2 GetcellSize()
    {
        return ImVec2{ cellSizeX, cellSizeY };
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

    char* GetBootDetectionCharacter()
    {
        return BootDetection;
    }

    const bool GetViewBool()
    {
        return ViewBool;
    }

    std::string& GetCLIText()
    {
        return CLI_Text;
    }

    const void SetUIAble()
    {
        UICount++;
        if (UICount == PortName.size())
        {
            UIdisabled = false;
            UICount = 0;
        }
    }

    void SetUIDisable()
    {
        UIdisabled = false;
    }

    const bool GetAutoCLI()
    {
        return AutoCLI;
    }

    void PlusPathCount()
    {
        PathCount++;
        if (PathCount != 0)
        {
            PathDisabled = true;
        }
    }

    void MinusPathCount()
    {
        PathCount--;
        if (PathCount == 0)
        {
            PathDisabled = false;
        }
    }


    void Instance(ImGuiIO& _io);
    std::string SaveFileDialog(std::string _format);
    std::string OpenFileDialog();
    void LogFlash(std::string _PortName, std::string _Content);

protected:
    void DebugModeSet();
    void AutoKeySetting(ImGuiIO& _io);
    void ScreenRelease();
    void PortBoxCreate();
    int extract_port_number(const std::string& s);
    void DrawLine();
    void AllConnectBox(ImGuiIO& _io);
    void WindowModes();
    void WindowSizeSet();
    void WindowDrawLineSet();
    void LogBoxOnOff();
    void ComPortDataSetting();
    UIMode SelectMode();
    void   OnModeChanged(UIMode m);
    void AllConnect();
    void AllDisConnect();
    void ComportReset();
    void CLIBox();
    void RadarTypeBox();
    void ASCIILineMode();
    void HEXLineMode();
    void FlashBox_ESP();
    void SystemPathSetting();
    std::string executeCommand(std::string command);
    void FlashSettings();
    std::string ExtractFileName(std::string _FileName);
    void FlashBox_STM();
    void ExportCLIMode();
    std::pair<float, float> GetDisplaySize();
    void LogBox();
    void LogManagementBox();
    void LogClear();
    void LogFileCreateSelect();
    void TextPATH(std::string& _PATH, const bool _IsLog = true);
    void DataSetting();
    void AutoKeyHelpText();
    void Frame_FPSBox(ImGuiIO& _io);
    void WindowZoomCheck();
    void BoxInstance();


    bool   IsViewMode()   const { return mode_ == UIMode::View; }
    bool   IsFlashESPMode()  const { return mode_ == UIMode::Flash_ESP; }
    bool   IsExportSTMMode() const { return mode_ == UIMode::Flash_STM; }
    bool   IsExportMode() const { return mode_ == UIMode::Export; }
private:
    UIMode mode_ = UIMode::View;

    const float kPosX[3] = { MINI_PORTVIEWSIZE_X,  NORMAL_PORTVIEWSIZE_X,  NORMAL_PORTVIEWSIZE_X };
    const float kSizeY[3] = { MINI_PORTVIEWSIZE_Y,  MINI_PORTVIEWSIZE_Y,    NORMAL_PORTVIEWSIZE_Y };
    const MyWinSize WindowMode[3] = 
    {
        { WINDOWSIZE_SMALL_X,  WINDOWSIZE_SMALL_Y,  false },
        { WINDOWSIZE_NORMAL_X, WINDOWSIZE_SMALL_Y,  false },
        { WINDOWSIZE_NORMAL_X, WINDOWSIZE_NORMAL_Y, true  }
    };

    std::vector<LineModePreset> ASCII_Mode = 
    {
        {"ASCII 1(1port)",  Line_1, 1},
        {"ASCII 2(6Port)",  Line_3, 6},
        {"ASCII 3(12Port)", Line_4, 12},
        {"ASCII 4(30Port)", Line_6, 30},
        {"ASCII 5(42Port)", Line_6, 42},
    };
    std::vector<LineModePreset> HEX_Mode = 
    {
        {"HEX 1(1port)",  Line_1, 1},
        {"HEX 2(6Port)",  Line_3, 6},
        {"HEX 3(12Port)", Line_4, 12},
        {"HEX 4(30Port)", Line_6, 30},
        {"HEX 5(42Port)", Line_6, 42},
    };

    bool UIdisabled = false;
    bool IsWindowZoom = false;
    bool SmallZoomDrawLine = false;
    bool PathDisabled = false;
    bool FrameCounter = false;
    bool DebugMode = false;
    bool ModeChange = false;
    bool CreateBool = false;
    bool scrollToBottom = false;
    bool UIVisible = false;
    bool LogBoxs = false;
    bool PortRawData = false;
    bool LogDatabool = false;
    bool PortRawDatabool = false;
    bool ClickASCII = false;
    bool ClickHEX = false;
    bool SelectModes = true;
    bool ViewBool = true;
    bool FlashBool = false;
    bool ExportBool = false;
    bool PythonCheck = true;
    bool UpgradePython = false;
    bool SystemPath = true;
    bool EspCheck = true;
    bool Installesptool = false;
    bool AutoCLI = false;
    bool zoomed = false;

    int PathCount = 0;
    int UICount = 0;
    int USBinfo = 0;
    int Bluetoothinfo = 0;
    int ETCinfo = 0;
    int Window_Button = 0;
    int ASCII_Button = 0;
    int HEX_Button = -1;
    int MaxPortCount = 1;
    int Sliderint = 5;
    int LineSwapSize = 1;
    int DataSettingBaudrate = 14;
    int DataSettingDatabit = 1;
    int DataSettingStopbit = 0;
    int DataSettingParity = 0;

    float LogBoxYSize = 230.0f;
    float cellSizeX = 1200.0f;
    float cellSizeY = 780.0f;
    float WinSizeX = 1500.0f;
    float WinSizeY = 820.0f;

    const char* BaudrateArray[18] = 
    {
        "110","300","600","1200","2400","4800","9600","14400","19200","38400","57600",
        "115200","230400","460800","921600","1000000","1250000","1843200"
    };
    const char* DatabitArray[2] = { "7","8" };
    const char* StopbitArray[2] = { "1","2" };
    const char* ParityArray[5] = { "NONE","Odd","Even","Mark","Space" };
    const char* FAddress[4] = { "0x1000", "0x8000", "0xe000", "0x10000" };

    std::vector<const char*> BluetoothPort;
    std::vector<const char*> USBSerialCount;
    std::vector<const char*> ETCCount;

    std::vector<std::string> logs;
    std::vector<std::string> PortName;
    std::vector<std::string> FlashFileName;

    std::vector<serial::PortInfo> PortInfo;

    std::vector<std::shared_ptr<PortBox>> ObjectBox;

    std::mutex logmutex;

    char BootDetection[128] = "START";
    char exceptiontarget[128] = "Enhanced COM";

    std::string PATH = "";
    std::string LogPATH = "";
    std::string Name = "PortBox";
    std::string target = "USB";
    std::string commandOutput;
    std::string EsptoolCommand;
    std::string CLI_Text;
    std::string LogFileName;

    std::ofstream logFile;

    HWND hwnds;

    std::shared_ptr<CLI_Window> CLI_Export_Window;
    std::shared_ptr<STProgrammer> STProgrammers;
};
