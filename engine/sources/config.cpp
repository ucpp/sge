#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace sge
{
    // TODO safe read and load
    bool Config::load(const std::string &path)
    {
        std::ifstream file_stream(path);
        nlohmann::json j;
        file_stream >> j;
        data = j.get<Data>();

        return true;
    }
}