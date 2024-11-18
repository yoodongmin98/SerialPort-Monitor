#pragma once
#include <iostream>


class SerialMonitor;
class PortBox
{
public:
	PortBox();
	~PortBox();
	void Instance();
protected:
private:
	std::shared_ptr<SerialMonitor> Monitors = nullptr;
};
