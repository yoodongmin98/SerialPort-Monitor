#pragma once
#include <iostream>
#include <string>
#include <serial/serial.h>



class PortBox
{
public:
	
	PortBox();
	PortBox(int _X,int _Y, std::string _Name);
	~PortBox();
	void Instance();
	void SerialMonitor(const std::string _ComPort);
protected:
	void PortCheck();
private:
	bool PortBoxBool = false;
	char inputString[64] = "";
	int Number = 0;
	std::string String;
	std::string Dataline;
	std::string Dots = ".";
	int DotCount = 1;

	bool IsLost = false;


	//initializer
	int X = 0;
	int Y = 0;
	std::string BoxName = "";
	serial::Serial my_serial;
};
