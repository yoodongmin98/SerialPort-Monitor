#include "PortBox.h"
#include "imgui.h"
#include <string>
#include "MessageBox.h"
#include <windows.h>
#include <iostream>
#include "MyTime.h"
#include <Windows.h>
#include <chrono>
#include "MyImGui.h"
#include <functional>
#include "ThreadPool.h"
#include "MyGUI_Interface.h"



ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // ����
ImVec4 yellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // ���
ImVec4 Colors = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // �ϴû�



PortBox::PortBox()
{

}

PortBox::PortBox(int _X, int _Y, std::string _Name)
	: X(_X), Y(_Y), BoxName(_Name)
{
	
}


PortBox::~PortBox()
{
	CloseSerialPort();
}


void PortBox::Instance(std::string _PortName)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customColor);
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_Always);
	ImGui::Begin(BoxName.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar);
	PortBoxSize = MyGUI_Interface::GUI->GetcellSize();
	ImGui::SetWindowSize(PortBoxSize);
	ImGui::PushItemWidth(50);


	String = _PortName;
	ImGui::Text("%s", String.c_str());

	ImGui::SameLine();
	if (ImGui::Button("Connect"))
		Connect();

	ImGui::SameLine();

	if (ImGui::Button("DisConnect"))
		DisConnect();

	if (IsLost)
		ImGui::TextColored(redColor, "Connection Lost");



	if (PortBoxBool)
	{
		if (LogFileBool) //����Ǿ����� ����
		{
			if(!logFile.is_open())
				logFile.open("Raw_" + _PortName + +".txt", std::ios::app);
			LogFileBool = false;
		}

		std::function<void()> Functions = std::bind(&PortBox::SerialMonitor, this);
		MyImGui::MyImGuis->GetThreadPool()->AddWork(Functions);

		if (WorkingBool) 
			ImGui::Text("Working%s", Dots.c_str());
		else if (MissingBool) 
			ImGui::TextColored(yellowColor, "Missing");
		else if (BootStart) 
			ImGui::TextColored(Colors, "Booting");
	}

	CreateRowDataBox();

	ImGui::End();
	ImGui::PopStyleColor();
}


void PortBox::SerialMonitor() 
{
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
				if (RawDataLog.size() >= 200)
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
				if ((PreHexCount % HexNumberCount) == 0)
				{
					RawHexLog.push_back(HexLineData);
					PreHexCount = 0;
					HexLineData.clear();
					scrollToBottom = true;
				}
			}
		

			if (!Dataline.find("\n") || Dataline.empty())
				logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << Dataline << std::endl << std::flush;
			else
				logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << Dataline << std::flush;

			if (!Dataline.empty())
			{
				dataReceived = true;
				// �����Ͱ� ������ �ϴ� Start�� ã�´�.
				if (Dataline.find("START") != std::string::npos) 
				{
					WorkingBool = false;
					MissingBool = false;
					BootStart = true;
					BootingTime = std::chrono::steady_clock::now();
				}
				else if(!BootStart) //���� ã���� BootStart�� true�� �ǹǷ� BootStart�� �ؿ��� 3�� ������������ ���� �ȵ������ؼ�
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
			currentMissingTime = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(currentMissingTime - MissingTime).count() >= 5) {
				WorkingBool = false;
				MissingBool = false;
				BootStart = false;
				MyImGui::MyImGuis->LogFlash(String, "�� �����Ͱ� 5�� �̻� ���ŵ��� �ʾҽ��ϴ�.");
				MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " No data received for 5 seconds");
			}
		}

		// BootStart ���¿��� ���� �ð��� ����ϸ� ó��
		if (BootStart) 
		{
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
	catch (const std::exception& e) {
		// ���� ó�� �� �α� ���
		std::lock_guard<std::mutex> lock(stateMutex);
		IsLost = true;
		MyImGui::MyImGuis->LogFlash(String, "�� �ø��� ����� ������ϴ�.");
		MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " Serial communication was lost");
		CloseSerialPort();
		return;
	}
}



void PortBox::CreatePortLogFile()
{

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
			my_serial.setBaudrate(921600);
			my_serial.setTimeout(timeout);
			MyGUI_Interface::GUI->GetASCIIButton() >= 0 ? SetASCIIMODE() : SetHEXMODE();
			switch (MyGUI_Interface::GUI->GetMaxPortCount())
			{
			case 1: {
				SetHexPrintNumberCount(70);
				break;
			}
			case 6: {
				SetHexPrintNumberCount(22);
				break;
			}
			case 36: {
				SetHexPrintNumberCount(10);
				break;
			}
			default:
				break;
			}
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
		{ //���� ����
			std::lock_guard<std::mutex> lock(serialMutex);
			if (ASCIIMODE)
			{
				for (const auto& Rawlog : RawDataLog)
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

			if (scrollToBottom)
			{
				ImGui::SetScrollHereY(1.0f);
				scrollToBottom = false;
			}
			
		}
		ImGui::EndChild();
		PortBoxSize = ImVec2{ 250 , 130 };
}

void PortBox::InputCLI(std::string& _CLI)
{
	{
		std::lock_guard<std::mutex> lock(serialMutex);
		if(my_serial.isOpen())
			my_serial.write(_CLI+"\n");
	}
}