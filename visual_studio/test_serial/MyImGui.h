#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <string>
#include <serial/serial.h>
#include <fstream>
#include <thread>

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ThreadPool;
class PortBox;
class MyImGui
{
public:

    static MyImGui* MyImGuis;
    MyImGui();
    ~MyImGui();
    void Instance();
    const std::vector<std::string> GetPortName()
    {
        return PortName;
    }
    void LogFlash(std::string _PortName, std::string _Content);

    std::shared_ptr<ThreadPool> GetThreadPool()
    {
        return ThreadPools;
    }
    void AddLogBoxString(const std::string message) 
    {
        logs.emplace_back(message);
        scrollToBottom = true; // 스크롤을 맨 아래로 내리기 위한 플래그
    }
protected:
    void LogFileOpen();
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

    void PortBoxCreate();
    void DrawLine();
    void RadarTypeBox();
    void AllConnectBox();
    void Frame_FPSBox(ImGuiIO& _io);
    void LogBox();
    void CLIBox();
    void AllConnect();
    void AllDisConnect();
    void ComportReset();
    void LogClear();
    void ButtonRelease();
private:
    std::shared_ptr<ThreadPool> ThreadPools;
    std::vector<std::string> logs;

    bool scrollToBottom = false;
    bool LogFileSet = true; //로그파일 이름설정할것
    bool CreateBool = true;

    std::string Name = "PortBox";
    std::string target = "USB";

    std::vector<std::shared_ptr<PortBox>> ObjectBox;


    std::vector<serial::PortInfo> PortInfo;
    std::vector<std::string> PortName;


    std::ofstream logFile;
    std::string LogFileName; //로그파일로 남길 이름
};