#include "MyGUI_Interface.h"
#include "imgui.h"
#include "PortBox.h"




#define MaxPortCount 30

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
    RadarTypeBox();
    AllConnectBox();
    Frame_FPSBox(_io);
    LogBox();
    CLIBox();
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
				Xpos = 0; Ypos += 100; Count = 0;
			}
			Count++;
			ObjectBox.push_back(make_shared<PortBox>(Xpos, Ypos, SetName));
			Xpos += 180;
		}
		CreateBool = false;
	}
}

void MyGUI_Interface::DrawLine()
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


void MyGUI_Interface::RadarTypeBox()
{
	ImGui::SetNextWindowPos(ImVec2(1080, 500), ImGuiCond_Always);
	ImGui::Begin("Radar Type", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(200, 110));

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

void MyGUI_Interface::AllConnectBox()
{
	ImGui::SetNextWindowPos(ImVec2(1080, 0), ImGuiCond_Always);
	ImGui::Begin("All Check", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(200, 150));
	AllConnect();
	AllDisConnect();
	ComportReset();
	AllRowData();
	LogClear();
	ImGui::End();
}


void MyGUI_Interface::Frame_FPSBox(ImGuiIO& _io)
{
	ImGui::SetNextWindowPos(ImVec2(1080, 610), ImGuiCond_Always);
	ImGui::StyleColorsClassic();

	ImGui::Begin("Frame / FPS", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(200, 150));
	ImGui::Text("Frame : %.3f ms/frame", 1000.0f / _io.Framerate);
	ImGui::Text("FPS : %.1f", _io.Framerate);
	ImGui::End();
}


void MyGUI_Interface::LogBox()
{
	if (LogBoxbool)
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
}


void MyGUI_Interface::CLIBox()
{
	ImGui::SetNextWindowPos(ImVec2(1080, 150), ImGuiCond_Always);
	ImGui::Begin("CLI Box", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(200, 200));
	static char buffer[256] = "";
	ImGui::Text("Input CLI");
	ImGui::InputText("##InputBox", buffer, 64);
	if (ImGui::Button("AllSend"))
	{
		std::string CLI_Text = buffer;
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->InputCLI(CLI_Text);
		}
	}
	ImGui::End();
}



void MyGUI_Interface::AllConnect()
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

void MyGUI_Interface::AllDisConnect()
{
	if (ImGui::Button("All DisConnect"))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->DisConnect();
		}
	}
}

void MyGUI_Interface::ComportReset()
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

void MyGUI_Interface::LogClear()
{
	if (ImGui::Button("Log Clear"))
	{
		logs.clear();
	}
}

void MyGUI_Interface::AllRowData()
{
	if (ImGui::Button("Show All RawData"))
	{
		for (std::shared_ptr<PortBox> obj : ObjectBox)
		{
			obj->SetRawDataBox();
		}
	}
}

void MyGUI_Interface::ButtonRelease()
{
	ObjectBox.clear();
	PortName.clear();
	CreateBool = true;
}
