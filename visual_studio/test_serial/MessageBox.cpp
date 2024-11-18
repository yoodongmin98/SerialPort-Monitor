#include <windows.h>
#include <string>
#include "MessageBox.h"



MsgBox* MsgBox::Msg = nullptr;

MsgBox::MsgBox()
{
	Msg = this;
}
MsgBox::~MsgBox()
{

}


void MsgBox::ShowWarningMessageBox(const std::string& message) 
{
	MessageBoxA(NULL, message.c_str(), "Warning", MB_ICONWARNING | MB_OK);
}