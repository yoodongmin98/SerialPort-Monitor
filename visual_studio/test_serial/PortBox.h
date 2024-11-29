#pragma once
#include <iostream>
#include <string>
#include <serial/serial.h>
#include <chrono>
#include <fstream>


class PortBox
{
public:
	
	PortBox();
	PortBox(int _X,int _Y, std::string _Name);
	~PortBox();
	void Instance(std::string& _PortName);
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
private:
	
	std::vector<const char*> PortName;
	serial::Timeout timeout = serial::Timeout::simpleTimeout(100);

	bool BootStart = false;
	bool PortBoxBool = false;
	bool IsLost = false; //통신 끊겼을때 판별
	bool MissingBool = true; //시간 1프레임만 적용시킬때 쓸 변수
	bool BootingBool = true; //시간 1프레임만 적용시킬때 쓸 변수

	int DotCount = 1; //Working ...뜨는거
	
	std::string String; //Combo누르면 포트번호가 담길 string
	std::string Dataline; //Serial Data를 읽을 string
	std::string Dots = "."; //Working ...그거 ㅇㅇ


	std::chrono::steady_clock::time_point MissingTime;
	std::chrono::steady_clock::time_point currentMissingTime;
	std::chrono::steady_clock::time_point BootingTime;
	std::chrono::steady_clock::time_point currentBootingTime;
	//initializer
	int X = 0;
	int Y = 0;
	std::string BoxName = "";
	serial::Serial my_serial;



	//File
	std::ofstream logFile;
	bool LogFileBool = true;
};
