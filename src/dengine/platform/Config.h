#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class Config
{
	static inline const fs::path root = fs::current_path();
	static inline const fs::path appLog = root / "Logs/App.log";
};
