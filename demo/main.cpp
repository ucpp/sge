#include <iostream>

#include "application.h"

int main()
{
    std::cout << "Staring demo..." << std::endl;

    try
    {
        Application application(1024, 768, "Demo");
        application.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Quit demo" << std::endl;
    return EXIT_SUCCESS;
}