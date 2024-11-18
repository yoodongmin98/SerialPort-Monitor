#pragma once
#include <iostream>
#include <string>




class MyTime
{
public:
	MyTime();
	~MyTime();
	static MyTime* Time;
	std::string GetLocalTime();
	std::string GetLocalDay();
protected:
private:
	__time64_t now2;
	tm tm_2;
};
