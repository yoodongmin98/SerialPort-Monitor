#include "Core.h"
#include "EngineDebug.h"
#include <fstream>
#include <iostream>




int main()
{
    //ShowWindow(GetConsoleWindow(), SW_NORMAL);
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    try 
    {
        EngineDebug::LeakCheck();
        new int; //Leak Check��
        std::shared_ptr<EngineCore> Cores = std::make_shared<EngineCore>();
        Cores->Instance();
    }
    catch (const std::exception& e) {
        std::ofstream logFile("error_log.txt", std::ios::app);
        logFile << "������ �߻��߽��ϴ�. Error : " << e.what() << "\n";
        logFile.close();
        std::cerr << "������ �߻��߽��ϴ�. Error : " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...) {
        std::ofstream logFile("error_log.txt", std::ios::app);
        logFile << "�˼����� ������ �߻��߽��ϴ�.\n";
        logFile.close();
        std::cerr << "�˼����� ������ �߻��߽��ϴ�.\n";
        return EXIT_FAILURE;
    }
   
    return EXIT_SUCCESS;
}