#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define DEFAULT_LOG_PATTERN "[%H:%M:%S] [%l]: %v"

void Logger::init()
{
	auto console = spdlog::stdout_color_mt("console");
	console->set_pattern(DEFAULT_LOG_PATTERN);
	auto err_logger = spdlog::stderr_color_mt("stderr");
	err_logger->set_pattern(DEFAULT_LOG_PATTERN);

	// spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

	console->set_level(spdlog::level::info);
#ifndef NDEBUG
	console->set_level(spdlog::level::debug); // Set global log level to debug
#endif
}
