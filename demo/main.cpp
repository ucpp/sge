#include <iostream>

#include "application.h"

int main(int argc, char **argv)
{
    std::cout << "Staring demo..." << std::endl;

    try
    {
        Application application("resources/demo_config.json");
        application.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Quit demo" << std::endl;
    return EXIT_SUCCESS;
}