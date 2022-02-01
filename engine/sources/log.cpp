#include "log.h"

#include <cstdarg>
#include <cstdlib>
#include <iostream>

namespace sge
{
	void Log::info(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vfprintf(stdout, format, args);
		va_end(args);
	}

	void Log::warning(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
	}

	void Log::error(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
	}
}