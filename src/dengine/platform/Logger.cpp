#include "Logger.h"

#define DEFAULT_LOG_PATTERN "[%H:%M:%S] [%l]: %v"

void Logger::init()
{
	spdlog::set_pattern(DEFAULT_LOG_PATTERN);
	// spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

	spdlog::set_level(spdlog::level::info);
#ifndef NDEBUG
	spdlog::set_level(spdlog::level::debug); // Set global log level to debug
#endif
}
