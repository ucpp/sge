#include "application.h"
#include "log.h"

int main(int argc, char **argv)
{
    try
    {
        std::string path = "resources/show_reel.json";
        sge::Log::info("Start demo with config: %s\n", path.c_str());
        Application application(path);
        application.run();
    }
    catch (const std::exception &e)
    {
        sge::Log::error(e.what());
        return EXIT_FAILURE;
    }

    sge::Log::info("Success quit demo.\n");

    return EXIT_SUCCESS;
}