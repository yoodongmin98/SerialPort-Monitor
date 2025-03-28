#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#include <vector>

#include <serial/serial.h>



// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ThreadPool;
class MyGUI_Interface;
class MyImGui
{
public:

    static MyImGui* MyImGuis;
    MyImGui();
    ~MyImGui();
    void Instance();
   

    std::shared_ptr<ThreadPool> GetThreadPool()
    {
        return ThreadPools;
    }
    const int GetWindowSize_X()
    {
        return width;
    }
    const int GetWindowSize_Y()
    {
        return height;
    }
    HWND& GetWindowHandle()
    {
        return hwnd;
    }
    RECT& GetRECT()
    {
        return rect;
    }
protected:
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

    void RenderLoop(ImGuiIO& io);
private:
    HWND hwnd;
    RECT rect;

    int width = 0;
    int height = 0;

    std::shared_ptr<MyGUI_Interface> MyGUI_Interfaces = nullptr;
    std::shared_ptr<ThreadPool> ThreadPools;
};