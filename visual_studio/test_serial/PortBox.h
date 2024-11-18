#pragma once
#include <iostream>




class SerialMonitor;
class PortBox
{
public:
	static PortBox* Box;
	PortBox();
	~PortBox();
	void Instance();
	void SetBoxBoolFalse()
	{
		PortBoxBool = false;
	}
protected:
private:
	bool PortBoxBool = false;
	char inputString[64] = "";
	int Number = 0;
	std::string String;
	std::shared_ptr<SerialMonitor> Monitors = nullptr;
};
