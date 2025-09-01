#pragma once
#include "CLI_Window.h"
#include "imgui.h"
#include "DataFile.h"
#include "MyGUI_Interface.h"
#include "ExportCLI_Record.h"
#include "CsvLogger.h"
#include <serial/serial.h>


CLI_Window::CLI_Window()
{
	CLI_Record = std::make_shared<ExportCLI_Record>();
	CsvLoggers = std::make_shared <CsvLogger>("cli.csv");
	CLIText.resize(4);
}


CLI_Window::~CLI_Window()
{
}


void CLI_Window::Instance(const float _X, const float _Y)
{
	WindowSizeX = _X;
	WindowSizeY = _Y;

	PortName = serial::list_ports();
	FullPortName.clear();
	for (serial::PortInfo& V : PortName)
	{
		if (V.description.find(target) != std::string::npos)
			FullPortName.push_back(std::make_pair(V.description.c_str(),V.port));

	}

	MyWindow().Pos(ImVec2{ 0,0 }).Size(ImVec2{ WindowSizeX ,WindowSizeY });
	MyStyle K;
	K.PushColor(ImGuiCol_WindowBg, Im4SteelBlue)
		.PushColor(ImGuiCol_Border, Im4White)
		.PushColor(ImGuiCol_Separator, Im4White);
		

	ImGuiBegin A("##Begins");


	PortUI();

	ImGui::SameLine();

	{
		ImGuiBeginChild PortListSet("##BeginChilds2", ImVec2{ WindowSizeX * 0.5f , WindowSizeY * 0.7f }, true);
		static char ExportCLIbuffer[20] = "";
		static char ExportCLIbuffer1[20] = "";
		static char ExportCLIbuffer2[20] = "";
		static char ExportCLIbuffer3[20] = "";
		ImGui::InputTextMultiline("Export CLI_1", ExportCLIbuffer, sizeof(ExportCLIbuffer), ImVec2(150, 20), ImGuiInputTextFlags_None | ImGuiInputTextFlags_EscapeClearsAll);
		CLIText[0] = ExportCLIbuffer;
		ImGui::InputTextMultiline("Export CLI_2", ExportCLIbuffer1, sizeof(ExportCLIbuffer1), ImVec2(150, 20), ImGuiInputTextFlags_None | ImGuiInputTextFlags_EscapeClearsAll);
		CLIText[1] = ExportCLIbuffer1;

		if(ImGui::Button("AllExport"))
		{
			bool any = false;
			for (auto& PN : FullPortName)
			{
				for (auto i = 0; i < CLIText.size(); ++i)
				{
					if (!CLIText[i].empty())
					{
						resultByPort[PN.second] = CLI_Record->Export(PN.second, 921600, CLIText[i]);
						CsvLoggers->set(CLIText[i], resultByPort[PN.second]);
						any = true;
					}
				}
				if (any)
					CsvLoggers->commit();
			}
		}
	}
}



void CLI_Window::PortUI()
{
	ImGui::SeparatorText("Export CLI MODE");

	ImGuiBeginChild PortList("##BeginChilds1", ImVec2{ WindowSizeX * 0.4f , WindowSizeY * 0.7f }, true);

	ImGui::SeparatorText("PortList");

	for (auto& PN : FullPortName)
	{
		ImGui::TextColored(Im4White, PN.first.c_str());
		ImGui::SameLine();
		ImGui::PushID(PN.second.c_str());
		if (ImGui::Button("Export", ImVec2{ 50,20 }))
		{
			bool any = false;
			for (auto i=0; i< CLIText.size(); ++i)
			{
				if (!CLIText[i].empty())
				{
					resultByPort[PN.second] = CLI_Record->Export(PN.second, 921600, CLIText[i]);
					CsvLoggers->set(CLIText[i], resultByPort[PN.second]);
					any = true;
				}
			}

			if(any)
				CsvLoggers->commit();
		}
		ImGui::PopID();

		if (auto it = resultByPort.find(PN.second); it != resultByPort.end()) 
		{
			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x);
			ImGui::TextUnformatted(it->second.c_str());
			ImGui::PopTextWrapPos();
		}

		ImGui::Separator();
	}
}