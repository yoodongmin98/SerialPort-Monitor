#pragma comment(lib, "comdlg32.lib")
#include "DataFile.h"
#include "imgui.h"
#include "MyGUI_Interface.h"
#include "MyImGui.h"
#include "PortBox.h"
#include "ThreadPool.h"
#include "MyTime.h"

#include <array>
#include <conio.h>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <ranges>
#include "Windows.h"
#include "CLI_Window.h"





MyGUI_Interface* MyGUI_Interface::GUI = nullptr;


MyGUI_Interface::MyGUI_Interface()
{
	GUI = this;
	FlashFileName.resize(4);
	CLI_Export_Window = std::make_shared<CLI_Window>();
}

MyGUI_Interface::~MyGUI_Interface()
{
	if (logFile.is_open())
	{
		logFile.close();
	}
}


void MyGUI_Interface::Instance(ImGuiIO& _io)
{
	if (DebugMode)
	{
		if (!ModeChange)
		{
			ShowWindow(GetConsoleWindow(), SW_NORMAL);
			ModeChange = true;
		}
	}
	else
	{
		if (ModeChange)
		{
			ShowWindow(GetConsoleWindow(), SW_HIDE);
			ModeChange = false;
		}

	}

	WinSizeX = MyImGui::MyImGuis->GetWindowSize_X();
	AutoKeySetting(_io);
    PortBoxCreate();
    DrawLine();
	AllConnectBox(_io);
	if (UIVisible)
	{
		LogBox();
		LogManagementBox();
	}
	BoxInstance();
}

void MyGUI_Interface::AutoKeySetting(ImGuiIO& _io)
{
	if (UIdisabled)
		return;
	//ImGuiKey_LeftAlt
	if (ImGui::IsKeyPressed(ImGuiKey_F) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		if (ViewBool)
		{
			FlashBool = true;
			ViewBool = false;
			ExportBool = false;
		}
		else if (FlashBool)
		{
			ViewBool = true;
			FlashBool = false;
			ExportBool = false;
		}
		else if (ExportBool)
		{
			ExportBool = false;
			FlashBool = true;
			ViewBool = false;
		}

		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
			obj->RawMonitorClear();
		}
		ScreenRelease();
	}
	if (ImGui::IsKeyPressed(ImGuiKey_R) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
			obj->RawMonitorClear();
		}
		ScreenRelease();
	}
	if (ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		if (!ViewBool)
			return;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			if (!obj->IsStringNull())
				obj->Connect();
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_D) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		if (!ViewBool)
			return;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_E) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		if (ViewBool || PortName.empty())
			return;
		UIdisabled = true;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->StartESPFlash(FlashFileName);
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_X) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		if (!ViewBool)
			return;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->InputCLI();
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_F12) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		DebugMode = !DebugMode;
	}
}

int MyGUI_Interface::extract_port_number(const std::string& s)
{
	size_t i = 0;
	while (i < s.size() && !std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
	if (i == s.size()) return INT_MAX; // 숫자 없음 -> 뒤로
	int n = 0;
	while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
		n = n * 10 + (s[i] - '0');
		++i;
	}
	return n;
}

void MyGUI_Interface::PortBoxCreate()
{
	PortInfo = serial::list_ports();
	if (CreateBool)
	{
		PortName.clear();
		int Xpos = ZERO, Ypos = ZERO, Count = ZERO;
		std::vector<const char*> BluetoothPort;
		std::vector<std::string> USBSerialCount;
		std::vector<const char*> ETCCount;

		for (serial::PortInfo& V : PortInfo)
		{
			if (V.description.find("Bluetooth") != std::string::npos)
				BluetoothPort.push_back(V.description.c_str());
			else if (V.description.find(target) != std::string::npos)
				USBSerialCount.push_back(V.port);
			else
				ETCCount.push_back(V.description.c_str());
		}
		if(MaxPortCount == 1 && USBSerialCount.size())
			PortName.push_back(USBSerialCount[USBinfo]);

		for (auto i = 0; i < PortInfo.size(); ++i)
		{
			if (PortName.size() >= MaxPortCount)
				break;
			if (PortInfo[i].description.find(target) != std::string::npos && PortInfo[i].description.find(exceptiontarget)==std::string::npos)
				PortName.push_back(PortInfo[i].port.c_str());
		
		}

		//Port Sorting
		std::ranges::sort(PortName, {},
			[this](const std::string& s) { return extract_port_number(s); });

		for (auto i = 0; i < PortName.size(); ++i)
		{
			std::string SetName = Name + std::to_string(i);
			if (Count == LineSwapSize)
			{
				Xpos = ZERO; Ypos += cellSizeY; Count = ZERO;
			}
			Count++;
			ObjectBox.push_back(make_shared<PortBox>(Xpos, Ypos, SetName));
			Xpos += cellSizeX;
		}
		CreateBool = false;
	}
}


void MyGUI_Interface::DrawLine()
{
	ImVec2 topLeft = XYZERO;
	ImVec2 bottomRight;
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
	{
		bottomRight = ImVec2(topLeft.x + MyImGui::MyImGuis->GetWindowSize_X(), topLeft.y + MyImGui::MyImGuis->GetWindowSize_Y());
	}
	else
	{
		bottomRight = ImVec2(topLeft.x + WinSizeX, topLeft.y + WinSizeY);
	}


	ImU32 color = PORTVIEWBACKGROUND;
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	// 수직선
	for (float x = topLeft.x; x <= bottomRight.x; x += cellSizeX)
		drawList->AddLine(ImVec2(x, topLeft.y), ImVec2(x, bottomRight.y), color);

	// 수평선
	for (float y = topLeft.y; y <= bottomRight.y; y += cellSizeY)
		drawList->AddLine(ImVec2(topLeft.x, y), ImVec2(bottomRight.x, y), color);
}


void MyGUI_Interface::AllConnectBox(ImGuiIO& _io)
{
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
		ImGui::SetNextWindowPos(ImVec2(MyImGui::MyImGuis->GetWindowSize_X()*0.8, ZERO), ImGuiCond_Always);
	else if (Window_Button == 0)
		ImGui::SetNextWindowPos(ImVec2(MINI_PORTVIEWSIZE_X, ZERO), ImGuiCond_Always);
	else
		ImGui::SetNextWindowPos(ImVec2(NORMAL_PORTVIEWSIZE_X, ZERO), ImGuiCond_Always);

	ImGui::Begin("Setting Box", nullptr, ImGuiWindowFlags_NoCollapse);
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
	{
		ImGui::SetWindowSize(ImVec2(MyImGui::MyImGuis->GetWindowSize_X()*0.2, MyImGui::MyImGuis->GetWindowSize_Y()));
	}
	else if (Window_Button == 0)
		ImGui::SetWindowSize(ImVec2(WinSizeX - MINI_PORTVIEWSIZE_X, MINI_PORTVIEWSIZE_Y));
	else if(Window_Button == 1)
		ImGui::SetWindowSize(ImVec2(WinSizeX - NORMAL_PORTVIEWSIZE_X, MINI_PORTVIEWSIZE_Y));
	else if(Window_Button == 2)
		ImGui::SetWindowSize(ImVec2(WinSizeX - NORMAL_PORTVIEWSIZE_X, NORMAL_PORTVIEWSIZE_Y));



	
	ImGui::BeginDisabled(UIdisabled);

	ImGui::BeginDisabled(IsWindowZoom);
	WindowMode();
	ImGui::EndDisabled();
	if(Window_Button == 0 || Window_Button == 1)
		LogBoxOnOff();
	ComPortDataSetting();
	SelectMode();
	if (ViewBool)
	{
		AllConnect();
		AllDisConnect();
		ComportReset();
		CLIBox();
		RadarTypeBox();
		ASCIILineMode();
		HEXLineMode();
	}
	else if(FlashBool)
	{
		FlashBox();
		ComportReset();
	}
	else if (ExportBool)
	{
		ExportCLIMode();
	}
	
	ImGui::EndDisabled();
	Frame_FPSBox(_io);

	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
	{
		if (!SmallZoomDrawLine && FrameCounter)
		{
			UIVisible = true;
			IsWindowZoom = true;
			cellSizeX = (MyImGui::MyImGuis->GetWindowSize_X() * 0.8) / LineSwapSize;
			cellSizeY = (MyImGui::MyImGuis->GetWindowSize_Y() * 0.8) / (MaxPortCount / LineSwapSize);

			for (std::shared_ptr<PortBox> obj : ObjectBox)
			{
				obj->DisConnect();
				obj->RawMonitorClear();
			}
			ScreenRelease();
			SmallZoomDrawLine = true;
		}
		FrameCounter = true;
	}
	else
	{
		IsWindowZoom = false;
		if (SmallZoomDrawLine)
		{
			FrameCounter = false;
			if(Window_Button!=2)
				UIVisible = false;
			CreateBool = true;
			WindowSizeSet();
			WindowDrawLineSet();
			SmallZoomDrawLine = false;
		}
	}
	ImGui::End();
}


void MyGUI_Interface::WindowMode()
{
	ImGui::SeparatorText("WindowMode");
	if (ImGui::RadioButton("Window1(1500 x 820)", &Window_Button, 0))
	{
		WinSizeX = WINDOWSIZE_SMALL_X; WinSizeY = WINDOWSIZE_SMALL_Y;
		UIVisible = false;
		WindowSizeSet();
		WindowDrawLineSet();
	}
	if (ImGui::RadioButton("Window2(1800 x 820)", &Window_Button, 1))
	{
		WinSizeX = WINDOWSIZE_NORMAL_X; WinSizeY = WINDOWSIZE_SMALL_Y;
		UIVisible = false;
		WindowSizeSet();
		WindowDrawLineSet();
	}
	if (ImGui::RadioButton("Window3(1800 x 1050) (LogBox On)", &Window_Button, 2))
	{
		WinSizeX = WINDOWSIZE_NORMAL_X; WinSizeY = WINDOWSIZE_NORMAL_Y;
		UIVisible = true;
		WindowSizeSet();
		WindowDrawLineSet();
	}
}


void MyGUI_Interface::WindowDrawLineSet()
{
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
		cellSizeX = MyImGui::MyImGuis->GetWindowSize_X() * 0.8 / LineSwapSize;
	else if (WinSizeX > WINDOW_CHECK_SIZE)
		cellSizeX = NORMAL_PORTVIEWSIZE_X / LineSwapSize;
	else
		cellSizeX = MINI_PORTVIEWSIZE_X / LineSwapSize;

	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
		cellSizeY = MyImGui::MyImGuis->GetWindowSize_Y() * 0.8 / (MaxPortCount / LineSwapSize);
	else
		cellSizeY = MINI_PORTVIEWSIZE_Y / (MaxPortCount / LineSwapSize);
	for (std::shared_ptr<PortBox> obj : ObjectBox)
	{
		obj->DisConnect();
	}
	ScreenRelease();
}


void MyGUI_Interface::WindowSizeSet()
{
	GetWindowRect(MyImGui::MyImGuis->GetWindowHandle(), &MyImGui::MyImGuis->GetRECT());

	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
	{
		WinSizeX = MyImGui::MyImGuis->GetWindowSize_X(); WinSizeY = MyImGui::MyImGuis->GetWindowSize_Y();
	}
	else
	{
		if (Window_Button == 0)
		{
			WinSizeX = WINDOWSIZE_SMALL_X; WinSizeY = WINDOWSIZE_SMALL_Y;
		}
		if (Window_Button == 1)
		{
			WinSizeX = WINDOWSIZE_NORMAL_X; WinSizeY = WINDOWSIZE_SMALL_Y;
		}
		if (Window_Button == 2)
		{
			WinSizeX = WINDOWSIZE_NORMAL_X; WinSizeY = WINDOWSIZE_NORMAL_Y;
		}
	}
	

	SetWindowPos(MyImGui::MyImGuis->GetWindowHandle(), nullptr,
		MyImGui::MyImGuis->GetRECT().left, MyImGui::MyImGuis->GetRECT().top,
		WinSizeX, WinSizeY, SWP_NOZORDER | SWP_NOACTIVATE);
}

void MyGUI_Interface::LogBoxOnOff()
{
	ImGui::SeparatorText("LogBox");
	ImGui::Checkbox("LogBox ON/OFF", &UIVisible);
}


void MyGUI_Interface::ComPortDataSetting()
{
	ImGui::SeparatorText("ComPortSetting");
	ImGui::Combo("BaudRate", &DataSettingBaudrate, BaudrateArray, IM_ARRAYSIZE(BaudrateArray));
	ImGui::Combo("Databit", &DataSettingDatabit, DatabitArray, IM_ARRAYSIZE(DatabitArray));
	ImGui::Combo("Stopbit", &DataSettingStopbit, StopbitArray, IM_ARRAYSIZE(StopbitArray));
	ImGui::Combo("Parity", &DataSettingParity, ParityArray, IM_ARRAYSIZE(ParityArray));
}


bool MyGUI_Interface::SelectMode()
{
	ImGui::SeparatorText("Select Mode");
	if (ImGui::Checkbox("View Mode", &ViewBool))
	{
		ViewBool = true;
		FlashBool = false;
		ExportBool = false;
		SelectModes = true;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
		}
		ScreenRelease();
	}

	if (ImGui::Checkbox("Flash Mode", &FlashBool))
	{
		ViewBool = false;
		FlashBool = true;
		ExportBool = false;
		SelectModes = false;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
		}
		ScreenRelease();
	}

	if (ImGui::Checkbox("Export Mode", &ExportBool))
	{
		ViewBool = false;
		FlashBool = false;
		ExportBool = true;
		SelectModes = false;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
		}
		ScreenRelease();
	}

	return SelectModes;
}


void MyGUI_Interface::FlashBox()
{
	//Python이랑 esptool 설치여부 확인
	ImGui::SeparatorText("Flash Setting");

	if (PythonCheck)
	{
		commandOutput = executeCommand("py -c \"import sys; print(sys.executable)\"");
		PythonCheck = false;
	}
	if (commandOutput.empty())
	{
		ImGui::TextColored(REDCOLOR, "Python is not installed.");
		ImGui::TextColored(REDCOLOR, "Please install Python first.");
	}
	else
	{
		SystemPathSetting();

		if (EspCheck)
		{
			EsptoolCommand = executeCommand("esptool version");
			EspCheck = false;
		}
		if (EsptoolCommand.empty())
		{
			ImGui::TextColored(REDCOLOR, "esptool is not installed");
			ImGui::TextColored(REDCOLOR, "Please run as administrator.");
			if (ImGui::Button("Install esptool", ButtonSize))
			{
				std::system("pip install esptool==4.8.1");
				EspCheck = true;
			}
		}
		else
			FlashSettings();
	}
}


void MyGUI_Interface::FlashSettings()
{
	ImGui::PushItemWidth(150);
	ImGui::InputText("0x1000", (char*)ExtractFileName(FlashFileName[0]).c_str(), ExtractFileName(FlashFileName[0]).size() + 1, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	if (ImGui::Button("File1"))
	{
		FlashFileName[0] = OpenFileDialog();
	}
	ImGui::InputText("0x8000", (char*)ExtractFileName(FlashFileName[1]).c_str(), ExtractFileName(FlashFileName[1]).size() + 1, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	if (ImGui::Button("File2"))
	{
		FlashFileName[1] = OpenFileDialog();
	}
	ImGui::InputText("0xe000", (char*)ExtractFileName(FlashFileName[2]).c_str(), ExtractFileName(FlashFileName[2]).size() + 1, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	if (ImGui::Button("File3"))
	{
		FlashFileName[2] = OpenFileDialog();
	}
	ImGui::InputText("0x10000", (char*)ExtractFileName(FlashFileName[3]).c_str(), ExtractFileName(FlashFileName[3]).size() + 1, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	if (ImGui::Button("File4"))
	{
		FlashFileName[3] = OpenFileDialog();
	}
	ImGui::PopItemWidth();



	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
	if (ImGui::Button("Erase And Flash", ButtonSize) && !PortName.empty())
	{
		UIdisabled = true;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->StartESPFlash(FlashFileName);
		}
	}
	ImGui::PopStyleColor(1);
}

std::string MyGUI_Interface::ExtractFileName(std::string _FileName)
{
	size_t pos = _FileName.find_last_of("\\");
	std::string filenames = (pos == std::string::npos) ? _FileName : _FileName.substr(pos + 1);
	return filenames;
}


std::string MyGUI_Interface::executeCommand(std::string command) 
{
	std::array<char, 128> buffer;
	std::string result;

	FILE* pipe = _popen(command.c_str(), "r");
	if (!pipe) 
	{
		std::cerr << "Error: Unable to open pipe for command: " << command << std::endl;
		return result;
	}

	while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) 
	{
		result += buffer.data();
	}

	_pclose(pipe);

	return result;
}

void MyGUI_Interface::RadarTypeBox()
{
	ImGui::SeparatorText("Radar Type");
	std::vector<const char*> BluetoothPort;
	std::vector<const char*> USBSerialCount;
	std::vector<const char*> ETCCount;
	for (serial::PortInfo& V : PortInfo)
	{
		if (V.description.find("Bluetooth") != std::string::npos)
			BluetoothPort.push_back(V.description.c_str());
		else if (V.description.find(target) != std::string::npos)
			USBSerialCount.push_back(V.description.c_str());
		else
			ETCCount.push_back(V.description.c_str());
	}
	ImGui::Text("Exception PortName");
	ImGui::InputText(" ", exceptiontarget, IM_ARRAYSIZE(exceptiontarget));
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Port detected %d", PortInfo.size());
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "USBSerial detected : %d", USBSerialCount.size());
	ImGui::Combo("USB(Serial)", &USBinfo, USBSerialCount.data(), USBSerialCount.size());
	ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.9f, 1.0f), "Blutooth detected : %d", BluetoothPort.size());
	ImGui::Combo("BlueTooth", &Bluetoothinfo, BluetoothPort.data(), BluetoothPort.size());
	ImGui::TextColored(ImVec4(0.7f,0.7f, 0.95f, 1.0f), "ETC Port detected : %d", ETCCount.size());
	ImGui::Combo("ETC", &ETCinfo, ETCCount.data(), ETCCount.size());
}


void MyGUI_Interface::LogManagementBox()
{
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
		ImGui::SetNextWindowPos(ImVec2(MyImGui::MyImGuis->GetWindowSize_X() * 0.55f, MyImGui::MyImGuis->GetWindowSize_Y()*0.8), ImGuiCond_Always);
	else if (WinSizeX > WINDOW_CHECK_SIZE)
		ImGui::SetNextWindowPos(ImVec2(NORMAL_PORTVIEWSIZE_X * 0.7f, MyImGui::MyImGuis->GetWindowSize_Y() - LogBoxYSize), ImGuiCond_Always);
	else
		ImGui::SetNextWindowPos(ImVec2(MINI_PORTVIEWSIZE_X * 0.7f, MyImGui::MyImGuis->GetWindowSize_Y() - LogBoxYSize), ImGuiCond_Always);
	ImGui::Begin("Manage Log", nullptr, ImGuiWindowFlags_NoCollapse);
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
		ImGui::SetWindowSize(ImVec2(MyImGui::MyImGuis->GetWindowSize_X() * 0.25f, MyImGui::MyImGuis->GetWindowSize_Y()*0.2));
	else if (WinSizeX > WINDOW_CHECK_SIZE)
		ImGui::SetWindowSize(ImVec2(NORMAL_PORTVIEWSIZE_X * 0.3f, LogBoxYSize));
	else
		ImGui::SetWindowSize(ImVec2(MINI_PORTVIEWSIZE_X * 0.3f, LogBoxYSize));
	LogClear();
	LogFileCreateSelect();
	DataSetting();
	ImGui::End();
}

void MyGUI_Interface::Frame_FPSBox(ImGuiIO& _io)
{
	ImGui::SeparatorText("Frame / FPS");
	ImGui::Text("Frame : %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
}


void MyGUI_Interface::LogBox()
{
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
		ImGui::SetNextWindowPos(ImVec2(0, MyImGui::MyImGuis->GetWindowSize_Y()*0.8), ImGuiCond_Always);
	else
		ImGui::SetNextWindowPos(ImVec2(0, MyImGui::MyImGuis->GetWindowSize_Y() - LogBoxYSize), ImGuiCond_Always);
	ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoCollapse);
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
		ImGui::SetWindowSize(ImVec2(ImVec2(MyImGui::MyImGuis->GetWindowSize_X() * 0.55f, MyImGui::MyImGuis->GetWindowSize_Y()*0.2)));
	else if (WinSizeX > WINDOW_CHECK_SIZE)
		ImGui::SetWindowSize(ImVec2(NORMAL_PORTVIEWSIZE_X * 0.7f, LogBoxYSize));
	else
		ImGui::SetWindowSize(ImVec2(MINI_PORTVIEWSIZE_X * 0.7f, LogBoxYSize));
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


void MyGUI_Interface::CLIBox()
{
	ImGui::SeparatorText("CLI (Write)");
	static char buffer[2000] = "";
	ImGui::Text("Input CLI");
	ImGui::InputTextMultiline("##output", buffer, sizeof(buffer), ImVec2(250, 300), ImGuiInputTextFlags_None | ImGuiInputTextFlags_EscapeClearsAll);
	CLI_Text = buffer;
	if (ImGui::Button("AllSend", ButtonSize))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->InputCLI();
		}
	}
	ImGui::SameLine();
	ImGui::Checkbox("Auto Send", &AutoCLI);
}

void MyGUI_Interface::ASCIILineMode()
{
	ImGui::SeparatorText("Data Port Mode(ASCII)");
	if(ImGui::RadioButton("ASCII 1(1port)", &ASCII_Button, 0))
	{
		LineSwapSize = Line_1;
		MaxPortCount = 1;
		ClickASCII = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("ASCII 2(6Port)", &ASCII_Button, 1))
	{
		LineSwapSize = Line_3;
		MaxPortCount = 6;
		ClickASCII = true;
	}
	if (ImGui::RadioButton("ASCII 3(12Port)", &ASCII_Button, 2))
	{
		LineSwapSize = Line_4;
		MaxPortCount = 12;
		ClickASCII = true;
	}
	ImGui::SameLine();
	if(ImGui::RadioButton("ASCII 4(30Port)", &ASCII_Button, 3))
	{
		LineSwapSize = Line_6;
		MaxPortCount = 30;
		ClickASCII = true;
	}
	if (ImGui::RadioButton("ASCII 5(42Port)", &ASCII_Button, 4))
	{
		LineSwapSize = Line_6;
		MaxPortCount = 42;
		ClickASCII = true;
	}

	if (ClickASCII)
	{
		HEX_Button = -1;
		WindowDrawLineSet();
		ClickASCII = false;
	}
}



void MyGUI_Interface::HEXLineMode()
{
	ImGui::SeparatorText("Data Port Mode(HEX)");
	if (ImGui::RadioButton("HEX 1(1port)", &HEX_Button, 0))
	{
		LineSwapSize = Line_1;
		MaxPortCount = 1;
		ClickHEX = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("HEX 2(6Port)", &HEX_Button, 1))
	{
		LineSwapSize = Line_3;
		MaxPortCount = 6;
		ClickHEX = true;
	}
	if (ImGui::RadioButton("HEX 3(12Port)", &HEX_Button, 2))
	{
		LineSwapSize = Line_4;
		MaxPortCount = 12;
		ClickHEX = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("HEX 4(30Port)", &HEX_Button, 3))
	{
		LineSwapSize = Line_6;
		MaxPortCount = 30;
		ClickHEX = true;
	}
	if (ImGui::RadioButton("HEX 5(42Port)", &HEX_Button, 4))
	{
		LineSwapSize = Line_6;
		MaxPortCount = 42;
		ClickHEX = true;
	}

	if (ClickHEX)
	{
		ASCII_Button = -1;
		WindowDrawLineSet();
		ClickHEX = false;
	}
}



void MyGUI_Interface::BoxInstance()
{
	for (auto i = 0; i < PortName.size(); ++i)
		ObjectBox[i]->Instance(PortName[i]);
}



void MyGUI_Interface::AllConnect()
{
	ImGui::SeparatorText("Connect");
	if (ImGui::Button("All Connect", ButtonSize))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			if (!obj->IsStringNull())
				obj->Connect();
		}
	}
}

void MyGUI_Interface::AllDisConnect()
{
	if (ImGui::Button("All DisConnect", ButtonSize))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
		}
	}
}

void MyGUI_Interface::ComportReset()
{
	if (ImGui::Button("ComPort Reset", ButtonSize))
	{
 		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
			obj->RawMonitorClear();
		}
		ScreenRelease();
	}
}

void MyGUI_Interface::LogClear()
{
	if (ImGui::Button("Log Clear"))
	{
		logs.clear();
	}
}

void MyGUI_Interface::LogFileCreateSelect()
{
	ImGui::BeginDisabled(PathDisabled);
	ImGui::SeparatorText("Log File Record");
	ImGui::Checkbox("Log Box Record", &LogBoxs);
	
	if (LogBoxs)
	{
		if (LogDatabool)
		{
			if (!logFile.is_open() && !LogPATH.empty())
			{
				size_t pos = LogPATH.find_last_of("\\");
				LogPATH = LogPATH.substr(0, pos + 1);
				logFile.open(LogPATH + "Log.txt", std::ios::app);
			}
			LogDatabool = false;
		}
	}
	else
	{
		if (!LogDatabool)
		{
			if (logFile.is_open())
			{
				logFile.close();
			}
			LogDatabool = true;
		}
	
	}
	ImGui::SameLine();
	if (ImGui::Button(" ..."))
	{
		LogPATH = SaveFileDialog();
	}
	TextPATH(LogPATH);


	ImGui::Checkbox("PortBox RawData Record", &PortRawData);
	
	if (PortRawData)
	{
		if (PortRawDatabool)
		{
			for (std::shared_ptr<PortBox> obj : ObjectBox)
				obj->SetCOMLogFile(PATH);
			PortRawDatabool = false;
		}
	}
	else
	{
		if (!PortRawDatabool)
		{
			for (std::shared_ptr<PortBox> obj : ObjectBox)
				obj->SetCOMLofFileClose();
			PortRawDatabool = true;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("... "))
	{
		PATH = SaveFileDialog();
	}
	TextPATH(PATH , false);

	ImGui::EndDisabled();
}


void MyGUI_Interface::TextPATH(std::string& _PATH , const bool _IsLog)
{
	std::string CopyPATH;
	if (!_PATH.empty())
	{
		size_t pos = _PATH.find_last_of("\\");
		if(_IsLog)
			CopyPATH = _PATH.substr(0, pos + 1) + "Log.txt";
		else
			CopyPATH = _PATH.substr(0, pos + 1) + " (COM NUMBER) ";
	}
	ImGui::Text("PATH : %s", CopyPATH.c_str());
}


void MyGUI_Interface::DataSetting()
{
	ImGui::SeparatorText("Data Setting");
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.8f, 1.0f));           
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.3f, 1.0f, 1.0f));    
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.1f, 0.1f, 0.6f, 1.0f));     
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));     
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::SliderInt("Undetected time", &Sliderint, 3, 30);
	ImGui::PopStyleColor(5);
	ImGui::InputText("Boot Detection Char", BootDetection, IM_ARRAYSIZE(BootDetection));
}

void MyGUI_Interface::ScreenRelease()
{
	ObjectBox.clear();
	PortName.clear();
	CreateBool = true;
	EspCheck = true;
	LogBoxs = false;
	PortRawData = false;
}


void MyGUI_Interface::SystemPathSetting()
{
	if (SystemPath)
	{
		size_t pos = commandOutput.find("\\python.exe");
		if (pos != std::string::npos)
			commandOutput = commandOutput.substr(0, pos);
		SetEnvironmentVariable("PYTHON_HOME", commandOutput.c_str());
		commandOutput += "\\Scripts";
		SetEnvironmentVariable("PYTHON_Script", commandOutput.c_str());
		executeCommand("pip install --upgrade pip");
		SystemPath = false;
	}
}

std::string MyGUI_Interface::SaveFileDialog()
{
	OPENFILENAME ofn;
	char szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = "txt";

	if (GetSaveFileName(&ofn))
		std::wcout << "파일 저장 경로: " << szFile << std::endl;
	else
		std::wcout << "파일 저장 취소됨" << std::endl;

	std::string SZFILE(szFile);

	return SZFILE;
}

std::string MyGUI_Interface::OpenFileDialog()
{
	char fileName[MAX_PATH] = { 0 };
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = "Firmware Files\0*.bin;*.elf;*.hex\0All Files\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		return std::string(fileName);
	}
	return "";
}


void MyGUI_Interface::LogFlash(std::string _PortName, std::string _Content)
{
	logFile << "\r [" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "]" << _PortName + _Content << std::flush;
}

void MyGUI_Interface::ExportCLIMode()
{	
	float CLIWinSizeX = 0.0f;
	float CLIWinSizeY = 0.0f;
	if (IsZoomed(MyImGui::MyImGuis->GetWindowHandle()))
	{
		CLIWinSizeX = MyImGui::MyImGuis->GetWindowSize_X() * 0.8f;
		CLIWinSizeY = MyImGui::MyImGuis->GetWindowSize_Y() * 0.8f;
	}
	else if (Window_Button == 0)
	{
		CLIWinSizeX = MyImGui::MyImGuis->GetWindowSize_X() * 0.8095f;
		CLIWinSizeY = MyImGui::MyImGuis->GetWindowSize_Y();
	}	
	else if (Window_Button == 1)
	{
		CLIWinSizeX = MyImGui::MyImGuis->GetWindowSize_X() * 0.8415f;
		CLIWinSizeY = MyImGui::MyImGuis->GetWindowSize_Y();
	}
	else if (Window_Button == 2)
	{
		CLIWinSizeX = MyImGui::MyImGuis->GetWindowSize_X() * 0.8415f;
		CLIWinSizeY = MyImGui::MyImGuis->GetWindowSize_Y() * 0.775f;
	}
	
	CLI_Export_Window->Instance(CLIWinSizeX, CLIWinSizeY);
}