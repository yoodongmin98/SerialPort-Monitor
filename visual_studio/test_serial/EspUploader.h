#pragma once

#include <string>
#include <vector>
#include <string>
#include <functional>
#include <mutex>



class PortBox;
class EspUploader
{
	friend PortBox;
public:
	EspUploader();
	~EspUploader();

	void TriggerEvent(const std::string& _Log)
	{
		if (Event)
			Event(_Log);
	}
	
	void Instance(std::string& _PortNum, std::vector<std::string>& _FileName);
protected:
private:
	bool scrollToBottom = true;

	std::vector<std::string> MemoryAddress = { "0x1000" , "0x8000" , "0xe000" , "0x10000" };

	std::function<void(const std::string&)> Event;
	std::mutex EspMutex;
};