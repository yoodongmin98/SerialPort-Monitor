#include "EspUploader.h"
#include "imgui.h"
#include "MyGUI_Interface.h"

#include <array>
#include <filesystem>
#include <iostream>



EspUploader::EspUploader()
{

}

EspUploader::~EspUploader()
{

}


void EspUploader::Instance(std::string& _PortNum, std::vector<std::string>& _FileName)
{ 
    EraseMemory(_PortNum);
	exePath = std::filesystem::current_path();
	for (size_t i = 0; i < MemoryAddress.size(); ++i)
	{
		filePath = exePath / _FileName[i];
		if (std::filesystem::exists(filePath)) 
		{
			TriggerEvent("The file exists");
		}
		else
		{
			TriggerEvent("The file does not exist");
			MyGUI_Interface::GUI->SetUIAble();
			return;
		}

		writeCommand = "esptool --chip esp32 --port " + _PortNum + " --baud 921600 write_flash " + MemoryAddress[i] + " " + filePath.string();

		FILE* pipe = my_popen(writeCommand.c_str(), "r");
		if (pipe == nullptr)
		{
			TriggerEvent("Shell Pipe in unavailable");
			closeAllPipes();
			MyGUI_Interface::GUI->SetUIAble();
			return;
		}

		while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
		{
			std::string Strings = buffer.data();
			TriggerEvent(Strings);
			if (Strings.find("Hash of data") != std::string::npos && i == 3)
			{
				TriggerEvent("Flash Complete");
				MyGUI_Interface::GUI->SetUIAble();
				my_pclose(pipe);
				return;
			}
		}
		my_pclose(pipe);
	}
	TriggerEvent("The port is unavailable.");
	closeAllPipes();
	MyGUI_Interface::GUI->SetUIAble();
	return;
}



void EspUploader::EraseMemory(std::string& _PortNum)
{
    std::unique_lock<std::mutex>(EspMutex);
    std::array<char, 128> Erasebuffer;
    std::string systemcmd = "esptool --port " + _PortNum + " erase_flash";
	FILE* pipes = my_popen(systemcmd.c_str(), "r");
	if (pipes == nullptr)
	{
		TriggerEvent("Shell Pipe in unavailable");
		TriggerEvent("Erase Fail");
		closeAllPipes();
		MyGUI_Interface::GUI->SetUIAble();
		return;
	}
    while (fgets(Erasebuffer.data(), Erasebuffer.size(), pipes) != nullptr)
    {
        TriggerEvent(Erasebuffer.data());
    }
	my_pclose(pipes);
}


FILE* EspUploader::my_popen(const char* command, const char* mode) 
{
	FILE* pipe = _popen(command, mode);
	if (pipe != nullptr) 
	{
		openPipes.push_back(pipe);
	}
	return pipe;
}


void EspUploader::my_pclose(FILE* pipe)
{
	if (pipe != nullptr) 
	{
		_pclose(pipe);
		openPipes.erase(std::remove(openPipes.begin(), openPipes.end(), pipe), openPipes.end());
	}
}


void EspUploader::closeAllPipes()
{
	for (FILE* pipe : openPipes) 
	{
		_pclose(pipe);
	}
	openPipes.clear();
}