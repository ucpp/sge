#include "sge_logger.h"
#include "sge_application.h"

int main()
{
    try
    {
        SGE::Application app("resources/configs/application_settings.json");
        app.Run();
    }
    catch(const std::exception& e)
    {
        LOG_ERROR(e.what());
    }
}