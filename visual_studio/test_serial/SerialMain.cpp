#include <iostream>
#include "Core.h"
#include <fstream>




int main()
{
    try {
        std::shared_ptr<EngineCore> Cores = std::make_shared<EngineCore>();
        Cores->Instance();
    }
    catch (const std::exception& e) {
        std::ofstream logFile("error_log.txt", std::ios::app);
        logFile << "Unhandled exception: " << e.what() << "\n";
        logFile.close();
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...) {
        std::ofstream logFile("error_log.txt", std::ios::app);
        logFile << "Unknown error occurred.\n";
        logFile.close();
        std::cerr << "Unknown error occurred.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}