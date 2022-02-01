#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace sge
{
	bool Config::load(const std::string& path)
	{
		std::ifstream file_stream(path);
		nlohmann::json json_file;
		file_stream >> json_file;
		data = json_file.get<Data>();

		return true;
	}
}