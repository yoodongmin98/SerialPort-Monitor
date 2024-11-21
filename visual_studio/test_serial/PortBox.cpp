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





ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 빨강
ImVec4 yellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // 노랑
ImVec4 Colors = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // 하늘색



PortBox::PortBox()
{

}

PortBox::PortBox(int _X, int _Y, std::string _Name)
	: X(_X), Y(_Y), BoxName(_Name)
{

}


PortBox::~PortBox()
{
	
}


void PortBox::Instance(std::string& _PortName)
{
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_Always);
	ImGui::Begin(BoxName.c_str());
	ImGui::SetWindowSize(ImVec2(200, 100));
	ImGui::PushItemWidth(50);

	String = _PortName;
	ImGui::Text("%s", String.c_str());
	

	bool isConnectClicked = ImGui::Button("Connect");
	if (isConnectClicked)
	{
		Connect();
	}
	ImGui::SameLine();
	bool isDisconnectClicked = ImGui::Button("DisConnect");
	if (isDisconnectClicked)
	{
		DisConnect();
	}
	if (IsLost)
	{
		ImGui::TextColored(redColor, "Connection Lost");
	}
	if (PortBoxBool)
	{
		SerialMonitor();
	}
	ImGui::End();
}


void PortBox::SerialMonitor()
{
	try
	{
		try
		{
			Dataline = my_serial.readline();
			if (Dataline == "START" || Dataline == "REBOOT")
				MyImGui::MyImGuis->LogFlash(String, "를 재시작합니다 ");
		}
		catch (const std::exception& e) {
			IsLost = true;
			MyImGui::MyImGuis->LogFlash(String, " 의 시리얼 통신이 끊겼습니다. ");
			std::cout << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "]" << String + " 의 시리얼 통신이 끊겼습니다. " << std::endl;
			PortBoxBool = false;
			String.clear();
			my_serial.close();
			return;
		}
	
		
		// 데이터가 비어있지 않을 때
		if (!Dataline.empty() && Dataline[0] == '#')
		{
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
		else if (!Dataline.empty() && Dataline[0] != '#')
		{
			ImGui::TextColored(Colors, "Booting");
			if (BootingBool)
			{
				BootingTime = std::chrono::steady_clock::now();
				BootingBool = false;
			}
			currentBootingTime = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(currentBootingTime - BootingTime).count() >= 5)
			{
				MyImGui::MyImGuis->LogFlash(String, "을 부팅중입니다 ");
				std::cout << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << String + "을 부팅중입니다 " << MyTime::Time->GetLocalTime() << std::endl;
				BootingBool = true;
			}
		}
		else
		{
			ImGui::TextColored(yellowColor, "Missing");
			if (MissingBool)
			{
				MissingTime = std::chrono::steady_clock::now();
				MissingBool = false;
			}
			currentMissingTime = std::chrono::steady_clock::now();
			// 데이터가 비었을 때
			if (std::chrono::duration_cast<std::chrono::seconds>(currentMissingTime - MissingTime).count() >= 5)
			{
				MyImGui::MyImGuis->LogFlash(String, "의 데이터가 수신되지 않았습니다. ");
				std::cout << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << String + "의 데이터가 수신되지 않았습니다. " << std::endl;
				MissingBool = true;
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




void PortBox::PortCheck()
{
	try
	{
		my_serial.open();
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

void PortBox::DisConnect()
{
	if (my_serial.isOpen())
	{
		PortBoxBool = false;
		String.clear();
		my_serial.close();
	}
}