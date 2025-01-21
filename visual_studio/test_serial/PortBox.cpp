#include "DataFile.h"
#include "EspUploader.h"
#include "imgui.h"
#include "MessageBox.h"
#include "MyGUI_Interface.h"
#include "MyImGui.h"
#include "MyTime.h"
#include "PortBox.h"
#include "ThreadPool.h"
#include "DebugPortBox.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <windows.h>
#include <Windows.h>







PortBox::PortBox()
{
	
}

PortBox::PortBox(int _X, int _Y, std::string _Name)
	: X(_X), Y(_Y), BoxName(_Name)
{
	ESP = std::make_shared<EspUploader>();
	D_Port = std::make_shared<DebugPortBox>();
	//ESP CallBack
	SetCallBackEvent();
	Set_X_Click_CallBackEvent();
}


PortBox::~PortBox()
{
	CloseSerialPort();
}



void PortBox::Instance(std::string& _PortName)
{
	GUISetting();
	DataSet();
	CreatePortButton(_PortName);
	DebugPortSetting(_PortName);
	InsertTask_WorkingCheck(_PortName);
	CreateRowDataBox();
}


void PortBox::GUISetting()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, PORTBOXCOLOR);
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_Always);
	//enum ImGuiCond_
	//{
	//	ImGuiCond_None = 0,        // No condition (always set the variable), same as _Always
	//	ImGuiCond_Always = 1 << 0,   // No condition (always set the variable), same as _None
	//	ImGuiCond_Once = 1 << 1,   // Set the variable once per runtime session (only the first call will succeed)
	//	ImGuiCond_FirstUseEver = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
	//	ImGuiCond_Appearing = 1 << 3,   // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
	//};
	ImGui::Begin(BoxName.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar);
	PortBoxSize = MyGUI_Interface::GUI->GetcellSize();
	ImGui::SetWindowSize(PortBoxSize);
	ImGui::PushItemWidth(50);
}


void PortBox::DataSet()
{
	NoDataTime = MyGUI_Interface::GUI->GetSliderInt();
	BootingString = MyGUI_Interface::GUI->GetBootDetectionCharacter();
	BaudRate = MyGUI_Interface::GUI->GetDataSettingBaudrate();
	Databit = MyGUI_Interface::GUI->GetDataSettingDatabit();
	Stopbit = MyGUI_Interface::GUI->GetDataSettingStopbit();
	WinSize = MyImGui::MyImGuis->GetWindowSize_X();
}


void PortBox::CreatePortButton(std::string& _PortName)
{
	String = _PortName;
	ImGui::Text("%s", String.c_str());

	if (MyGUI_Interface::GUI->GetViewBool())
	{
		ImGui::SameLine();
		if (ImGui::Button("Conn"))
			Connect();

		ImGui::SameLine();

		if (ImGui::Button("Disc"))
			DisConnect();

		ImGui::SameLine();

		if (ImGui::Button("CLI"))
			InputCLI(MyGUI_Interface::GUI->GetCLIText());
	}
	
	if (IsLost)
		ImGui::TextColored(REDCOLOR, "Connection Lost");
}


void PortBox::InsertTask_WorkingCheck(std::string& _PortName)
{
	if (PortBoxBool)
	{
		if (LogFileBool)
		{
			if (!logFile.is_open())
				logFile.open("Raw_" + _PortName + +".txt", std::ios::app);
			LogFileBool = false;
		}

 		std::function<void()> Functions = std::bind(&PortBox::SerialMonitor, this);
		MyImGui::MyImGuis->GetThreadPool()->AddWork(Functions);

		if (WorkingBool)
			ImGui::Text("Working%s", Dots.c_str());
		else if (MissingBool)
			ImGui::TextColored(YELLOWCOLOR, "Missing");
		else if (BootStart)
			ImGui::TextColored(BLUECOLOR, "Booting");
	}
}


void PortBox::SerialMonitor() 
{
	//�굵 �����ϸ� ���������� ������ ����
	try 
	{
		if (!my_serial.isOpen())
				return;

		bool dataReceived = false;

		// ������ �б�
		if (my_serial.available()) 
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			if (ASCIIMODE)
				Dataline = my_serial.readline();
			else if (HEXMODE)
				Dataline = my_serial.read();
			
			if (!Dataline.empty())
			{
				DotCount++;
				if (DotCount > 7) 
					DotCount = 1;
				Dots.clear();
				for (int i = 0; i < DotCount; ++i) 
					Dots += ".";
			}
			
			//RawDataBox������ ��ũ�� �߰��ϴ°�
			if (ASCIIMODE)
			{
				if (RawDataLog.size() >= 1000)
					RawDataLog.pop_front(); 
				RawDataLog.push_back(Dataline); 
				scrollToBottom = true;
			}
			else if (HEXMODE)
			{
				hexStream.str("");
				for (unsigned char c : Dataline)
					hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
				HexLineData += hexStream.str();
				PreHexCount++;
				if ((PreHexCount - HexNumberCount) == 0)
				{
					RawHexLog.push_back(HexLineData);
					PreHexCount = 0;
					HexLineData.clear();
					scrollToBottom = true;
				}
			}
			
			//������ ���
			if (ASCIIMODE)
			{
				if (!Dataline.find("\n") || Dataline.empty())
					logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << Dataline << std::endl << std::flush;
				else
					logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << Dataline << std::flush;
			}
			else if(HEXMODE) //���⿡ ���� ������ ����� �ʿ��� ���� �������� ū ����
				logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << HexLineData << std::endl << std::flush;
			

			if (!Dataline.empty())
			{
				dataReceived = true;
				// �����Ͱ� ������ �ϴ� Start�� ã�´�.
				if (Dataline.find(BootingString) != std::string::npos)
				{
					WorkingBool = false;
					MissingBool = false;
					BootStart = true;
					BootingTime = std::chrono::steady_clock::now();
				}
				else if(!BootStart) //���� ã���� BootStart�� true�� �ǹǷ� BootStart�� �ؿ��� 5�� ������������ ���� �ȵ������ؼ�
					//���� ��µǴ°� ����
				{
					// �����Ͱ� ������� ������ Working ����
					WorkingBool = true;
					MissingBool = false;
					BootStart = false;
				}
			}
		}
		 
		//���� �����Ͱ� ��� datareceived�� �ȹٲ�� false�϶� �Ǵ� MissingBool�� true�϶�
		if (!dataReceived && !BootStart)
		{
			//�̶����� �ð��� ��� ����
			if (!MissingBool)
			{
				MissingTime = std::chrono::steady_clock::now();
				MissingBool = true;
			}
		}

		//�ð��� ��� ������ �������� �̰� ���ư��� 5�ʰ� ������ �۵���
		if (MissingBool) 
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			currentMissingTime = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(currentMissingTime - MissingTime).count() >= NoDataTime) {
				WorkingBool = false;
				MissingBool = false;
				BootStart = false;
				MyImGui::MyImGuis->LogFlash(String, "�� �����Ͱ� �����ð� �̻� ���ŵ��� �ʾҽ��ϴ�.");
				MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " No data received for " + std::to_string(NoDataTime) + " seconds");
			}
		}

		// BootStart ���¿��� ���� �ð��� ����ϸ� ó��
		if (BootStart) 
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			currentBootingTime = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(currentBootingTime - BootingTime).count() >= 5) {
				WorkingBool = false;
				MissingBool = false;
				BootStart = false;
				MyImGui::MyImGuis->LogFlash(String, "�� ���õǾ����ϴ�.");
				MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " Boot completed");
			}
		}
		
	}
	catch (const std::exception& e)
	{
		// ���� ó�� �� �α� ���
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			IsLost = true;
			MyImGui::MyImGuis->LogFlash(String, "�� �ø��� ����� ������ϴ�.");
			MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " Serial communication was lost");
		}

		CloseSerialPort();
	}
}



void PortBox::PortCheck()
{
	try
	{
		my_serial.open();
		my_serial.setRTS(false);
		my_serial.setDTR(false);
	}
	catch (const std::exception e)
	{
		PortBoxBool = false;
		return;
	}
	catch (const std::invalid_argument& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("���� ������ �߸��Ǿ����ϴ�.");
		PortBoxBool = false;
		return;
	}
	catch (const std::out_of_range& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("�Է��� ���ڰ� �ʹ� Ů�ϴ�.");
		PortBoxBool = false;
		return;
	}
}

void PortBox::Connect()
{
	{
		std::lock_guard<std::mutex> lock(serialMutex);
		if (!my_serial.isOpen())
		{
			PortBoxBool = true;
			IsLost = false;
			my_serial.setPort(String);
			my_serial.setBaudrate(BaudRate);
			my_serial.setBytesize(serial::bytesize_t::eightbits);
			my_serial.setTimeout(timeout);
			ASCII_HEX_Setting();
			PortCheck();
		}
	}
}


void PortBox::DisConnect()
{
	CloseSerialPort();
}

void PortBox::RawMonitorClear()
{
	RawDataLog.clear();
}

void PortBox::CloseSerialPort()
{
	{
		std::lock_guard<std::mutex> lock(serialMutex);
		if (my_serial.isOpen())
		{
			my_serial.setRTS(true);
			my_serial.setDTR(true);
			my_serial.close();
			PortBoxBool = false;
			String.clear();
		}
	}
}


void PortBox::CreateRowDataBox()
{
	ImGui::BeginChild("Row Data", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
	{ 
		if (ASCIIMODE)
		{
			for (std::string Rawlog : RawDataLog)
				ImGui::Text("%s", Rawlog.c_str());
		}
		else if (HEXMODE)
		{
			for (auto& Rawlog : RawHexLog)
			{
				try
				{
					ImGui::Text("%s", Rawlog.c_str());
				}
				catch (const std::exception e)
				{
					std::cout << "Hex ���ڿ��� �д� ���� ������ �߻��߽��ϴ�. error : " << e.what() << std::endl;
					return;
				}
			}
		}

		float scrollY = ImGui::GetScrollY();      // ���� ��ũ�� ��ġ
		float scrollMaxY = ImGui::GetScrollMaxY();// ��ũ�� ������ �ִ� ��ġ
		bool isAtBottom = (scrollY >= scrollMaxY-50);// ���� ��ũ���� �� �Ʒ����� Ȯ��

	
		if ((scrollToBottom && isAtBottom) || (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)))
		{
			ImGui::SetScrollHereY(1.0f);
			scrollToBottom = false;
		}

	}
	ImGui::EndChild();
	PortBoxSize = ImVec2{ 250 , 130 };
	ImGui::End();
	ImGui::PopStyleColor();
}

void PortBox::InputCLI(std::string& _CLI)
{
	{
		std::lock_guard<std::mutex> lock(serialMutex);
		if(my_serial.isOpen())
			my_serial.write(_CLI+"\n");
	}
}


void PortBox::ASCII_HEX_Setting()
{
	MyGUI_Interface::GUI->GetASCIIButton() >= ZERO ? SetASCIIMODE() : SetHEXMODE();
	switch (MyGUI_Interface::GUI->GetMaxPortCount())
	{
	case 1: {
		if(WinSize> WINDOW_CHECK_SIZE)
			SetHexPrintNumberCount(69);
		else
			SetHexPrintNumberCount(55);
		break;
	}
	case 6: {
		if (WinSize > WINDOW_CHECK_SIZE)
			SetHexPrintNumberCount(22);
		else
			SetHexPrintNumberCount(17);
		break;
	}
	case 12: {
		if (WinSize > WINDOW_CHECK_SIZE)
			SetHexPrintNumberCount(16);
		else
			SetHexPrintNumberCount(12);
		break;
	}
	case 30:
	case 42: {
		if (WinSize > WINDOW_CHECK_SIZE)
			SetHexPrintNumberCount(10);
		else
			SetHexPrintNumberCount(8);
		break;
	}
	default:
		break;
	}
}



void PortBox::StartESPFlash(std::vector<std::string>& _FileName)
{
	std::function<void()> Functions = std::bind(&EspUploader::Instance, ESP.get(), std::ref(String), std::ref(_FileName));
	MyImGui::MyImGuis->GetThreadPool()->AddWork(Functions);
}


void PortBox::DebugPortSetting(std::string& _PortName)
{
	ImGui::SameLine();
	if (ImGui::Button("+"))
	{
		Bebug = !Bebug;
	}
	if (Bebug)
	{
		if (nullptr == D_Port)
		{
			D_Port = std::make_shared<DebugPortBox>();
			Set_X_Click_CallBackEvent();
		}
		D_Port->Instance(_PortName, RawDataLog);
	}
}