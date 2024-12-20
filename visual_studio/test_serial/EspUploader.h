#pragma once

#include <string>
#include <vector>
#include <string>


class EspUploader
{
public:
	EspUploader();
	~EspUploader();


	void Instance(std::string& _PortNum, std::vector<std::string>& _FileName);
	void PrintLog();
protected:
private:
	bool scrollToBottom = true;
	std::vector<std::string> FlashLog;
	std::vector<std::string> MemoryAddress = { "0x1000" , "0x8000" , "0xe000" , "0x10000" };
};