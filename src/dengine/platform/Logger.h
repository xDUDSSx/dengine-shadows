#pragma once

#include "spdlog/spdlog.h"

#ifndef NDEBUG
#define LOG_DEBUG(...) spdlog::get("console")->debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif
#define LOG_INFO(...) spdlog::get("console")->info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::get("stderr")->warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::get("stderr")->error(__VA_ARGS__)
#define LOG_FATAL(...) spdlog::get("stderr")->critical(__VA_ARGS__)

class Logger
{
  public:
	static void init();
};
