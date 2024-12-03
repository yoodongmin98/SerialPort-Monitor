#pragma once
#include <iostream>
#include <string>
#include <serial/serial.h>
#include <chrono>
#include <fstream>
#include <mutex>
#include "imgui.h"



class PortBoxChild;
class PortBox
{
public:
	
	PortBox();
	PortBox(int _X,int _Y, std::string _Name);
	~PortBox();
	void Instance(std::string _PortName);
	void Connect();
	void DisConnect();

	bool IsStringNull()
	{
		return String.empty();
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
	bool MissingBool = true; //�ð� 1�����Ӹ� �����ų�� �� ����
	bool BootingBool = true; //�ð� 1�����Ӹ� �����ų�� �� ����
	bool RowDataBox = false;
	bool WorkingBool = false;
	std::mutex stateMutex;


	int DotCount = 1; //Working ...�ߴ°�
	
	std::string String; //Combo������ ��Ʈ��ȣ�� ��� string
	std::string Dataline; //Serial Data�� ���� string
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
	std::shared_ptr<PortBoxChild> ChildBox = nullptr;
	ImVec2 PortBoxSize = ImVec2{ 180,100 };

	//Serial
	serial::Serial my_serial;
	std::mutex serialMutex;
	serial::Timeout timeout = serial::Timeout::simpleTimeout(10);
	std::vector<const char*> PortName;



	//File
	std::ofstream logFile;
	bool LogFileBool = true;
};
