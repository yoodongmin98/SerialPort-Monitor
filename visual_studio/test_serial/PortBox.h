#pragma once
#include <iostream>
#include <string>
#include <serial/serial.h>
#include <chrono>
#include <fstream>
#include <mutex>
#include "imgui.h"
#include <deque>



class MyGUI_Interface;
class PortBox
{
public:
	
	PortBox();
	PortBox(int _X,int _Y, std::string _Name);
	~PortBox();

	void Instance(std::string _PortName);
	void Connect();
	void DisConnect();
	void InputCLI(std::string& _CLI);
	bool IsStringNull()
	{
		return String.empty();
	}
	void RawMonitorClear();
	void SetASCIIMODE()
	{
		ASCIIMODE = true;
		HEXMODE = false;
	}
	void SetHEXMODE()
	{
		ASCIIMODE = false;
		HEXMODE = true;
	}
	void SetHexPrintNumberCount(int _Count)
	{
		HexNumberCount = _Count;
	}
protected:
	void PortCheck();
	void CreatePortLogFile();
	void SerialMonitor();
	void CloseSerialPort();
	void CreateRowDataBox();
	
private:
	bool BootStart = false;
	bool PortBoxBool = false;
	bool IsLost = false; //��� �������� �Ǻ�
	bool MissingBool = false; //�ð� 1�����Ӹ� �����ų�� �� ����
	bool WorkingBool = false;

	bool ASCIIMODE = true;
	bool HEXMODE = false;
	std::mutex stateMutex;

	int PreHexCount = 0;
	int HexNumberCount = 0;
	int DotCount = 1; //Working ...�ߴ°�
	
	std::string String; //Combo������ ��Ʈ��ȣ�� ��� string
	std::string Dataline; //Serial Data�� ���� string
	std::stringstream hexStream; //Serial Data�� ���� hex
	std::string HexLineData; //hex�� ���ٷ� �̾���� string
	std::string Dots = "."; //Working ...�װ� ����


	//Time
	std::chrono::steady_clock::time_point MissingTime;
	std::chrono::steady_clock::time_point currentMissingTime;
	std::chrono::steady_clock::time_point BootingTime;
	std::chrono::steady_clock::time_point currentBootingTime;


	//initializer
	int X = 0;
	int Y = 0;
	std::string BoxName = "";

	//UI
	ImVec4 customColor = ImVec4(0.3f, 0.2f, 0.4f, 0.4f);
	ImVec2 PortBoxSize = ImVec2{ 250,130 };

	//Serial
	serial::Serial my_serial;
	std::mutex serialMutex;
	serial::Timeout timeout = serial::Timeout::simpleTimeout(10);
	std::vector<const char*> PortName;



	//File
	std::ofstream logFile;
	bool LogFileBool = true;



	//Log
	std::deque<std::string> RawDataLog;
	std::vector<std::string> RawHexLog;
	bool scrollToBottom = false;
};
