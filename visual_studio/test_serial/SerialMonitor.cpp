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
	//baudrate����� ����
	try
	{
		serial::Serial my_serial(_ComPort, 921600, serial::Timeout::simpleTimeout(1000));
		std::string Dataline;

		try {
			Dataline = my_serial.readline();
		}
		catch (const std::exception& e) {
			std::cout << _ComPort + "�� �ø��� ����� ������ϴ�. " <<std::endl;
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
			std::cout << _ComPort + "�� �����Ͱ� ���ŵ��� �ʾҽ��ϴ�. " << std::endl;
		}
	}
	catch (const serial::IOException& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("�ùٸ� ��Ʈ��ȣ�� �Էµ��� �ʾҽ��ϴ�.");
		PortBox::Box->SetBoxBoolFalse();
	}
	catch (const std::exception& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("�� �� ���� ������ �߻��߽��ϴ�.");
		PortBox::Box->SetBoxBoolFalse();
	}
	catch (...)
	{
		MsgBox::Msg->ShowWarningMessageBox("�� �� ���� ������ �߻��߽��ϴ�.");
		PortBox::Box->SetBoxBoolFalse();
	}

}