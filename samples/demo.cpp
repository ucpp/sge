#include "core/sge_logger.h"
#include "core/sge_application.h"

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