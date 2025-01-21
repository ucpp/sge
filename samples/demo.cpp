#include "sge_logger.h"
#include "sge_application.h"
#include "sge_application_settings.h"

int main()
{
    try
    {
        SGE::Application app(SGE::DEFAULT_SETTINGS_PATH);
        app.Run();
    }
    catch(const std::exception& e)
    {
        LOG_ERROR(e.what());
    }
}