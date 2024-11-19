#include "PortBox.h"
#include "imgui.h"
#include <string>
#include "MessageBox.h"
#include <windows.h>
#include <iostream>
#include "MyTime.h"
#include <Windows.h>



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
	if (logFile.is_open()) 
	{
		logFile.close(); 
	}
}


void PortBox::Instance()
{
	//��Ʈ Ž��

	


	ImGui::Begin(BoxName.c_str());
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(250, 130));
	ImGui::PushItemWidth(50);






	if (ImGui::Button("Show Available Ports")) {
		Selections = -1; //������ �ʱ�ȭ
		ComboClick = true;
		//��Ʈ Ž��
		PortName.clear();
		PortInfo = serial::list_ports();
		for (auto i = 0; i < PortInfo.size(); ++i)
		{
			PortName.push_back(PortInfo[i].port.c_str());
		}
		isListVisible = !isListVisible;
	}

	if (isListVisible) 
	{
		
		if (ImGui::CollapsingHeader("Available Ports", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (int i = 0; i < PortName.size(); ++i)
			{
				if (ImGui::Selectable(PortName[i], Selections == i))
				{
					Selections = i;
				}
			}
		}
	}


	if (Selections >= 0)
	{
		IsLost = false; //���õǾ����� Lost�� �ȶ߰�
		String = PortName[Selections];
		if (ComboClick)
		{
			LogFileName = String + "Log.txt";
			isListVisible = false;
			ComboClick = false;
		}
	}
	if (!isListVisible && !String.empty())
	{
		ImGui::Text("%s", String);
	}


	bool isConnectClicked = ImGui::Button("Connect");
	if (isConnectClicked)
	{
		if (!my_serial.isOpen())
		{
			//�α����� ����
			logFile.open(LogFileName, std::ios::app);
			PortBoxBool = true;
			my_serial.setPort(String);
			my_serial.setBaudrate(921600);
			my_serial.setTimeout(timeout);
			PortCheck();
		}
	}
	ImGui::SameLine();
	bool isDisconnectClicked = ImGui::Button("DisConnect");
	if (isDisconnectClicked)
	{
		PortBoxBool = false;
		String.clear();
		my_serial.close();
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
	//baudrate����� ����
	try
	{
		try {
			Dataline = my_serial.readline();
		}
		catch (const std::exception& e) {
			IsLost = true;
			logFile << String + "�� �ø��� ����� ������ϴ�. " << MyTime::Time->GetLocalTime() << std::endl;
			std::cout << String + "�� �ø��� ����� ������ϴ�. " << MyTime::Time->GetLocalTime() << std::endl;
			PortBoxBool = false;
			Selections = -1; //������ �ʱ�ȭ
			String.clear();
			my_serial.close();
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
			logFile << String + "�� �����Ͱ� ���ŵ��� �ʾҽ��ϴ�. " << MyTime::Time->GetLocalTime() << std::endl;
			std::cout << String + "�� �����Ͱ� ���ŵ��� �ʾҽ��ϴ�. " << MyTime::Time->GetLocalTime() << std::endl;
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
	}
	catch (const std::exception e)
	{
		MsgBox::Msg->ShowWarningMessageBox("������ �� ���� ��Ʈ �Դϴ�.");
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