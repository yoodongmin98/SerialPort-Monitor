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
	void Instance();
	void Connect();
	void DisConnect();
protected:
	void PortCheck();
	void SerialMonitor();
private:
	std::vector<serial::PortInfo> PortInfo;
	std::vector<const char*> PortName;
	serial::Timeout timeout = serial::Timeout::simpleTimeout(100);

	bool PortBoxBool = false;
	bool isListVisible = false; //Combo보이는거/안보이는거
	bool IsLost = false; //통신 끊겼을때 판별
	bool initialized = false; // 창이 초기화되었는지 확인하는 플래그
	bool ComboClick = false; //Combo창이 눌렸음을 판단하는것
	bool IsFirst = true;

	int Selections = -1; //Combo선택 index
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
