#pragma once
#include <string>

enum class SerialState
{
	Working,
	ReBooting,
	Missing,
	DisConnected
};
class SerialMonitor
{
public:
	SerialMonitor();
	~SerialMonitor();

	void Instance(const std::string _ComPort);
protected:
private:
	
};
