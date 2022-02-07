#pragma once

#include <sstream>

namespace sge
{
	class Utils
	{
    public:
        template <typename T>
        static std::string to_string(const T value, const int n = 6)
        {
            std::ostringstream out;
            out.precision(n);
            out << std::fixed << value;
            return out.str();
        }
	};
}