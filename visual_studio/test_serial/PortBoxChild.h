#pragma once
#include <iostream>
#include "imgui.h"
#include <string>




class PortBoxChild
{
public:
	PortBoxChild();
	~PortBoxChild();
	void CreateChild();
	void SendRowData(std::string _Data);
protected:
private:
	
	ImVec4 ButtonColor = ImVec4{ 0.5f,0.5f,0.5f,1.0f };
	std::string RowData;
};