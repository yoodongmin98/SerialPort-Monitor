#pragma once
#include <windows.h>
#include <string>

class MsgBox
{
public:
	static MsgBox* Msg;
	MsgBox();
	~MsgBox();
	void ShowWarningMessageBox(const std::string& message);
protected:
private:
};