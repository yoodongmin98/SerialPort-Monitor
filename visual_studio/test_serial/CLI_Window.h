#pragma once
#include <string>
#include <vector>
#include <serial/serial.h>
#include <unordered_map>

class CsvLogger;
class ExportCLI_Record;
class CLI_Window
{
public:
    CLI_Window();

    ~CLI_Window();

    CLI_Window(const CLI_Window&) = delete;
    CLI_Window(CLI_Window&&) = delete;
    CLI_Window& operator=(const CLI_Window&) = delete;
    CLI_Window& operator=(CLI_Window&&) = delete;

    void Instance(const float _X, const float _Y);
protected:
    void SplitPort();
    void PortUI();
    void SettingUI();
private:
	float WindowSizeX = 0;
	float WindowSizeY = 0;
    std::string target = "USB";
    std::vector<std::string> CLIText;
    std::vector<serial::PortInfo> PortName;
    std::vector<std::pair<std::string, std::string>> FullPortName;

    std::string ResultString;
    std::string SavePaths;


    std::shared_ptr<ExportCLI_Record> CLI_Record;
    std::shared_ptr<CsvLogger> CsvLoggers;
    std::unordered_map<std::string, std::string> resultByPort;
};
