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
protected:
    void LogFileOpen();
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();



    void AllConnect();
    void AllDisConnect();
    void ComportReset();
    void ButtonRelease();
private:
  
    ThreadPool

    bool LogFileSet = true; //로그파일 이름설정할것
    bool CreateBool = true;



    std::vector<std::shared_ptr<PortBox>> ObjectBox;


    std::vector<serial::PortInfo> PortInfo;
    std::vector<std::string> PortName;


    std::ofstream logFile;
    std::string LogFileName; //로그파일로 남길 이름
};