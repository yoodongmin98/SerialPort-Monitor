#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <serial/serial.h>
#include <chrono>



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
	serial::Timeout timeout = serial::Timeout::simpleTimeout(500);

	bool PortBoxBool = false;
	bool IsLost = false; //��� �������� �Ǻ�
	bool IsFirst = true; //���� �Ǻ�
	bool LogFileSet = true; //�α����� �̸������Ұ�
	bool MissingBool; //�ð� 1�����Ӹ� �����ų�� �� ����
	bool BootingBool; //�ð� 1�����Ӹ� �����ų�� �� ����

	int DotCount = 1; //Working ...�ߴ°�
	
	std::string String; //Combo������ ��Ʈ��ȣ�� ��� string
	std::string Dataline; //Serial Data�� ���� string
	std::string Dots = "."; //Working ...�װ� ����
	std::ofstream logFile;
	std::string LogFileName; //�α����Ϸ� ���� �̸�

	std::chrono::steady_clock::time_point MissingTime;
	std::chrono::steady_clock::time_point currentMissingTime;
	std::chrono::steady_clock::time_point BootingTime;
	std::chrono::steady_clock::time_point currentBootingTime;
	//initializer
	int X = 0;
	int Y = 0;
	std::string BoxName = "";
	serial::Serial my_serial;
};
