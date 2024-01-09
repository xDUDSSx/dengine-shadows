#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/spdlog-inl.h"

#ifndef NDEBUG
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__);
#else
#define LOG_DEBUG(...)
#endif
#define LOG_INFO(...) spdlog::info(__VA_ARGS__);
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__);
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__);
#define LOG_FATAL(...) spdlog::critical(__VA_ARGS__);

class Logger
{
  public:
	static void init();
};
