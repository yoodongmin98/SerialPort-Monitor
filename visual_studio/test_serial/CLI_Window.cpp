#pragma once
#include "CLI_Window.h"
#include "imgui.h"
#include "DataFile.h"

CLI_Window::CLI_Window()
{

}


CLI_Window::~CLI_Window()
{
}


void CLI_Window::Instance(const float _X , const float _Y)
{
	WindowSizeX = _X;
	WindowSizeY = _Y;


	MyWindow().Pos(ImVec2{ 0,0 }).Size(ImVec2{ WindowSizeX ,WindowSizeY });
	MyStyle K;
	K.PushColor(ImGuiCol_WindowBg, Im4SteelBlue),
	K.PushColor(ImGuiCol_Border, Im4White);
	
	ImGuiBegin A("##Begins");

	ImGui::SeparatorText("Export CLI MODE");
	ImGuiBeginChild Childs("##BeginChilds", ImVec2{ WindowSizeX * 0.7f , WindowSizeY * 0.7f }, true);
}