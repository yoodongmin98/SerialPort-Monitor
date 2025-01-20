#pragma once
#include <iostream>
#include <imgui.h>
#include <deque>
#include <functional>




class DebugPortBox
{
public:
	DebugPortBox();
	~DebugPortBox();

	std::function<void()>& GetXEvent()
	{
		return XEvent;
	}
	void Instance(std::string _PortName, std::deque<std::string>& _RawDataLog);
protected:
	void ScrollSetting();
	void Trigger_X_Event()
	{
		if (XEvent)
			XEvent();
	}
private:
	bool isSizeInitialized = true;
	std::function<void()> XEvent;
};