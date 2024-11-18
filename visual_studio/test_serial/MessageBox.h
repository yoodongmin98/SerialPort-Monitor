#pragma once
#include <windows.h>


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