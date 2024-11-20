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
	bool isListVisible = false; //Combo���̴°�/�Ⱥ��̴°�
	bool IsLost = false; //��� �������� �Ǻ�
	bool initialized = false; // â�� �ʱ�ȭ�Ǿ����� Ȯ���ϴ� �÷���
	bool ComboClick = false; //Comboâ�� �������� �Ǵ��ϴ°�
	bool IsFirst = true;

	int Selections = -1; //Combo���� index
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
