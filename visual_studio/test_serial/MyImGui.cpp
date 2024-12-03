#include "MyImGui.h"
#include <iostream>
#include "PortBox.h"
#include <string>
#include "MyTime.h"
#include "ThreadPool.h"
#include <functional>


#define MaxPortCount 30
#define ThreadCount 3


MyImGui* MyImGui::MyImGuis = nullptr;

MyImGui::MyImGui()
	: ThreadPools(std::make_shared<ThreadPool>(ThreadCount))
{
	MyImGuis = this;
}
MyImGui::~MyImGui()
{
	if (logFile.is_open())
	{
		logFile.close();
	}
}


void MyImGui::Instance()
{
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"NHN Test Program", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
	}

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

	// Main loop
	bool done = false;
	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		g_SwapChainOccluded = false;

		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////


		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (LogFileSet)
		{
			LogFileName = "Log.txt";
			LogFileOpen();
			LogFileSet = false;
		}

		PortBoxCreate();
		DrawLine();
		AllConnectBox();
		Frame_FPSBox(io);
		RadarTypeBox();
		LogBox();
		EtcBox();
		for (auto i = 0; i < PortName.size(); ++i)
			ObjectBox[i]->Instance(PortName[i]);

	
		

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		HRESULT hr = g_pSwapChain->Present(1, 0);
		g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}


bool MyImGui::CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}


void MyImGui::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam);
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


void MyImGui::CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}


void MyImGui::CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}


void MyImGui::LogFileOpen()
{
	//로그파일 생성
	logFile.open(LogFileName, std::ios::app);
}

void MyImGui::LogFlash(std::string _PortName , std::string _Content)
{
	logFile << "\r [" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "]" << _PortName + _Content << std::flush;
}


void MyImGui::PortBoxCreate()
{
	if (CreateBool)
	{
		int Xpos = 0, Ypos = 0, Count = 0;
		PortInfo = serial::list_ports();
		for (auto i = 0; i < PortInfo.size(); ++i)
		{
			if (PortInfo[i].description.find(target) != std::string::npos)
				PortName.push_back(PortInfo[i].port.c_str());
		}
		for (auto i = 0; i < MaxPortCount; ++i)
		{
			std::string SetName = Name + std::to_string(i);
			if (Count == 6)
			{
				Xpos = 0; Ypos += 100; Count = 0;
			}
			Count++;
			ObjectBox.push_back(make_shared<PortBox>(Xpos, Ypos, SetName));
			Xpos += 180;
		}
		CreateBool = false;
	}
}

void MyImGui::DrawLine()
{
	ImVec2 topLeft = { 0.0f,0.0f };
	ImVec2 bottomRight = ImVec2(topLeft.x + 1080, topLeft.y + 500);
	float cellSizeX = 180.0f; // X 크기
	float cellSizeY = 100.0f; // Y 크기
	ImU32 color = IM_COL32(205, 205, 205, 128); 
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	// 수직선
	for (float x = topLeft.x; x <= bottomRight.x; x += cellSizeX) 
		drawList->AddLine(ImVec2(x, topLeft.y), ImVec2(x, bottomRight.y), color);

	// 수평선
	for (float y = topLeft.y; y <= bottomRight.y; y += cellSizeY) 
		drawList->AddLine(ImVec2(topLeft.x, y), ImVec2(bottomRight.x, y), color);
	
}


void MyImGui::RadarTypeBox()
{
	ImGui::SetNextWindowPos(ImVec2(1080, 300), ImGuiCond_Always);
	ImGui::Begin("Radar Type", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(200, 200));

	std::vector<serial::PortInfo> Infos = serial::list_ports();
	unsigned int BluetoothCount = 0;
	unsigned int USBSerialCount = 0;
	unsigned int ETCCount = 0;
	for (serial::PortInfo& V : Infos)
	{
		if (V.description.find("Bluetooth") != std::string::npos)
			BluetoothCount++;
		else if (V.description.find(target) != std::string::npos)
			USBSerialCount++;
		else
			ETCCount++;
	}
	ImGui::Text("Port detected : %d", Infos.size());
	ImGui::Text("Blutooth detected : %d", BluetoothCount);
	ImGui::Text("USBSerial detected : %d", USBSerialCount);
	ImGui::Text("ETC Port detected : %d", ETCCount);
	ImGui::End();
}

void MyImGui::AllConnectBox()
{
	ImGui::SetNextWindowPos(ImVec2(1080, 0), ImGuiCond_Always);
	ImGui::Begin("All Check", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(200, 150));
	AllConnect();
	AllDisConnect();
	ComportReset();
	if (ImGui::Button("Log Clear"))
	{
		logs.clear();
	}
	ImGui::End();
}


void MyImGui::Frame_FPSBox(ImGuiIO& _io)
{
	ImGui::SetNextWindowPos(ImVec2(1080, 150), ImGuiCond_Always);
	ImGui::StyleColorsClassic();

	ImGui::Begin("Frame / FPS", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(200, 150));
	ImGui::Text("Frame : %.3f ms/frame", 1000.0f / _io.Framerate);
	ImGui::Text("FPS : %.1f", _io.Framerate);
	ImGui::End();
}


void MyImGui::LogBox()
{
	ImGui::SetNextWindowPos(ImVec2(0, 500), ImGuiCond_Always);
	ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(1080, 260));
	ImGui::BeginChild("Console", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
	
	
	for (const auto& log : logs) 
	{
		ImGui::Text("%s", log.c_str());
	}
	if (scrollToBottom) 
	{
		ImGui::SetScrollHereY(1.0f);
		scrollToBottom = false;
	}

	ImGui::EndChild();
	ImGui::End();
}


void MyImGui::EtcBox()
{
	ImGui::SetNextWindowPos(ImVec2(1080, 500), ImGuiCond_Always);
	ImGui::Begin("Etc", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(200, 260));
	ImGui::End();
}



void MyImGui::AllConnect()
{
	if (ImGui::Button("All Connect"))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			if (!obj->IsStringNull())
				obj->Connect();
		}
	}
}
void MyImGui::AllDisConnect()
{
	if (ImGui::Button("All DisConnect"))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
		}
	}
}
void MyImGui::ComportReset()
{
	if (ImGui::Button("ComPort Reset"))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
		}
		ButtonRelease();
	}
}
void MyImGui::ButtonRelease()
{
	ObjectBox.clear();
	PortName.clear();
	CreateBool = true;
}
