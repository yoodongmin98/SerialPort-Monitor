#pragma once
#include <chrono>
#include <deque>
#include <fstream>
#include <iostream>
#include <mutex>
#include <serial/serial.h>
#include <string>

#include "imgui.h"


class EspUploader;
class MyGUI_Interface;
class PortBox
{
public:
	
	PortBox();
	PortBox(int _X,int _Y, std::string _Name);
	~PortBox();

	void Instance(std::string& _PortName);
	void Connect();
	void DisConnect();
	void InputCLI(std::string& _CLI);
	void StartESPFlash();
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
	void SetCOMLogFile()
	{
		LogFileBool = !LogFileBool;
	}
	void SetCOMLofFileClose()
	{
		if (logFile.is_open())
			logFile.close();
	}
	void SetNoDataTime(int _Time)
	{
		NoDataTime = _Time;
	}
protected:
	void GUISetting();
	void DataSet();
	void CreatePortButton(std::string& _PortName);
	void InsertTask_WorkingCheck(std::string& _PortName);
	void PortCheck();

	void SerialMonitor();
	void CloseSerialPort();
	void CreateRowDataBox();
	void ASCII_HEX_Setting();

private:
	bool BootStart = false;
	bool PortBoxBool = false;
	bool IsLost = false; //��� �������� �Ǻ�
	bool MissingBool = false; //�ð� 1�����Ӹ� �����ų�� �� ����
	bool WorkingBool = false;


	//MODE
	bool ASCIIMODE = true;
	bool HEXMODE = false;


	//Serial Data Setting
	int BaudRate = 921600; //Defalut
	int Databit = 8; //Default
	bool Stopbit = true; //Default

	int WinSize = 1200; //Default
	int PreHexCount = 0;
	int HexNumberCount = 0;
	int DotCount = 1; //Working ...�ߴ°�
	
	std::string String; //Combo������ ��Ʈ��ȣ�� ��� string
	std::string Dataline; //Serial Data�� ���� string
	std::stringstream hexStream; //Serial Data�� ���� hex
	std::string HexLineData; //hex�� ���ٷ� �̾���� string
	std::string Dots = "."; //Working ...�װ� ����
	std::string BootingString = "START"; //������ ������ string


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
	ImVec2 PortBoxSize = ImVec2{ 250,130 };


	//Serial
	serial::Serial my_serial;
	std::mutex serialMutex;
	std::mutex stateMutex;
	serial::Timeout timeout = serial::Timeout::simpleTimeout(10);
	std::vector<const char*> PortName;



	//File
	std::ofstream logFile;
	bool LogFileBool = false;
	int NoDataTime = 5;


	//Log
	std::deque<std::string> RawDataLog;
	std::vector<std::string> RawHexLog;
	bool scrollToBottom = false;




	//EspUploader
	std::shared_ptr<EspUploader> ESP = nullptr;
};
