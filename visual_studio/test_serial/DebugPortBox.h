#pragma once
#include <iostream>
#include <imgui.h>
#include <deque>



class DebugPortBox
{
public:
	DebugPortBox();
	~DebugPortBox();


	void Instance(std::string _PortName, std::deque<std::string>& _RawDataLog);
protected:
	void ScrollSetting();
private:
	bool isSizeInitialized = true;
};