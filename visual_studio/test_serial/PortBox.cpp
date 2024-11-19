#include "PortBox.h"
#include "imgui.h"
#include <string>
#include "MessageBox.h"
#include <windows.h>
#include <iostream>
#include "MyTime.h"
#include <Windows.h>



ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 빨강
ImVec4 yellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // 노랑
ImVec4 Colors = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // 하늘색



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
	//포트 탐색

	


	ImGui::Begin(BoxName.c_str());
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(250, 130));
	ImGui::PushItemWidth(50);






	if (ImGui::Button("Show Available Ports")) {
		Selections = -1; //누르면 초기화
		ComboClick = true;
		//포트 탐색
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
		IsLost = false; //선택되었으면 Lost는 안뜨게
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
			//로그파일 생성
			logFile.open(LogFileName, std::ios::app);
			PortBoxBool = true;
			IsFirst = true;
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
	try 
	{
		try 
		{
			if (IsFirst) 
			{
				while (true) 
				{
					Dataline = my_serial.readline();
					if (!Dataline.empty()) 
					{
						IsFirst = false;
						break;
					}
					Sleep(300);
				}
			}
			else {
				Dataline = my_serial.readline();
				if(Dataline =="START" || Dataline == "REBOOT")
					logFile << "\r" << String + "를 재시작합니다 " << MyTime::Time->GetLocalTime() << std::flush;
			}
		}
		catch (const std::exception& e) {
			IsLost = true;
			IsFirst = true;
			logFile << "\r [" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "]" << String + " 의 시리얼 통신이 끊겼습니다. " <<  std::flush;
			std::cout << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "]" << String + " 의 시리얼 통신이 끊겼습니다. " << std::endl;
			PortBoxBool = false;
			Selections = -1; //누르면 초기화
			String.clear();
			logFile.close();
			my_serial.close();
			return;
		}
		//한 20번정도 검수해야징
		if (Dataline.empty())
		{
			for (auto i = 0; i < 20; ++i)
			{
				Dataline = my_serial.readline();
				if (!Dataline.empty())
					break;
			}
			
		}

		// 데이터가 비어있지 않을 때
		if (!Dataline.empty() && Dataline[0]=='#')
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
			logFile << "\r" << String + "을 부팅중입니다 " << MyTime::Time->GetLocalTime() << std::flush;
			std::cout << String + "을 부팅중입니다 " << MyTime::Time->GetLocalTime() << std::endl;
		}
		else
		{
				// 데이터가 비었을 때
				ImGui::TextColored(yellowColor, "Missing");
				logFile << "\r [" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << String + "의 데이터가 수신되지 않았습니다. " << std::flush;
				std::cout << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << String + "의 데이터가 수신되지 않았습니다. " << std::endl;
		}
	}
	catch (const serial::IOException& e) {
		MsgBox::Msg->ShowWarningMessageBox("올바른 포트번호가 입력되지 않았습니다.");
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
		MsgBox::Msg->ShowWarningMessageBox("연결할 수 없는 포트 입니다.");
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