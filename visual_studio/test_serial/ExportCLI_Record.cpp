#include "ExportCLI_Record.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <chrono>
#include <thread>


static inline void rtrim_eol(std::string& s) 
{
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) s.pop_back();
}

static inline void trim_spaces(std::string& s) 
{
    size_t a = s.find_first_not_of(" \t");
    size_t b = s.find_last_not_of(" \t");
    if (a == std::string::npos) 
    {
        s.clear(); return; 
    }
    s = s.substr(a, b - a + 1);
}
static inline std::string tolower_copy(std::string s) 
{
    std::transform
    (
        s.begin(), s.end(), s.begin(),
        [](unsigned char c) 
        {
            return static_cast<char>(std::tolower(c)); 
        }
    );
    return s;
}

static bool contains_icase(std::string_view hay, std::string_view needle) 
{
    if (needle.empty()) 
        return true;

    std::string H(hay);
    std::string N(needle);
    H = tolower_copy(H); N = tolower_copy(N);
    return H.find(N) != std::string::npos;
}





ExportCLI_Record::ExportCLI_Record() {}
ExportCLI_Record::~ExportCLI_Record() {}

std::string ExportCLI_Record::Export(const std::string _Port,const unsigned int _Baudrate,std::string _CLI)
{
    if (ExportSerial.isOpen())
        ExportSerial.close();

    ExportSerial.setPort(_Port);
    ExportSerial.setBaudrate(_Baudrate);

    try 
    {
        ExportSerial.open();
        serial::Timeout t = serial::Timeout::simpleTimeout(50);
        ExportSerial.setTimeout(t);
    }
    catch (const std::exception& e) 
    {
        return std::string("Open Fail: ") + e.what();
    }
    if (!ExportSerial.isOpen()) return "Open Fail";

    try 
    { 
        ExportSerial.flushInput(); 
    }
    catch (...) 
    {
        while (ExportSerial.available())
            ExportSerial.read(ExportSerial.available());
    }


    std::string cmd = _CLI;
    if (!cmd.empty() && cmd.back() != '\n' && cmd.back() != '\r') cmd += "\r\n";
    ExportSerial.write(cmd);
    ExportSerial.flush();

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1500);


    const std::string normCLI = tolower_copy([](std::string s) { rtrim_eol(s); trim_spaces(s); return s; }(_CLI));
    bool echo_skipped = false;

    std::string rx;           
    std::string matched_line;  

    while (std::chrono::steady_clock::now() < deadline) 
    {
        size_t avail = 0;
        try { avail = ExportSerial.available(); }
        catch (...) { avail = 0; }

        if (avail == 0) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); continue; 
        }
        rx += ExportSerial.read(avail);

      
        for (;;) 
        {
            size_t pos = rx.find_first_of("\r\n");
            if (pos == std::string::npos) break;

            std::string line = rx.substr(0, pos);
            size_t next = rx.find_first_not_of("\r\n", pos);
            rx.erase(0, (next == std::string::npos) ? rx.size() : next);

            rtrim_eol(line); trim_spaces(line);
            if (line.empty()) continue;

            if (contains_icase(line, _CLI)) 
            {
                matched_line = line;
                goto DONE;
            }
        }
    }

DONE:
    ExportSerial.close();
    return matched_line.empty() ? std::string("Timeout") : matched_line;
}
