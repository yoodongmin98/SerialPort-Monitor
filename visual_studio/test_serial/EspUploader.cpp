#include "EspUploader.h"
#include "imgui.h"
#include "MyGUI_Interface.h"

#include <array>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib> 


EspUploader::EspUploader()
{

}

EspUploader::~EspUploader()
{

}


void EspUploader::Instance(std::string& _PortNum, std::vector<std::string>& _FileName)
{ 
	//나중에 뮤텍스로 잠궈야할수도있음(서버에서 경로지정받아서 다운받게되면)
	//싫으면 그냥 경로를 지역변수로 만들면됨 ㅇㅇ
    EraseMemory(_PortNum);
	exePath = std::filesystem::current_path().wstring();
	for (size_t i = 0; i < MemoryAddress.size(); ++i)
	{
		filePath = (std::filesystem::path(exePath) / _FileName[i]).wstring();
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

		std::wstringstream wss;
		wss << L"esptool --chip esp32 --port " << std::wstring(_PortNum.begin(), _PortNum.end())
			<< L" --baud 921600 write_flash " << std::wstring(MemoryAddress[i].begin(), MemoryAddress[i].end())
			<< L" " << filePath;
		writeCommand = wss.str();

		FILE* pipe = my_popen(writeCommand, L"r");
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



void EspUploader::EraseMemory(std::string _PortNum)
{
	std::wstring wPortNum(_PortNum.size(), L'\0');
	std::mbstowcs(&wPortNum[0], _PortNum.c_str(), _PortNum.size());
	wPortNum.resize(std::wcslen(wPortNum.c_str())); // Null-terminator 제거

    std::unique_lock<std::mutex>(EspMutex);
    std::array<char, 128> Erasebuffer;
	std::wstringstream wss;
	wss << L"esptool --port " << wPortNum << L" erase_flash";
	std::wstring systemcmd = wss.str();
	FILE* pipes = my_popen(systemcmd.c_str(), L"r");
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


FILE* EspUploader::my_popen(std::wstring command, const wchar_t* mode) 
{
	FILE* pipe = _wpopen(command.c_str(), mode);
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