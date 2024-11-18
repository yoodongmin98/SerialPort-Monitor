#include "MyTime.h"
#include <iostream>
#include <ctime>
#include <chrono>
#include <string>



MyTime* MyTime::Time = nullptr;

MyTime::MyTime()
{
	Time = this;
}

MyTime::~MyTime()
{

}

std::string MyTime::GetLocalTime()
{
	now2 = _time64(nullptr);
	localtime_s(&tm_2, &now2);
	std::string LocalTime = std::to_string(tm_2.tm_hour) + ":" + std::to_string(tm_2.tm_min) + ":" + std::to_string(tm_2.tm_sec);
	return LocalTime;
}


std::string MyTime::GetLocalDay()
{
	now2 = _time64(nullptr);
	localtime_s(&tm_2, &now2);
	std::string LocalDay = std::to_string((tm_2.tm_year + 1900)) + "³â" + std::to_string((tm_2.tm_mon + 1)) + "¿ù" + std::to_string((tm_2.tm_mday)) + "ÀÏ";
	return LocalDay;
}