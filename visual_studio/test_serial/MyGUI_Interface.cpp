#pragma comment(lib, "comdlg32.lib")
#include "DataFile.h"
#include "imgui.h"
#include "MyGUI_Interface.h"
#include "MyImGui.h"
#include "PortBox.h"
#include "ThreadPool.h"
#include "MyTime.h"

#include <string>
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
	//static pointer instance (singletone 과 유사한 디자인패턴)
	GUI = this;
	//Esp Uploader에 담길 Filename 4개(Bootloader , partition , boot_app , firmware)
	FlashFileName.resize(4);
	//CLI Mode Instance
	CLI_Export_Window = std::make_shared<CLI_Window>();
}

MyGUI_Interface::~MyGUI_Interface()
{
	//logfile 관리 ( 전체 PortBox에 대한 로그파일)
	if (logFile.is_open())
	{
		logFile.close();
	}
}


//Loop Instance
void MyGUI_Interface::Instance(ImGuiIO& _io)
{
	//상단바를 제외한 윈도우의 X Y size를 받아와서 사용
	WinSizeX = MyImGui::MyImGuis->GetWindowSize_X();
	WinSizeY = MyImGui::MyImGuis->GetWindowSize_Y();
	hwnds = MyImGui::MyImGuis->GetWindowHandle();
	zoomed = IsZoomed(hwnds);

	//디버그모드 관리
	DebugModeSet();

	//단축키 관리
	AutoKeySetting(_io);

	//CLI exportMode가 아닐때 PortBox생성 Setting ( 생성 x )
	if(!ExportBool)
		PortBoxCreate();

	//Window GridLine생성 ( ASCII , HEX 포트 개수에 따른 라인 그리기 )
    DrawLine();

	//연결 및 설정을 관리하는 버튼들
	AllConnectBox(_io);

	//WindowMode에따라 LogBox ON/OFF여부
	if (UIVisible)
	{
		//로그박스와 로그 관리박스 생성
		LogBox();
		LogManagementBox();
	}

	//PortBoxCreate에서 생성된 Container정보를 바탕으로 PortBox생성
	BoxInstance();
}


//DebugMode(Console)을 띄우는 키를 설정합니다. (밑 AutoKeySetting참조 Ctrl + F12)
void MyGUI_Interface::DebugModeSet()
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
}

//단축키 세팅
void MyGUI_Interface::AutoKeySetting(ImGuiIO& _io)
{
	if (UIdisabled)
		return;
	//ImGuiKey_LeftAlt
	if (ImGui::IsKeyPressed(ImGuiKey_F) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		//여기 나중에 Mode많아지면 Container에 담고 iterator로 돌리는게 나아보임
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

	//단축키 많아지면 function pointer로 바인딩해서 넘겨주는함수 만들어도 괜찮을듯
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

//렌더링 화면 Release
void MyGUI_Interface::ScreenRelease()
{
	ObjectBox.clear();
	PortName.clear();
	CreateBool = true;
	EspCheck = true;
	LogBoxs = false;
	PortRawData = false;
}


/////////////////////////////////////PortBox생성
void MyGUI_Interface::PortBoxCreate()
{
	//시스템 Port list가져옴
	PortInfo = serial::list_ports();
	if (CreateBool)
	{
		PortName.clear();
		//윈도우에 그려진 라인에따라 위치를 잡아줄 변수들(PortBox 생성자 Initializer에 들어감)
		int Xpos = ZERO, Ypos = ZERO, Count = ZERO;
		std::vector<const char*> BluetoothPort;
		std::vector<std::string> USBSerialCount;
		std::vector<const char*> ETCCount;

		//포트 분류(Blutooth , USB , 기타)
		for (serial::PortInfo& V : PortInfo)
		{
			if (V.description.find("Bluetooth") != std::string::npos)
				BluetoothPort.push_back(V.description.c_str());
			else if (V.description.find(target) != std::string::npos)
				USBSerialCount.push_back(V.port);
			else
				ETCCount.push_back(V.description.c_str());
		}

		//모드에서 설정된 최대 포트개수가 1개고 USB size가 1개이상 인식되어있을 때
		if(MaxPortCount == 1 && USBSerialCount.size())
			PortName.push_back(USBSerialCount[USBinfo]);

		//최대 포트개수까지 list를 돌면서 container에 ComName만 분류
		for (auto i = 0; i < PortInfo.size(); ++i)
		{
			if (PortName.size() >= MaxPortCount)
				break;
			//포트 이름에서 target(USB)문자가 있고 , exceptiontarget(default : Enhanced) 가 없을 때 push 
			if (PortInfo[i].description.find(target) != std::string::npos && PortInfo[i].description.find(exceptiontarget)==std::string::npos)
				PortName.push_back(PortInfo[i].port.c_str());
		
		}

		//Port Sorting
		std::ranges::sort(PortName, {},
			[this](const std::string& s) { return extract_port_number(s); });

		//윈도우에 그려지는 Line Size에따라 Pos를 받고 PortBox 생성자 initialize
		for (auto i = 0; i < PortName.size(); ++i)
		{
			std::string SetName = Name + std::to_string(i);
			if (Count == LineSwapSize)
			{
				Xpos = ZERO; Ypos += cellSizeY; Count = ZERO;
			}
			Count++;
			//생성 및 Initializer 인자 전달
			ObjectBox.push_back(make_shared<PortBox>(Xpos, Ypos, SetName));
			Xpos += cellSizeX;
		}
		CreateBool = false;
	}
}

////포트 번호 추출함수
int MyGUI_Interface::extract_port_number(const std::string& s)
{
	size_t i = 0;
	while (i < s.size() && !std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
	if (i == s.size()) return INT_MAX; // 숫자 없음 -> 뒤로
	int n = 0;
	while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
	{
		n = n * 10 + (s[i] - '0');
		++i;
	}
	return n;
}

////////Window Portbox자리에 그려지는 Line생성(Grid)
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
	//WindowMode에 따른 창 크기 Setting
	if (zoomed) 
	{
		ImGui::SetNextWindowPos(ImVec2(WinSizeX * 0.8f, 0.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(WinSizeX * 0.2f, WinSizeY), ImGuiCond_Always);
	}
	else 
	{
		ImGui::SetNextWindowPos(ImVec2(kPosX[Window_Button], 0.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(WinSizeX - kPosX[Window_Button], kSizeY[Window_Button]), ImGuiCond_Always);
	}

	ImGui::Begin("Setting Box", nullptr, ImGuiWindowFlags_NoCollapse);


	ImGui::BeginDisabled(UIdisabled);

	////Zoom되어있을때(윈도우 최대창일때)는 Mode UI선택 불가////
	ImGui::BeginDisabled(IsWindowZoom);
	WindowModes();
	ImGui::EndDisabled();
	////////////////////////////////////////////////////////

	if(Window_Button == 0 || Window_Button == 1)
		LogBoxOnOff();

	ComPortDataSetting();

	switch (SelectMode()) 
	{
	case UIMode::View:
		AllConnect();
		AllDisConnect();
		ComportReset();
		CLIBox();
		RadarTypeBox();
		ASCIILineMode();
		HEXLineMode();
		break;

	case UIMode::Flash:
		FlashBox();
		ComportReset();
		break;

	case UIMode::Export:
		ExportCLIMode();
		break;
	}
	AutoKeyHelpText();
	
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

//프로그램 윈도우 창 Mode Setting
void MyGUI_Interface::WindowModes()
{
	const int prev = Window_Button;

	ImGui::SeparatorText("WindowMode");
	ImGui::RadioButton("Window1(1500 x 820)", &Window_Button, 0);
	ImGui::RadioButton("Window2(1800 x 820)", &Window_Button, 1);
	ImGui::RadioButton("Window3(1800 x 1050) (LogBox On)", &Window_Button, 2);

	if (Window_Button != prev) 
	{
		WinSizeX = WindowMode[Window_Button].x;
		WinSizeY = WindowMode[Window_Button].y;
		UIVisible = WindowMode[Window_Button].Visible;

		WindowSizeSet();
		WindowDrawLineSet();
	}
}

/////Mode에 따라 WindowSize Setting
void MyGUI_Interface::WindowSizeSet()
{
	GetWindowRect(hwnds, &MyImGui::MyImGuis->GetRECT());

	if (!zoomed)
	{
		WinSizeX = WindowMode[Window_Button].x;
		WinSizeY = WindowMode[Window_Button].y;
	}

	SetWindowPos(MyImGui::MyImGuis->GetWindowHandle(), nullptr,
		MyImGui::MyImGuis->GetRECT().left, MyImGui::MyImGuis->GetRECT().top,
		WinSizeX, WinSizeY, SWP_NOZORDER | SWP_NOACTIVATE);
}

/////Mode에 따라 창에 그려질 LineSize Setting
void MyGUI_Interface::WindowDrawLineSet()
{
	// X: 확대면 WinSizeX * 0.8, 아니면 프리셋 / 열 수
	cellSizeX = (
		zoomed
		? WinSizeX * 0.8f
		: (WinSizeX > WINDOW_CHECK_SIZE ? NORMAL_PORTVIEWSIZE_X : MINI_PORTVIEWSIZE_X))
		/ std::max(1, LineSwapSize);

	// Y: 확대면 WinSizeY, 아니면 프리셋 / 행 수(0 나눗셈 방지)
	const int cols = std::max(1, LineSwapSize);
	const int rows = std::max(1, MaxPortCount / cols);
	cellSizeY = (zoomed ? WinSizeY : MINI_PORTVIEWSIZE_Y) / rows;

	for (auto& obj : ObjectBox)
	{
		obj->DisConnect();
	}
	ScreenRelease();
}

/////LogBox on/off
void MyGUI_Interface::LogBoxOnOff()
{
	ImGui::SeparatorText("LogBox");
	ImGui::Checkbox("LogBox ON/OFF", &UIVisible);
}

//연결 정보 데이터 setting (Baudrate  , Databit , Stopbit , Parity)
void MyGUI_Interface::ComPortDataSetting()
{
	ImGui::SeparatorText("ComPortSetting");
	ImGui::Combo("BaudRate", &DataSettingBaudrate, BaudrateArray, IM_ARRAYSIZE(BaudrateArray));
	ImGui::Combo("Databit", &DataSettingDatabit, DatabitArray, IM_ARRAYSIZE(DatabitArray));
	ImGui::Combo("Stopbit", &DataSettingStopbit, StopbitArray, IM_ARRAYSIZE(StopbitArray));
	ImGui::Combo("Parity", &DataSettingParity, ParityArray, IM_ARRAYSIZE(ParityArray));
}


UIMode MyGUI_Interface::SelectMode()
{
	ImGui::SeparatorText("Select Mode");

	int m = static_cast<int>(mode_);
	const int prev = m;

	ImGui::RadioButton("View Mode", &m, static_cast<int>(UIMode::View));
	ImGui::RadioButton("Flash Mode", &m, static_cast<int>(UIMode::Flash));
	ImGui::RadioButton("Export Mode", &m, static_cast<int>(UIMode::Export));

	if (m != prev)
	{
		OnModeChanged(static_cast<UIMode>(m));
	}

	return mode_;
}


//변경된 모드 감지
void MyGUI_Interface::OnModeChanged(UIMode m)
{
	mode_ = m;
	for (auto& obj : ObjectBox)
	{
		obj->DisConnect();
	}
	ScreenRelease();
}

////PortBox AllConnect
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

////PortBox AllDisConnect
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

////PortBox AllReset
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

//////PortBox CLI InputBox
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

//// Port Type 분류표시
void MyGUI_Interface::RadarTypeBox()
{
	ImGui::SeparatorText("Radar Type");
	BluetoothPort.clear();
	USBSerialCount.clear();
	ETCCount.clear();
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
	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.95f, 1.0f), "ETC Port detected : %d", ETCCount.size());
	ImGui::Combo("ETC", &ETCinfo, ETCCount.data(), ETCCount.size());
}

//ASCII_Mode Setting
void MyGUI_Interface::ASCIILineMode()
{
	const int prev = ASCII_Button;
	ImGui::SeparatorText("Data Port Mode(ASCII)");

	for (int i = 0; i < ASCII_Mode.size(); ++i) 
	{
		if (i % 2 == 1) ImGui::SameLine();              
		if (ImGui::RadioButton(ASCII_Mode[i].label, &ASCII_Button, i)) 
		{
			LineSwapSize = ASCII_Mode[i].lineSwap;
			MaxPortCount = ASCII_Mode[i].ports;
		}
	}

	if (ASCII_Button != prev) 
	{
		HEX_Button = -1;
		WindowDrawLineSet();                            
	}
}


//HEX_Mode Setting
void MyGUI_Interface::HEXLineMode()
{
	const int prev = HEX_Button;
	ImGui::SeparatorText("Data Port Mode(HEX)");

	for (int i = 0; i < HEX_Mode.size(); ++i)
	{
		if (i % 2 == 1) ImGui::SameLine();
		if (ImGui::RadioButton(HEX_Mode[i].label, &HEX_Button, i))
		{
			LineSwapSize = HEX_Mode[i].lineSwap;
			MaxPortCount = HEX_Mode[i].ports;
		}
	}

	if (HEX_Button != prev)
	{
		ASCII_Button = -1;
		WindowDrawLineSet();
	}
}

//FlashMode Python설치여부 및 esptool설치여부 확인
void MyGUI_Interface::FlashBox()
{
	ImGui::SeparatorText("Flash Setting");

	//Python 체크
	if (PythonCheck) 
	{
		commandOutput = executeCommand("py -c \"import sys; print(sys.executable)\"");
		PythonCheck = false;
	}
	if (commandOutput.empty()) 
	{
		ImGui::TextColored(REDCOLOR, "Python is not installed.");
		ImGui::TextColored(REDCOLOR, "Please install Python first.");
		return;                             
	}

	//esptool 체크
	if (EspCheck) 
	{
		SystemPathSetting();
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
		return;
	}

	FlashSettings();
}

//python Script 환경변수 내부적으로 설정해주는 함수
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

//////popen으로 shell단에 명령어 전송해주는 함수
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


//EspUploader에 넘겨줄 파일의 주소 입력부(Bootloder 등등....)
void MyGUI_Interface::FlashSettings()
{
	if (ImGui::BeginTable("flash_files", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV))
	{
		for (int i = 0; i < 4; ++i)
		{
			ImGui::TableNextRow();

			// Address
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(FAddress[i]);

			// File name (read-only 표시)
			ImGui::TableSetColumnIndex(1);
			const std::string name = ExtractFileName(FlashFileName[i]);
			if (name.empty())
				ImGui::TextDisabled("<select file>");
			else
				ImGui::TextUnformatted(name.c_str());

			// Browse button
			ImGui::TableSetColumnIndex(2);
			std::string btn = std::string("File") + std::to_string(i + 1);
			if (ImGui::Button(btn.c_str())) 
			{
				FlashFileName[i] = OpenFileDialog();
			}
		}
		ImGui::EndTable();
	}

	// 실행 버튼
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
	if (ImGui::Button("Erase And Flash", ButtonSize) && !PortName.empty())
	{
		UIdisabled = true;
		for (auto& obj : ObjectBox)
		{
			obj->StartESPFlash(FlashFileName);
		}
	}
	ImGui::PopStyleColor(1);
}


//파일 이름 추출해주는 함수
std::string MyGUI_Interface::ExtractFileName(std::string _FileName)
{
	size_t pos = _FileName.find_last_of("\\");
	std::string filenames = (pos == std::string::npos) ? _FileName : _FileName.substr(pos + 1);
	return filenames;
}

//CLI Mode 창 instance 함수
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

//하단 로그박스 instance함수
void MyGUI_Interface::LogBox()
{
	float logY = zoomed ? (WinSizeY * 0.8f) : std::max(0.0f, WinSizeY - LogBoxYSize);
	ImGui::SetNextWindowPos(ImVec2(0.0f, logY), ImGuiCond_Always);

	float logW = zoomed ? (WinSizeX * 0.55f) : ((WinSizeX > WINDOW_CHECK_SIZE ? NORMAL_PORTVIEWSIZE_X : MINI_PORTVIEWSIZE_X) * 0.7f);
	float logH = zoomed ? (WinSizeY * 0.2f) : LogBoxYSize;
	ImGui::SetNextWindowSize(ImVec2(logW, logH), ImGuiCond_Always);

	ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoCollapse);

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

void MyGUI_Interface::LogManagementBox()
{
	if (zoomed) 
	{
		ImGui::SetNextWindowPos(ImVec2(WinSizeX * 0.55f, WinSizeY * 0.8f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(WinSizeX * 0.25f, WinSizeY * 0.2f), ImGuiCond_Always);
	}
	else 
	{
		const float baseX = (WinSizeX > WINDOW_CHECK_SIZE) ? NORMAL_PORTVIEWSIZE_X : MINI_PORTVIEWSIZE_X;
		const float posY = std::max(0.0f, WinSizeY - LogBoxYSize);
		ImGui::SetNextWindowPos(ImVec2(baseX * 0.7f, posY), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(baseX * 0.3f, LogBoxYSize), ImGuiCond_Always);
	}
	ImGui::Begin("Manage Log", nullptr, ImGuiWindowFlags_NoCollapse);

	LogClear();
	LogFileCreateSelect();
	DataSetting();

	ImGui::End();
}

//Log Clear Button
void MyGUI_Interface::LogClear()
{
	if (ImGui::Button("Log Clear"))
	{
		logs.clear();
	}
}

////////LogFile , LogBox(PortBox) Data Record 관련 함수
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
		LogPATH = SaveFileDialog(".txt");
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
		PATH = SaveFileDialog(".txt");
	}
	TextPATH(PATH, false);

	ImGui::EndDisabled();
}

//로그파일 경로 설정된 Text표시
void MyGUI_Interface::TextPATH(std::string& _PATH, const bool _IsLog)
{
	std::string CopyPATH;
	if (!_PATH.empty())
	{
		size_t pos = _PATH.find_last_of("\\");
		if (_IsLog)
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

////////단축키 설명 함수
void MyGUI_Interface::AutoKeyHelpText()
{
	ImGui::SeparatorText("Auto Key");
	ImGui::Text("Ctrl + F = ViewMode < - > FlashMode");

	ImGui::Text("(ViewMode)   Ctrl + R = ComportReset");
	ImGui::Text("(ViewMode)   Ctrl + C = AllConnect");
	ImGui::Text("(ViewMode)   Ctrl + D = AllDisConnect");
	ImGui::Text("(ViewMode)   Ctrl + X = SendCLI");
	ImGui::Text("(FlashMode)  Ctrl + E = EraseAndFlash");
	//ImGui::Text("(ExportMode) Ctrl + E = AllExport");
}

//Frame표시해주는 Text
void MyGUI_Interface::Frame_FPSBox(ImGuiIO& _io)
{
	ImGui::SeparatorText("Frame / FPS");
	ImGui::Text("Frame : %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
}


//PortBox 실행 (Instancing)
void MyGUI_Interface::BoxInstance()
{
	for (auto i = 0; i < PortName.size(); ++i)
		ObjectBox[i]->Instance(PortName[i]);
}






///////////////////////////////Helper Function///////////////////////////////

//파일저장위치 설정 함수
std::string MyGUI_Interface::SaveFileDialog(std::string _format)
{
	OPENFILENAME ofn;
	char szFile[260] = { 0 };

	std::string A;
	A = ("Files (*" + _format + ")\0 *" + _format + "\0All Files(*.*)\0 * .*\0").c_str();

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = A.c_str();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = _format.c_str();

	if (GetSaveFileName(&ofn))
		std::wcout << "파일 저장 경로: " << szFile << std::endl;
	else
		std::wcout << "파일 저장 취소됨" << std::endl;

	std::string SZFILE(szFile);

	return SZFILE;
}

//파일오픈위치 설정 함수
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


//로그파일에 남길 string입력 함수
void MyGUI_Interface::LogFlash(std::string _PortName, std::string _Content)
{
	logFile << "\r [" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "]" << _PortName + _Content << std::flush;
}