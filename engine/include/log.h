#pragma once

namespace sge
{
	class Log
	{
	public:
		static void info(const char* format, ...);
		static void warning(const char* format, ...);
		static void error(const char* format, ...);
	};
}