#include "DataFile.h"
#include "EspUploader.h"
#include "imgui.h"
#include "MessageBox.h"
#include "MyGUI_Interface.h"
#include "MyImGui.h"
#include "MyTime.h"
#include "PortBox.h"
#include "ThreadPool.h"
#include "DebugPortBox.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <windows.h>
#include <algorithm>







PortBox::PortBox()
{
	
}

PortBox::PortBox(int _X, int _Y, std::string _Name)
	: X(_X), Y(_Y), BoxName(_Name)
{
	ESP = std::make_shared<EspUploader>();
	D_Port = std::make_shared<DebugPortBox>();
	//ESP CallBack
	SetCallBackEvent();
	Set_X_Click_CallBackEvent();
}


PortBox::~PortBox()
{
	CloseSerialPort();
}



void PortBox::Instance(std::string& _PortName)
{
	GUISetting();
	DataSet();
	CreatePortButton(_PortName);
	InsertTask_WorkingCheck(_PortName);
	CreateRowDataBox();
}


void PortBox::GUISetting()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, PORTBOXCOLOR);
	ImGui::SetNextWindowPos(ImVec2(X, Y), ImGuiCond_Always);
	ImGui::Begin(BoxName.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar);
	PortBoxSize = MyGUI_Interface::GUI->GetcellSize();
	ImGui::SetWindowSize(PortBoxSize);
	ImGui::PushItemWidth(50);
}


void PortBox::DataSet()
{
	NoDataTime = MyGUI_Interface::GUI->GetSliderInt();
	BootingString = MyGUI_Interface::GUI->GetBootDetectionCharacter();
	BaudRate = MyGUI_Interface::GUI->GetDataSettingBaudrate();
	Databit = MyGUI_Interface::GUI->GetDataSettingDatabit();
	Stopbit = MyGUI_Interface::GUI->GetDataSettingStopbit();
	WinSize = MyImGui::MyImGuis->GetWindowSize_X();
}


void PortBox::CreatePortButton(std::string& _PortName)
{
	String = _PortName;
	ImGui::Text("%s", String.c_str());

	if (MyGUI_Interface::GUI->GetViewBool())
	{
		ImGui::SameLine();
		if (ImGui::Button("Conn"))
			Connect();

		ImGui::SameLine();

		if (ImGui::Button("Disc"))
			DisConnect();

		ImGui::SameLine();

		if (ImGui::Button("CLI"))
			InputCLI();
	}
	DebugPortSetting(_PortName);
	
	if (IsLost && !PortBoxBool)
		ImGui::TextColored(REDCOLOR, "Connection Lost");
}


void PortBox::InsertTask_WorkingCheck(std::string& _PortName)
{
	if (PortBoxBool)
	{
		if (LogFileBool)
		{
			if (!logFile.is_open())
			{
				std::string MyPATH = GetFileNameFromPath(SystemPATH);
				logFile.open(MyPATH + _PortName+".txt", std::ios::app);
			}
			LogFileBool = false;
		}

		if (!my_serial.isOpen() && PortBoxBool && IsLost)
			ImGui::TextColored(Im4PastelCoral, "Reconnect...");
		else if (WorkingBool)
			ImGui::Text("Working%s", Dots.c_str());
		else if (MissingBool)
			ImGui::TextColored(YELLOWCOLOR, "Missing");
		else if (BootStart)
			ImGui::TextColored(BLUECOLOR, "Booting");
		
	}
}


void PortBox::SerialMonitor()
{
	static auto nextAttempt = std::chrono::steady_clock::now();

	try
	{
		if (!my_serial.isOpen())
		{
			//포트가 닫혀 있으면 "수신 시도"는 건너뛰고 아래의 재연결 블록으로 진행.
		}
		else
		{
			bool dataReceived = false;

			// 데이터 읽기
			if (my_serial.available())
			{
				std::lock_guard<std::mutex> lock(stateMutex); // 상태 갱신 보호

				if (ASCIIMODE)
					Dataline = my_serial.readline(); 
				else if (HEXMODE)
					Dataline = my_serial.read();

				if (!Dataline.empty())
				{
					// 동작 표시(…)
					DotCount++;
					if (DotCount > 7) DotCount = 1;
					Dots.clear();
					for (int i = 0; i < DotCount; ++i) Dots += ".";
				}

				// Raw 로그 패널 데이터 적재
				if (ASCIIMODE)
				{
					if (RawDataLog.size() >= 10000) RawDataLog.pop_front();
					RawDataLog.push_back(Dataline);
					scrollToBottom = true;
				}
				else if (HEXMODE)
				{
					hexStream.str("");
					for (unsigned char c : Dataline)
						hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
					HexLineData += hexStream.str();
					PreHexCount++;

					if (RawHexLog.size() >= 10000) RawHexLog.pop_front();

					if ((PreHexCount - HexNumberCount) == 0)
					{
						HEXLOGRECORD = true;
						RawHexLog.push_back(HexLineData);
						PreHexCount = 0;
						HexLineData.clear();
						scrollToBottom = true;
					}
				}

				// 파일 로그 기록
				if (ASCIIMODE)
				{
					if (!Dataline.find("\n") || Dataline.empty())
						logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << Dataline << std::endl << std::flush;
					else
						logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << Dataline << std::flush;
				}
				else if (HEXMODE && HEXLOGRECORD)
				{
					// NOTE: HEX 줄 구분은 상위 설정(HexNumberCount)에 의존.
					logFile << "[" << MyTime::Time->GetLocalDay() << MyTime::Time->GetLocalTime() << "] " << RawHexLog.back() << std::endl << std::flush;
					HEXLOGRECORD = false;
				}

				// 상태 갱신(Working/Missing/Boot)
				if (!Dataline.empty())
				{
					dataReceived = true;

					if (Dataline.find(BootingString) != std::string::npos)
					{
						WorkingBool = false;
						MissingBool = false;
						BootStart = true;
						BootingTime = std::chrono::steady_clock::now();
					}
					else if (!BootStart) // 부팅 중이 아닐 때만 Working으로
					{
						WorkingBool = true;
						MissingBool = false;
						BootStart = false;
					}
				}
			}

			// 읽을 데이터가 없을 때 "미수신 상태" 타이머
			if (!my_serial.available() && !BootStart)
			{
				if (!MissingBool)
				{
					MissingTime = std::chrono::steady_clock::now();
					MissingBool = true;
				}
			}

			// 미수신 경과 체크
			if (MissingBool)
			{
				std::lock_guard<std::mutex> lock(stateMutex);
				currentMissingTime = std::chrono::steady_clock::now();
				if (std::chrono::duration_cast<std::chrono::seconds>(currentMissingTime - MissingTime).count() >= NoDataTime)
				{
					WorkingBool = false;
					MissingBool = false;
					BootStart = false;
					MyGUI_Interface::GUI->LogFlash(String, "의 데이터가 설정시간 이상 수신되지 않았습니다.");
					MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " No data received for " + std::to_string(NoDataTime) + " seconds");
				}
			}

			// 부팅 완료 경과 체크
			if (BootStart)
			{
				std::lock_guard<std::mutex> lock(stateMutex);
				currentBootingTime = std::chrono::steady_clock::now();
				if (std::chrono::duration_cast<std::chrono::seconds>(currentBootingTime - BootingTime).count() >= 5)
				{
					WorkingBool = false;
					MissingBool = false;
					BootStart = false;
					MyGUI_Interface::GUI->LogFlash(String, "이 부팅되었습니다.");
					MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " Boot completed");
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		// 끊김/예외 시 상태 플래그 갱신 + 포트만 닫고 포트명/의도(PortBoxBool)는 유지 → 자동 재연결 가능
		{
			std::lock_guard<std::mutex> lock(stateMutex);
			IsLost = true;
			MyGUI_Interface::GUI->LogFlash(String, "의 시리얼 통신이 끊겼습니다.");
			MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " Serial communication was lost");
		}
		{
			std::lock_guard<std::mutex> lock(serialMutex);
			if (my_serial.isOpen())
			{
				my_serial.setRTS(true);
				my_serial.setDTR(true);
				my_serial.close(); // String/PortBoxBool은 건드리지 않음(DisConnect()와 구분)
			}
		}
	}

	// 자동 재연결 
	// 조건: 포트가 닫혀 있고, 사용자가 수동으로 끈 게 아니라(PortBoxBool==true), 끊김 상태(IsLost==true)일 때만 시도
	if (!my_serial.isOpen() && PortBoxBool && IsLost)
	{
		auto now = std::chrono::steady_clock::now();
		if (now >= nextAttempt)
		{
			bool ok = false;
			{
				std::lock_guard<std::mutex> lock(serialMutex);
				try
				{
					// Connect()와 동일 파라미터 재적용(PortCheck를 쓰면 내부에서 PortBoxBool을 false로 만들 수 있어 직접 처리)
					my_serial.setPort(String);
					my_serial.setBaudrate(BaudRate);
					my_serial.setBytesize(serial::bytesize_t::eightbits);
					my_serial.setTimeout(timeout);
					ASCII_HEX_Setting();

					my_serial.open();
					my_serial.setRTS(false);
					my_serial.setDTR(false);

					ok = my_serial.isOpen();
				}
				catch (...) { ok = false; }
			}

			if (ok)
			{
				// 상태 초기화 및 알림
				{
					std::lock_guard<std::mutex> lock(stateMutex);
					WorkingBool = false;
					MissingBool = false;
					BootStart = false;
					IsLost = false;
				}
				MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " Auto-reconnected");

				// 초기 명령 자동 전송 옵션(기존 동작 유지)
				AutoCliCheck();

				nextAttempt = now;
			}
			else
			{
				nextAttempt = now + std::chrono::milliseconds(backoffMs);
			}
		}
	}
	else
	{
		//딱히 필요없는데 일단 놔두기
	}
}



void PortBox::PortCheck()
{
	try
	{
		my_serial.open();
		my_serial.setRTS(false);
		my_serial.setDTR(false);
		MyGUI_Interface::GUI->PlusPathCount();
		AutoCliCheck();
	}
	catch (const std::exception e)
	{
		PortBoxBool = false;
		return;
	}
	catch (const std::invalid_argument& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("숫자 형식이 잘못되었습니다.");
		PortBoxBool = false;
		return;
	}
	catch (const std::out_of_range& e)
	{
		MsgBox::Msg->ShowWarningMessageBox("입력한 숫자가 너무 큽니다.");
		PortBoxBool = false;
		return;
	}
}

void PortBox::Connect()
{
	{
		std::lock_guard<std::mutex> lock(serialMutex);
		if (!my_serial.isOpen())
		{
			PortBoxBool = true;
			IsLost = false;
			my_serial.setPort(String);
			my_serial.setBaudrate(BaudRate);
			my_serial.setBytesize(serial::bytesize_t::eightbits);
			my_serial.setTimeout(timeout);
			ASCII_HEX_Setting();
			PortCheck();

			if (my_serial.isOpen() && !monitorRunning.exchange(true))
			{
				monitorThread = std::jthread([this](std::stop_token st)
					{
						MonitorLoop(st);
					}
				);
			}
		}
		else
		{
			return;
		}
	}
}


bool PortBox::TryReconnect()
{
	std::lock_guard<std::mutex> lock(serialMutex);

	if (my_serial.isOpen())
		return true; 

	try
	{
		my_serial.setPort(String);
		my_serial.setBaudrate(BaudRate);
		my_serial.setBytesize(serial::bytesize_t::eightbits);
		my_serial.setTimeout(timeout);
		ASCII_HEX_Setting();

		PortCheck();

		if (my_serial.isOpen())
		{
			{
				std::lock_guard<std::mutex> s(stateMutex);
				WorkingBool = false;
				MissingBool = false;
				BootStart = false;
				IsLost = false;
			}
			MyGUI_Interface::GUI->AddLogBoxString("[" + MyTime::Time->GetLocalDay() + MyTime::Time->GetLocalTime() + "] " + String + " Auto-reconnected");
			return true;
		}
	}
	catch (...)
	{
		return false; //포트가 사라졌거나 접근 거부 등. 다음 백오프 주기에 재시도.
	}
	return false;
}


void PortBox::DisConnect()
{
	MissingTime = std::chrono::steady_clock::time_point();

	if (monitorThread.joinable()) 
	{
		monitorThread.request_stop();
		monitorThread.join();
	}
	monitorRunning = false;

	CloseSerialPort();
}

void PortBox::RawMonitorClear()
{
	RawDataLog.clear();
}

void PortBox::CloseSerialPort()
{
	{
		std::lock_guard<std::mutex> lock(serialMutex);
		if (my_serial.isOpen())
		{
			MyGUI_Interface::GUI->MinusPathCount();
			my_serial.setRTS(true);
			my_serial.setDTR(true);
			my_serial.close();
			PortBoxBool = false;
			String.clear();
		}
	}
}

void PortBox::CloseSerialPortOnError()
{
	std::lock_guard<std::mutex> lock(serialMutex);
	if (my_serial.isOpen())
	{
		// NOTE: 어댑터에 따라 RTS/DTR 토글이 재연결에 영향을 줄 수 있음(장비 스펙 기준). 유지.
		my_serial.setRTS(true);
		my_serial.setDTR(true);
		my_serial.close();
	}
	// PortBoxBool, String, IsLost 유지 → MonitorLoop에서 자동 재연결 기준으로 사용.
}


void PortBox::CreateRowDataBox()
{
	ImGui::BeginChild("Row Data", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
	{ 
		if (ASCIIMODE) 
		{
			ImGuiListClipper clipper;
			clipper.Begin((int)RawDataLog.size());
			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
					ImGui::TextUnformatted(RawDataLog[i].data(), RawDataLog[i].data() + RawDataLog[i].size());
			}
		}
		else if (HEXMODE)
		{
			for (auto& Rawlog : RawHexLog)
			{
				try
				{
					ImGui::Text("%s", Rawlog.c_str());
				}
				catch (const std::exception e)
				{
					std::cout << "Hex 문자열을 읽는 도중 오류가 발생했습니다. error : " << e.what() << std::endl;
					return;
				}
			}
		}

		float scrollY = ImGui::GetScrollY();      // 현재 스크롤 위치
		float scrollMaxY = ImGui::GetScrollMaxY();// 스크롤 가능한 최대 위치
		bool isAtBottom = (scrollY >= scrollMaxY-25);// 현재 스크롤이 맨 아래인지 확인

	
		if ((scrollToBottom && isAtBottom) || (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)))
		{
			ImGui::SetScrollHereY(1.0f);
			scrollToBottom = false;
		}

	}
	ImGui::EndChild();
	ImGui::End();
	ImGui::PopStyleColor();
}


std::vector<uint8_t> HexStringToBytes(const std::string& hex) 
{
	std::vector<uint8_t> bytes;
	std::istringstream hexStream(hex);
	std::string byteStr;

	while (hexStream >> std::setw(2) >> byteStr) {
		bytes.push_back(static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16)));
	}
	return bytes;
}

void PortBox::InputCLI()
{
	{
		std::lock_guard<std::mutex> lock(serialMutex);
		if (my_serial.isOpen())
		{
			my_serial.write(MyGUI_Interface::GUI->GetCLIText() +"\n");
		}
	}
}


void PortBox::ASCII_HEX_Setting()
{
	MyGUI_Interface::GUI->GetASCIIButton() >= ZERO ? SetASCIIMODE() : SetHEXMODE();
	switch (MyGUI_Interface::GUI->GetMaxPortCount())
	{
	case 1: {
		if(WinSize> WINDOW_CHECK_SIZE)
			SetHexPrintNumberCount(69);
		else
			SetHexPrintNumberCount(55);
		break;
	}
	case 6: {
		if (WinSize > WINDOW_CHECK_SIZE)
			SetHexPrintNumberCount(22);
		else
			SetHexPrintNumberCount(17);
		break;
	}
	case 12: {
		if (WinSize > WINDOW_CHECK_SIZE)
			SetHexPrintNumberCount(16);
		else
			SetHexPrintNumberCount(12);
		break;
	}
	case 30:
	case 42: {
		if (WinSize > WINDOW_CHECK_SIZE)
			SetHexPrintNumberCount(10);
		else
			SetHexPrintNumberCount(8);
		break;
	}
	default:
		break;
	}
}



void PortBox::StartESPFlash(std::vector<std::string>& _FileName)
{
	std::function<void()> Functions = std::bind(&EspUploader::Instance, ESP.get(), std::ref(String), std::ref(_FileName));
	MyImGui::MyImGuis->GetThreadPool()->AddWork(Functions);
}


void PortBox::DebugPortSetting(std::string& _PortName)
{
	ImGui::SameLine();
	if (ImGui::Button("+"))
	{
		Bebug = !Bebug;
	}
	if (Bebug)
	{
		if (nullptr == D_Port)
		{
			D_Port = std::make_shared<DebugPortBox>();
			Set_X_Click_CallBackEvent();
		}
		D_Port->Instance(_PortName, RawDataLog);
	}
}


void PortBox::AutoCliCheck()
{
	if (MyGUI_Interface::GUI->GetAutoCLI())
	{
		my_serial.write(MyGUI_Interface::GUI->GetCLIText() + "\n");
	}
}


std::string PortBox::GetFileNameFromPath(const std::string& path) 
{
	size_t pos = path.find_last_of("\\"); 
	if (pos != std::string::npos)
		return path.substr(0, pos + 1);
	return path;
}




void PortBox::MonitorLoop(std::stop_token st)
{
	while (!st.stop_requested()) {
		SerialMonitor(); // 내부에서 isOpen() 아니면 바로 return함

		if (!my_serial.isOpen()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	monitorRunning = false;
}

