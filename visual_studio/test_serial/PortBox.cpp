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



ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 빨강
ImVec4 yellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // 노랑
ImVec4 Colors = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // 하늘색



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
		if (LogFileBool) //연결되었을때 생성
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
			// 멀티쓰레드에서 queue에 추가된 작업들이 꺼진상태에서도 작업을 실행하지않도록  serial여부를 체크
			if (!my_serial.isOpen())
				return;
			//데이터를 라인 기준으로 읽어옴
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
			MyImGui::MyImGuis->LogFlash(String, " 의 시리얼 통신이 끊겼습니다. ");
			MyImGui::MyImGuis->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "]" + String + " Serial communication was lost");
			CloseSerialPort();
			return;
		}
	
		// 데이터가 비어있지 않을 때
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
				MyImGui::MyImGuis->LogFlash(String, "이 부팅되었습니다");
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
					MyImGui::MyImGuis->LogFlash(String, "의 데이터가 수신되지 않았습니다. ");
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
		MsgBox::Msg->ShowWarningMessageBox("알 수 없는 오류가 발생했습니다.");
		PortBoxBool = false;
	}
	catch (...) {
		MsgBox::Msg->ShowWarningMessageBox("알 수 없는 오류가 발생했습니다.");
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
		MsgBox::Msg->ShowWarningMessageBox("숫자 형식이 잘못되었습니다.");
		PortBoxBool = false;
		return;
	}
	catch (const std::out_of_range& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("입력한 숫자가 너무 큽니다.");
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
		{ //경합 방지
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