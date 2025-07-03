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
        new int; //Leak Check용
        std::shared_ptr<EngineCore> Cores = std::make_shared<EngineCore>();
        Cores->Instance();
    }
    catch (const std::exception& e) {
        std::ofstream logFile("error_log.txt", std::ios::app);
        logFile << "에러가 발생했습니다. Error : " << e.what() << "\n";
        logFile.close();
        std::cerr << "에러가 발생했습니다. Error : " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...) {
        std::ofstream logFile("error_log.txt", std::ios::app);
        logFile << "알수없는 오류가 발생했습니다.\n";
        logFile.close();
        std::cerr << "알수없는 오류가 발생했습니다.\n";
        return EXIT_FAILURE;
    }
   
    return EXIT_SUCCESS;
}