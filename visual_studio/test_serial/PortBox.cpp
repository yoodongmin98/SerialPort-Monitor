#include "PortBox.h"
#include "imgui.h"
#include <string>
#include "MessageBox.h"
#include <windows.h>
#include <iostream>
#include "MyTime.h"
#include <serial/serial.h>



ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 빨강
ImVec4 yellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // 노랑



PortBox::PortBox()
{

}

PortBox::PortBox(int _X, int _Y, std::string _Name)
	: X(_X) , Y(_Y) , BoxName(_Name)
{
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(250, 120));

}

PortBox::~PortBox()
{

}

void PortBox::Instance() //위치정보를 인자로 받아서 위치 세팅해야함
{
	
	ImGui::Begin(BoxName.c_str());
	
	ImGui::PushItemWidth(50);
	ImGui::InputText("Input Comport Number", inputString, IM_ARRAYSIZE(inputString));
	bool isConnectClicked = ImGui::Button("Connect");
	if (isConnectClicked) 
	{
		PortBoxBool = true;
		String = inputString;
		if (!String.empty())
			Number = stoi(String);
		String = "COM" + String;
		my_serial.setPort(String);
		my_serial.setBaudrate(921600);
		PortCheck();
	}
	ImGui::SameLine();
	bool isDisconnectClicked = ImGui::Button("DisConnect");
	if (isDisconnectClicked)
	{
		PortBoxBool = false;
		String.clear();
		Number = 0;
		my_serial.close();
	}
	if (IsLost)
	{
		ImGui::TextColored(redColor, "Connection Lost");
	}
		
	if (PortBoxBool)
	{
		SerialMonitor(String);
	}
	ImGui::End();
}


void PortBox::SerialMonitor(const std::string _ComPort)
{
	//baudrate물어보기 ㅇㅇ
	try
	{
		try {
			Dataline = my_serial.readline();
		}
		catch (const std::exception& e) {
			IsLost = true;
			std::cout << _ComPort + "의 시리얼 통신이 끊겼습니다. " << MyTime::Time->GetLocalTime() << std::endl;
			PortBoxBool = false;
			return;
		}

		if (!Dataline.empty())
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
		else
		{
			ImGui::TextColored(yellowColor, "Missing");
			std::cout << _ComPort + "의 데이터가 수신되지 않았습니다. " << MyTime::Time->GetLocalTime() << std::endl;
		}
	}
	catch (const serial::IOException& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("올바른 포트번호가 입력되지 않았습니다.");
		PortBoxBool = false;
	}
	catch (const std::exception& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("알 수 없는 오류가 발생했습니다.");
		PortBoxBool = false;
	}
	catch (...)
	{
		MsgBox::Msg->ShowWarningMessageBox("알 수 없는 오류가 발생했습니다.");
		PortBoxBool = false;
	}

}


void PortBox::PortCheck()
{
	try
	{
		my_serial.open();
		if (Number <= 0 || Number > 255)
		{
			MsgBox::Msg->ShowWarningMessageBox("1에서 255사이의 숫자를 입력해주세요");
			PortBoxBool = false;
		}
		else
		{
			IsLost = false;
		}
	}
	catch (const std::exception e)
	{
		MsgBox::Msg->ShowWarningMessageBox("연결할 수 없는 포트 입니다.");
		return;
	}
	catch (const std::invalid_argument& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("숫자 형식이 잘못되었습니다.");
		return;
	}
	catch (const std::out_of_range& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("입력한 숫자가 너무 큽니다.");
		return;
	}
}