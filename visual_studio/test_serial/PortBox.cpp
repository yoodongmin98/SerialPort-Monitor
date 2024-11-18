#include "PortBox.h"
#include "imgui.h"
#include <string>
#include "MessageBox.h"
#include "SerialMonitor.h"
#include <windows.h>
#include <iostream>

void ShowWarningMessageBox(const std::string& message) {
	MessageBoxA(NULL, message.c_str(), "Warning", MB_ICONWARNING | MB_OK);
}






PortBox::PortBox()
{
	Monitors = std::make_shared<SerialMonitor>();
}
PortBox::~PortBox()
{

}
char inputString[64] = "";
void PortBox::Instance() //��ġ������ ���ڷ� �޾Ƽ� ��ġ �����ؾ���
{
	ImGui::Begin("PortBox1");
	ImGui::Text("Set Port Number");
	ImGui::PushItemWidth(50);
	ImGui::InputText("Input Comport Number", inputString, IM_ARRAYSIZE(inputString));
	if (ImGui::Button("Button"))
	{
		try
		{
			std::string String(inputString);
			memset(inputString, 0, sizeof(inputString));
			int Number = stoi(String);
			if (Number <= 0 || Number > 255)
			{
				MsgBox::Msg->ShowWarningMessageBox("1���� 255������ ���ڸ� �Է����ּ���");
			}
			else
			{
				String = "COM" + String;
				Monitors->Instance(String);
			}
		}
		catch (const std::invalid_argument& e)
		{
			MsgBox::Msg->ShowWarningMessageBox("���� ������ �߸��Ǿ����ϴ�.");
		}
		catch (const std::out_of_range& e)
		{
			MsgBox::Msg->ShowWarningMessageBox("�Է��� ���ڰ� �ʹ� Ů�ϴ�.");
		}
	}
	ImGui::End();
}