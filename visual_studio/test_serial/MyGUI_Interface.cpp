#include "MyGUI_Interface.h"
#include "imgui.h"
#include "PortBox.h"
#include "MyImGui.h"
#include <functional>
#include <conio.h>


#define ButtonSize ImVec2{130,20}

MyGUI_Interface* MyGUI_Interface::GUI = nullptr;


MyGUI_Interface::MyGUI_Interface()
{
	GUI = this;
}

MyGUI_Interface::~MyGUI_Interface()
{

}


void MyGUI_Interface::Instance(ImGuiIO& _io)
{
	VisibleUI(_io);
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


void MyGUI_Interface::VisibleUI(ImGuiIO& _io)
{
	if(ImGui::IsKeyReleased(ImGuiKey_L))
	{
		UIVisible = !UIVisible;
	}
}

void MyGUI_Interface::PortBoxCreate()
{
	if (CreateBool)
	{
		int Xpos = 0, Ypos = 0, Count = 0;
		PortInfo = serial::list_ports();
		for (auto i = 0; i < PortInfo.size(); ++i)
		{
			if (PortInfo[i].description.find(target) != std::string::npos)
				PortName.push_back(PortInfo[i].port.c_str());
			if (PortName.size() >= MaxPortCount)
				break;
			
		}
		for (auto i = 0; i < PortName.size(); ++i)
		{
			std::string SetName = Name + std::to_string(i);
			if (Count == LineSwapSize)
			{
				Xpos = 0; Ypos += cellSizeY; Count = 0;
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
	ImVec2 topLeft = { 0.0f,0.0f };
	ImVec2 bottomRight = ImVec2(topLeft.x + 1200, topLeft.y + 780);
	ImU32 color = IM_COL32(205, 205, 205, 128);
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

	ImGui::SetNextWindowPos(ImVec2(MyImGui::MyImGuis->GetWindowSize_X() - 284, 0), ImGuiCond_Always);
	ImGui::Begin("Setting Box", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(294, 780));
	ComPortDataSetting();
	ImGui::SeparatorText("Connect");
	AllConnect();
	AllDisConnect();
	ComportReset();
	CLIBox();
	RadarTypeBox();
	ASCIILineMode();
	HEXLineMode();
	Frame_FPSBox(_io);
	ImGui::SeparatorText("LogBox");
	ImGui::Checkbox("LogBox ON/OFF", &UIVisible);
	ImGui::End();
}


void MyGUI_Interface::ComPortDataSetting()
{
	ImGui::SeparatorText("ComPortSetting");
	ImGui::Combo("BaudRate", &DataSettingBaudrate, BaudrateArray, IM_ARRAYSIZE(BaudrateArray));
	ImGui::Combo("Databit", &DataSettingDatabit, DatabitArray, IM_ARRAYSIZE(DatabitArray));
	ImGui::Combo("Stopbit", &DataSettingStopbit, StopbitArray, IM_ARRAYSIZE(StopbitArray));
	ImGui::Combo("Parity", &DataSettingParity, ParityArray, IM_ARRAYSIZE(ParityArray));
}


void MyGUI_Interface::RadarTypeBox()
{
	ImGui::SeparatorText("Radar Type");
	std::vector<serial::PortInfo> Infos = serial::list_ports();
	std::vector<const char*> BluetoothPort;
	std::vector<const char*> USBSerialCount;
	std::vector<const char*> ETCCount;
	for (serial::PortInfo& V : Infos)
	{
		if (V.description.find("Bluetooth") != std::string::npos)
			BluetoothPort.push_back(V.description.c_str());
		else if (V.description.find(target) != std::string::npos)
			USBSerialCount.push_back(V.description.c_str());
		else
			ETCCount.push_back(V.description.c_str());
	}
	static int USBinfo = 0;
	static int Bluetoothinfo = 0;
	static int ETCinfo = 0;
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Port detected %d",Infos.size());
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "USBSerial detected : %d", USBSerialCount.size());
	ImGui::Combo("USB(Serial)", &USBinfo, USBSerialCount.data(), USBSerialCount.size());
	ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.9f, 1.0f), "Blutooth detected : %d", BluetoothPort.size());
	ImGui::Combo("BlueTooth", &Bluetoothinfo, BluetoothPort.data(), BluetoothPort.size());
	ImGui::TextColored(ImVec4(0.7f,0.7f, 0.95f, 1.0f), "ETC Port detected : %d", ETCCount.size());
	ImGui::Combo("ETC", &ETCinfo, ETCCount.data(), ETCCount.size());
}


void MyGUI_Interface::LogManagementBox()
{
	ImGui::SetNextWindowPos(ImVec2(MyImGui::MyImGuis->GetWindowSize_X() * 0.7f, MyImGui::MyImGuis->GetWindowSize_Y() - 230), ImGuiCond_Always);
	ImGui::Begin("Manage Log", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(MyImGui::MyImGuis->GetWindowSize_X() * 0.3f, 230));
	LogClear();
	LogFileCreateSelect();
	DataSetting();
	ImGui::End();
}

void MyGUI_Interface::Frame_FPSBox(ImGuiIO& _io)
{
	ImGui::SeparatorText("Frame / FPS");
	ImGui::Text("Frame : %.3f ms/frame", 1000.0f / _io.Framerate);
	ImGui::Text("FPS : %.1f", _io.Framerate);
}


void MyGUI_Interface::LogBox()
{
	ImGui::SetNextWindowPos(ImVec2(0, MyImGui::MyImGuis->GetWindowSize_Y() - 230), ImGuiCond_Always);
	ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(MyImGui::MyImGuis->GetWindowSize_X() * 0.7f, 230));
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
	static char buffer[1500] = "";
	ImGui::Text("Input CLI");
	ImGui::InputTextMultiline("##output", buffer, sizeof(buffer), ImVec2(250, 300), ImGuiInputTextFlags_None | ImGuiInputTextFlags_EscapeClearsAll);
	if (ImGui::Button("AllSend"))
	{
		std::string CLI_Text = buffer;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->InputCLI(CLI_Text);
		}
	}
}

void MyGUI_Interface::ASCIILineMode()
{
	ImGui::SeparatorText("Data Port Mode(ASCII)");
	if(ImGui::RadioButton("ASCII 1(1port)", &ASCII_Button, 0))
	{
		HEX_Button = -1;
		LineSwapSize = 1;
		LineModeReset(1, 1200.0f, 780.0f);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("ASCII 2(6Port)", &ASCII_Button, 1))
	{
		HEX_Button = -1;
		LineSwapSize = 3;
		LineModeReset(6,400.0f,390.0f);
	}
	if (ImGui::RadioButton("ASCII 3(12Port)", &ASCII_Button, 2))
	{
		HEX_Button = -1;
		LineSwapSize = 3;
		LineModeReset(12, 300.0f, 260.0f);
	}
	ImGui::SameLine();
	if(ImGui::RadioButton("ASCII 4(36Port)", &ASCII_Button, 3))
	{
		HEX_Button = -1;
		LineSwapSize = 6;
		LineModeReset(36, 200.0f, 130.0f);
	}
}



void MyGUI_Interface::HEXLineMode()
{
	ImGui::SeparatorText("Data Port Mode(HEX)");
	if (ImGui::RadioButton("HEX 1(1port)", &HEX_Button, 0))
	{
		ASCII_Button = -1;
		LineSwapSize = 1;
		LineModeReset(1, 1200.0f, 780.0f);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("HEX 2(6Port)", &HEX_Button, 1))
	{
		ASCII_Button = -1;
		LineSwapSize = 3;
		LineModeReset(6, 400.0f, 390.0f);
	}
	if (ImGui::RadioButton("HEX 3(36Port)", &HEX_Button, 2))
	{
		ASCII_Button = -1;
		LineSwapSize = 6;
		LineModeReset(36, 200.0f, 130.0f);
	}
}



void MyGUI_Interface::LineModeReset(int _PortCount, float _SizeX, float _SizeY)
{
	MaxPortCount = _PortCount;
	cellSizeX = _SizeX;
	cellSizeY = _SizeY;
	for (std::shared_ptr<PortBox> obj : ObjectBox)
	{
		obj->DisConnect();
	}
	ButtonRelease();
}


void MyGUI_Interface::BoxInstance()
{
	for (auto i = 0; i < PortName.size(); ++i)
		ObjectBox[i]->Instance(PortName[i]);
}



void MyGUI_Interface::AllConnect()
{
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
		ButtonRelease();
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
	static bool LogBoxs = false;
	static bool PortRawData = false;
	ImGui::SeparatorText("Log File Record");
	if (ImGui::Checkbox("Log Box Record", &LogBoxs))
	{
		if (LogBoxs)
			MyImGui::MyImGuis->SetLogBoxBool();
		else
			MyImGui::MyImGuis->SetLogBoxFileClose();
	}
	if (ImGui::Checkbox("PortBox RawData Record", &PortRawData))
	{
		if (PortRawData)
		{
			for (std::shared_ptr<PortBox> obj : ObjectBox)
				obj->SetCOMLogFile();
		}
		else
		{
			for (std::shared_ptr<PortBox> obj : ObjectBox)
				obj->SetCOMLofFileClose();
		}
	}
}

void MyGUI_Interface::DataSetting()
{
	ImGui::SeparatorText("Data Setting");
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.8f, 1.0f));           
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.3f, 1.0f, 1.0f));    
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.1f, 0.1f, 0.6f, 1.0f));     
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));     
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::Text("Default : 5 second");
	ImGui::SliderInt("Undetected time", &Sliderint, 5, 10);
	ImGui::PopStyleColor(5);
	ImGui::Text("Default : START");
	ImGui::InputText("Boot Detection Char", BootDetection, IM_ARRAYSIZE(BootDetection));
}

void MyGUI_Interface::ButtonRelease()
{
	ObjectBox.clear();
	PortName.clear();
	CreateBool = true;
}
