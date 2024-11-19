#pragma once
#include <iostream>
#include <string>




class PortBox
{
public:
	
	PortBox();
	PortBox(int _X,int _Y, std::string _Name);
	~PortBox();
	void Instance();
	void SerialMonitor(const std::string _ComPort);
protected:

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
};
