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
#include "PortBoxChild.h"



ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // ����
ImVec4 yellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // ���
ImVec4 Colors = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // �ϴû�



PortBox::PortBox()
{
	
}

PortBox::PortBox(int _X, int _Y, std::string _Name)
	: X(_X), Y(_Y), BoxName(_Name)
{
	ChildBox = std::make_shared<PortBoxChild>();
}


PortBox::~PortBox()
{
	CloseSerialPort();
}


void PortBox::Instance(std::string _PortName)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customColor);
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_Always);
	ImGui::Begin(BoxName.c_str(), nullptr,ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowSize(PortBoxSize);
	ImGui::PushItemWidth(50);


	String = _PortName;
	ImGui::Text("%s", String.c_str());
	

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
			logFile.open("ABB_Raw_" + _PortName+ +".txt", std::ios::app);
			LogFileBool = false;
		}


		std::function<void()> Functions = std::bind(&PortBox::SerialMonitor, this);
		MyImGui::MyImGuis->GetThreadPool()->AddWork(Functions);


		if (!BootStart)
			ImGui::TextColored(yellowColor, "Missing");
		else if (BootStart)
			ImGui::TextColored(Colors, "Booting");
		{
			std::lock_guard<std::mutex> lock(serialMutex);
			ChildBox->SendRowData(Dataline);
		}
	}

	CreateRowDataBox();

	ImGui::End();
	ImGui::PopStyleColor();
}


void PortBox::SerialMonitor()
{
	try
	{
		try
		{
			// ��Ƽ�����忡�� queue�� �߰��� �۾����� �������¿����� �۾��� ���������ʵ���  serial���θ� üũ
			if (!my_serial.isOpen())
				return;
			//�����͸� ���� �������� �о��
			if (my_serial.available())
			{
				Dataline = my_serial.readline();
				if (!Dataline.empty())
					MissingBool = true;
				else if (!Dataline.find("\n") || Dataline.empty())
					logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << Dataline << std::endl << std::flush;
				else if (Dataline.find("START") != std::string::npos && !BootStart)
					BootStart = true;

				logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << Dataline << std::flush;
			}
			else
				return;
		}
		catch (const std::exception& e) {
			IsLost = true;
			MyImGui::MyImGuis->LogFlash(String, " �� �ø��� ����� ������ϴ�. ");
			MyImGui::MyImGuis->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "]" + String + " Serial communication was lost");
			CloseSerialPort();
			return;
		}
	
		// �����Ͱ� ������� ���� ��
		if (!Dataline.empty() && Dataline[0] == '#')
		{
			BootStart = false;
			DotCount++;
			if (DotCount > 7) {
				DotCount = 1;
			}
			Dots.clear();
			for (int i = 0; i < DotCount; ++i) {
				Dots += ".";
			}
			ImGui::Text("Working%s", Dots.c_str());
		}
		else if (BootStart)
		{
			if (BootingBool)
			{
				BootingTime = std::chrono::steady_clock::now();
				BootingBool = false;
			}
			currentBootingTime = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(currentBootingTime - BootingTime).count() >= 3)
			{
				MyImGui::MyImGuis->LogFlash(String, "�� ���õǾ����ϴ�");
				MyImGui::MyImGuis->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "]" + String + " Port has rebooted");
				BootingBool = true;
			}
		}
		else 
		{
			if (!BootStart)
			{
				if (MissingBool)
				{
					MissingTime = std::chrono::steady_clock::now();
					MissingBool = false;
				}
				currentMissingTime = std::chrono::steady_clock::now();
				if (std::chrono::duration_cast<std::chrono::seconds>(currentMissingTime - MissingTime).count() >= 6)
				{
					MyImGui::MyImGuis->LogFlash(String, "�� �����Ͱ� ���ŵ��� �ʾҽ��ϴ�. ");
					MyImGui::MyImGuis->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "]" + String + " No data was received");
					MissingBool = true;
				}
			}
			
		}
	}
	catch (const serial::IOException& e) {
		PortBoxBool = false;
	}
	catch (const std::exception& e) {
		MsgBox::Msg->ShowWarningMessageBox("�� �� ���� ������ �߻��߽��ϴ�.");
		PortBoxBool = false;
	}
	catch (...) {
		MsgBox::Msg->ShowWarningMessageBox("�� �� ���� ������ �߻��߽��ϴ�.");
		PortBoxBool = false;
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
			PortCheck();
		}
	}
}

void PortBox::DisConnect()
{
	CloseSerialPort();
}

void PortBox::CloseSerialPort()
{
	{
		std::lock_guard<std::mutex> lock(serialMutex);
		if (my_serial.isOpen())
		{
			my_serial.close();
			PortBoxBool = false;
			String.clear();
			RowDataBox = false;
		}
	}
}


void PortBox::CreateRowDataBox()
{
	ImGui::Checkbox("Row Data", &RowDataBox);
	if (RowDataBox)
	{
		ImGui::BeginChild("Row Data", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
		{ //���� ����
			std::lock_guard<std::mutex> lock(serialMutex);
			ImGui::Text(Dataline.c_str());
		}
		ImGui::EndChild();
		PortBoxSize = ImVec2{ 400 , 150 };
	}
	else
	{
		PortBoxSize = ImVec2{ 180 , 100 };
	}
}