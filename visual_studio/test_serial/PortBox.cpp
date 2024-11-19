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
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(250, 120));

}

PortBox::~PortBox()
{

}

void PortBox::Instance() //��ġ������ ���ڷ� �޾Ƽ� ��ġ �����ؾ���
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
	//baudrate����� ����
	try
	{
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


void PortBox::PortCheck()
{
	try
	{
		my_serial.open();
		if (Number <= 0 || Number > 255)
		{
			MsgBox::Msg->ShowWarningMessageBox("1���� 255������ ���ڸ� �Է����ּ���");
			PortBoxBool = false;
		}
		else
		{
			IsLost = false;
		}
	}
	catch (const std::exception e)
	{
		MsgBox::Msg->ShowWarningMessageBox("������ �� ���� ��Ʈ �Դϴ�.");
		return;
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