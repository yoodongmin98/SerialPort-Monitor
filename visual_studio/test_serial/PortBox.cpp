#include "PortBox.h"
#include "imgui.h"
#include <string>
#include "MessageBox.h"
#include <windows.h>
#include <iostream>
#include "MyTime.h"
#include <serial/serial.h>



ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // ����
ImVec4 yellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // ���



PortBox::PortBox()
{

}

PortBox::PortBox(int _X, int _Y, std::string _Name)
	: X(_X) , Y(_Y) , BoxName(_Name)
{

}

PortBox::~PortBox()
{

}

void PortBox::Instance() //��ġ������ ���ڷ� �޾Ƽ� ��ġ �����ؾ���
{
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_Appearing);
	ImGui::Begin(BoxName.c_str());
	ImGui::SetWindowSize(ImVec2(250, 120));
	ImGui::PushItemWidth(50);
	ImGui::InputText("Input Comport Number", inputString, IM_ARRAYSIZE(inputString));
	if (ImGui::Button("Connect"))
	{
		PortBoxBool = true;
		String=inputString;
		if(!String.empty())
			Number = stoi(String);
		String = "COM" + String;
	}
	ImGui::SameLine();
	if (ImGui::Button("DisConnect"))
	{
		PortBoxBool = false;
		String.clear();
		Number = 0;
	}
	if(IsLost)
		ImGui::TextColored(redColor, "Connection Lost");
		
	if (PortBoxBool)
	{
		try
		{
			if (Number <= 0 || Number > 255)
			{
				MsgBox::Msg->ShowWarningMessageBox("1���� 255������ ���ڸ� �Է����ּ���");
				PortBoxBool = false;
			}
			else
			{
				IsLost = false;
				SerialMonitor(String);
			}
		}
		catch (const std::invalid_argument& e)
		{
			MsgBox::Msg->ShowWarningMessageBox("���� ������ �߸��Ǿ����ϴ�.");
			return;
		}
		catch (const std::out_of_range& e)
		{
			MsgBox::Msg->ShowWarningMessageBox("�Է��� ���ڰ� �ʹ� Ů�ϴ�.");
			return;
		}
	}
	ImGui::End();
}


void PortBox::SerialMonitor(const std::string _ComPort)
{
	//baudrate����� ����
	try
	{
		serial::Serial my_serial(_ComPort, 921600, serial::Timeout::simpleTimeout(1000));
		try {
			Dataline = my_serial.readline();
		}
		catch (const std::exception& e) {
			IsLost = true;
			std::cout << _ComPort + "�� �ø��� ����� ������ϴ�. " << MyTime::Time->GetLocalTime() << std::endl;
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
			std::cout << _ComPort + "�� �����Ͱ� ���ŵ��� �ʾҽ��ϴ�. " << MyTime::Time->GetLocalTime() << std::endl;
		}
	}
	catch (const serial::IOException& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("�ùٸ� ��Ʈ��ȣ�� �Էµ��� �ʾҽ��ϴ�.");
		PortBoxBool = false;
	}
	catch (const std::exception& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("�� �� ���� ������ �߻��߽��ϴ�.");
		PortBoxBool = false;
	}
	catch (...)
	{
		MsgBox::Msg->ShowWarningMessageBox("�� �� ���� ������ �߻��߽��ϴ�.");
		PortBoxBool = false;
	}

}