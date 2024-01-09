#pragma once

#include <memory>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

#include "dengine/platform/Logger.h"

template <typename T>
using Ptr = std::shared_ptr<T>;

template <typename T>
using UPtr = std::unique_ptr<T>;

template <typename T>
using WPtr = std::weak_ptr<T>;

#include "magic_enum.hpp"

template <typename T>
std::string enumName(T val)
{
	return std::string(magic_enum::enum_name(val));
}

#define D_ASSERT(cond, message)                                                                                                  \
	if (!(cond))                                                                                                                 \
	{                                                                                                                            \
		LOG_FATAL(message);                                                                                                      \
	}                                                                                                                            \
	assert(cond)

#define D_ABORT(message)                                                                                                         \
	LOG_FATAL(message);                                                                                                          \
	std::abort()