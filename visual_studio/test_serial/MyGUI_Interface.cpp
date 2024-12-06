#include "MyGUI_Interface.h"
#include "imgui.h"
#include "PortBox.h"
#include "MyImGui.h"




#define MaxPortCount 42
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
    PortBoxCreate();
    DrawLine();
    AllConnectBox(_io);
	LogManagementBox();
    LogBox();
    for (auto i = 0; i < PortName.size(); ++i)
        ObjectBox[i]->Instance(PortName[i]);
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
		}
		for (auto i = 0; i < MaxPortCount; ++i)
		{
			std::string SetName = Name + std::to_string(i);
			if (Count == 6)
			{
				Xpos = 0; Ypos += 130; Count = 0;
			}
			Count++;
			ObjectBox.push_back(make_shared<PortBox>(Xpos, Ypos, SetName));
			Xpos += 250;
		}
		CreateBool = false;
	}
}

void MyGUI_Interface::DrawLine()
{
	ImVec2 topLeft = { 0.0f,0.0f };
	ImVec2 bottomRight = ImVec2(topLeft.x + 1500, topLeft.y + 780);
	float cellSizeX = 250.0f; // X 크기
	float cellSizeY = 130.0f; // Y 크기
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
	ImGui::Begin("All Check", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(294, 600));
	ImGui::SeparatorText("Connect");
	AllConnect();
	AllDisConnect();
	ComportReset();
	CLIBox();
	RadarTypeBox();
	Frame_FPSBox(_io);

	ImGui::End();
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
	ImGui::SetNextWindowPos(ImVec2(1080, MyImGui::MyImGuis->GetWindowSize_Y() - 230), ImGuiCond_Always);
	ImGui::Begin("Manage Log", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(100, 230));
	LogClear();
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
	ImGui::SetWindowSize(ImVec2(1080, 230));
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
	static char buffer[256] = "";
	ImGui::Text("Input CLI");
	ImGui::InputText("##InputBox", buffer, 64);
	ImGui::SameLine();
	if (ImGui::Button("AllSend"))
	{
		std::string CLI_Text = buffer;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->InputCLI(CLI_Text);
		}
	}
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

void MyGUI_Interface::ButtonRelease()
{
	ObjectBox.clear();
	PortName.clear();
	CreateBool = true;
}
