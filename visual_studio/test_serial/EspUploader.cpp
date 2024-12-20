#include "EspUploader.h"
#include <filesystem>
#include <iostream>


namespace fs = std::filesystem;



EspUploader::EspUploader()
{

}

EspUploader::~EspUploader()
{

}




void EspUploader::Instance(std::string& _PortNum)
{
	fs::path exePath = fs::current_path();
    std::vector<std::string> FirmWareDirectory =
    { (exePath / "bootloader.bin").string(),
      (exePath / "partitions.bin").string(),
      (exePath / "boot_app0.bin").string(),
      (exePath / "firmware.bin").string() };


    std::string systemcmd = "esptool --port " + _PortNum + " erase_flash";
    if (std::system(systemcmd.c_str()) == 0)
        std::cout << _PortNum + " �� Erase�� �Ϸ�Ǿ����ϴ�." << std::endl;
    else 
        std::cout << _PortNum + " �� Erase �����߽��ϴ�." << std::endl;
}