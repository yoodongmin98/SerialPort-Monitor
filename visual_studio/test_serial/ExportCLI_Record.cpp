#pragma once
#include "ExportCLI_Record.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <chrono>

ExportCLI_Record::ExportCLI_Record()
{
}

ExportCLI_Record::~ExportCLI_Record()
{
}




// ASCII 전용 대소문자 무시 검색
static bool contains_icase(std::string_view haystack, std::string_view needle) {
    if (needle.empty()) return true;
    auto it = std::search(haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](unsigned char a, unsigned char b) {
            return std::tolower(a) == std::tolower(b);
        });
    return it != haystack.end();
}

static void rtrim_eol(std::string& s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) s.pop_back();
}

std::string ExportCLI_Record::Export(const std::string _Port, const unsigned int _Baudrate, std::string _CLI)
{
    // 열려 있으면 닫기
    if (ExportSerial.isOpen()) ExportSerial.close();

    ExportSerial.setPort(_Port);
    ExportSerial.setBaudrate(_Baudrate);



    try {
        ExportSerial.open();
    }
    catch (const std::exception& e) {
        return std::string("Open Fail: ") + e.what();
    }
    if (!ExportSerial.isOpen()) return "Open Fail";

    // 명령 전송 (장비가 CR/LF 요구하면 붙여서 보냄)
    std::string cmd(_CLI);
    if (!cmd.empty() && cmd.back() != '\n' && cmd.back() != '\r') cmd += "\r\n";
    ExportSerial.write(cmd);
    ExportSerial.flush();

    // 전체 대기 시간 제한 (예: 2초)
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);

    while (std::chrono::steady_clock::now() < deadline) 
    {
        std::string Line = ExportSerial.readline(); // 타임아웃 시 빈 문자열 가능
        if (Line.empty()) continue;

        rtrim_eol(Line);

        if (contains_icase(Line, _CLI)) {
            ExportSerial.close();
            return Line;
        }
    }

    ExportSerial.close();
    return "Timeout";
}
