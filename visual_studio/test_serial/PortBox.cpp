#include "PortBox.h"
#include "imgui.h"
#include <string>
#include "MessageBox.h"
#include "SerialMonitor.h"
#include <windows.h>
#include <iostream>




PortBox* PortBox::Box = nullptr;
PortBox::PortBox()
{
	Box = this;
	Monitors = std::make_shared<SerialMonitor>();
}
PortBox::~PortBox()
{

}

void PortBox::Instance() //위치정보를 인자로 받아서 위치 세팅해야함
{
	ImGui::Begin("PortBox1");
	ImGui::SetWindowSize(ImVec2(250, 150));
	ImGui::Text("Set Port Number");
	ImGui::PushItemWidth(50);
	ImGui::InputText("Input Comport Number", inputString, IM_ARRAYSIZE(inputString));
	if (ImGui::Button("Button"))
	{
		PortBoxBool = true;
		String=inputString;
		if(!String.empty())
			Number = stoi(String);
		String = "COM" + String;
	}
	if (ImGui::Button("DisConnect"))
	{
		PortBoxBool = false;
		String.clear();
		Number = 0;
	}
		
	if (PortBoxBool)
	{
		try
		{
			if (Number <= 0 || Number > 255)
			{
				MsgBox::Msg->ShowWarningMessageBox("1에서 255사이의 숫자를 입력해주세요");
				PortBoxBool = false;
			}
			else
			{
				Monitors->Instance(String);
			}
		}
		catch (const std::invalid_argument& e)
		{
			MsgBox::Msg->ShowWarningMessageBox("숫자 형식이 잘못되었습니다.");
		}
		catch (const std::out_of_range& e)
		{
			MsgBox::Msg->ShowWarningMessageBox("입력한 숫자가 너무 큽니다.");
		}
	}
	ImGui::End();
}