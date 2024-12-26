#pragma once

#include <string>
#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <filesystem>
#include <array>



class PortBox;
class EspUploader
{
public:
	EspUploader();
	~EspUploader();

	void TriggerEvent(const std::string& _Log)
	{
		if (Event)
			Event(_Log);
	}

	std::function<void(const std::string&)>& GetEspEvent()
	{
		return Event;
	}

	void Instance(std::string& _PortNum, std::vector<std::string>& _FileName);
protected:
	void EraseMemory(std::string& _PortNum);
private:
	bool scrollToBottom = true;

	bool findlastaddress = false;
	std::vector<std::string> MemoryAddress = { "0x1000" , "0x8000" , "0xe000" , "0x10000" };

	std::function<void(const std::string&)> Event;
	std::mutex EspMutex;

	std::filesystem::path exePath;
	std::filesystem::path filePath;
	std::string writeCommand;
	std::array<char, 128> buffer;
};