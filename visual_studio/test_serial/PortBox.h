#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <serial/serial.h>



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
	void SerialMonitor();
private:
	
	std::vector<const char*> PortName;
	serial::Timeout timeout = serial::Timeout::simpleTimeout(10);

	bool PortBoxBool = false;
	bool IsLost = false; //통신 끊겼을때 판별
	bool IsFirst = true; //부팅 판별
	bool LogFileSet = true; //로그파일 이름설정할것

	int DotCount = 1; //Working ...뜨는거
	
	std::string String; //Combo누르면 포트번호가 담길 string
	std::string Dataline; //Serial Data를 읽을 string
	std::string Dots = "."; //Working ...그거 ㅇㅇ
	std::ofstream logFile;
	std::string LogFileName; //로그파일로 남길 이름

	

	//initializer
	int X = 0;
	int Y = 0;
	std::string BoxName = "";
	serial::Serial my_serial;
};
