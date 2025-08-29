#pragma once
#include <serial/serial.h>
#include <string>
#include <vector>


class ExportCLI_Record
{
public:
    ExportCLI_Record();
    ~ExportCLI_Record();

    ExportCLI_Record(const ExportCLI_Record&) = delete;
    ExportCLI_Record(ExportCLI_Record&&) = delete;
    ExportCLI_Record& operator=(const ExportCLI_Record&) = delete;
    ExportCLI_Record& operator=(ExportCLI_Record&&) = delete;

    std::string Export(const std::string _Port, const unsigned int _Baudrate, std::string _CLI);
protected:

private:
    serial::Serial ExportSerial;

};
