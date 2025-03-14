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
	//메모리 지우기
    EraseMemory(_PortNum);
	//현재 작업중인 Directory경로 반환
	exePath = std::filesystem::current_path().wstring();
	// esp는 4개의 주소의 4개의 파일을 써야하니까 그만큼 반복문
	for (size_t i = 0; i < MemoryAddress.size(); ++i)
	{
		//전달받은 파일 이름 추출
		size_t pos = _FileName[i].find_last_of("\\");
		std::string filenames = (pos == std::string::npos) ? _FileName[i] : _FileName[i].substr(pos + 1);

		filePath = (std::filesystem::path(exePath) / filenames).wstring();
		//파일경로가 유효한지 확인
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

		//커맨드로 입력할 명령어 입력하는 부분
		std::wstringstream wss;
		wss << L"esptool --chip esp32 --port " << std::wstring(_PortNum.begin(), _PortNum.end())
			<< L" --baud 921600 write_flash " << std::wstring(MemoryAddress[i].begin(), MemoryAddress[i].end())
			<< L" " << filePath;
		writeCommand = wss.str();

		//shell pipe열어서 명령어 전송
		FILE* pipe = my_popen(writeCommand, L"r");
		if (pipe == nullptr)
		{
			TriggerEvent("Shell Pipe in unavailable");
			closeAllPipes();
			MyGUI_Interface::GUI->SetUIAble();
			return;
		}

		//유효하다면 버퍼에서 전송받는 데이터를 받아서 PortBox에 callback으로 데이터를 트리거 전송
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