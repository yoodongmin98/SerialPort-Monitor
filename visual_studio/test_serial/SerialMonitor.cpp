#include "SerialMonitor.h"
#include "serial/serial.h"
#include <string>
#include <iostream>
#include "imgui.h"
#include "MessageBox.h"
#include "Portbox.h"

SerialMonitor::SerialMonitor()
{

}

SerialMonitor::~SerialMonitor()
{

}
std::string Dots = ".";
int DotCount = 1;

void SerialMonitor::Instance(const std::string _ComPort)
{
	//baudrate물어보기 ㅇㅇ
	try
	{
		serial::Serial my_serial(_ComPort, 921600, serial::Timeout::simpleTimeout(1000));
		std::string Dataline;

		try {
			Dataline = my_serial.readline();
		}
		catch (const std::exception& e) {
			std::cout << _ComPort + "의 시리얼 통신이 끊겼습니다. " <<std::endl;
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
			std::cout << _ComPort + "의 데이터가 수신되지 않았습니다. " << std::endl;
		}
	}
	catch (const serial::IOException& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("올바른 포트번호가 입력되지 않았습니다.");
		PortBox::Box->SetBoxBoolFalse();
	}
	catch (const std::exception& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("알 수 없는 오류가 발생했습니다.");
		PortBox::Box->SetBoxBoolFalse();
	}
	catch (...)
	{
		MsgBox::Msg->ShowWarningMessageBox("알 수 없는 오류가 발생했습니다.");
		PortBox::Box->SetBoxBoolFalse();
	}

}