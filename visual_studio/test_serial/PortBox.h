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
	bool IsLost = false; //��� �������� �Ǻ�
	bool IsFirst = true; //���� �Ǻ�
	bool LogFileSet = true; //�α����� �̸������Ұ�

	int DotCount = 1; //Working ...�ߴ°�
	
	std::string String; //Combo������ ��Ʈ��ȣ�� ��� string
	std::string Dataline; //Serial Data�� ���� string
	std::string Dots = "."; //Working ...�װ� ����
	std::ofstream logFile;
	std::string LogFileName; //�α����Ϸ� ���� �̸�

	

	//initializer
	int X = 0;
	int Y = 0;
	std::string BoxName = "";
	serial::Serial my_serial;
};
