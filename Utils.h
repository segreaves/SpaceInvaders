#pragma once
#define RUNNING_WINDOWS
#include <iostream>
#include <string>
#include <algorithm>

namespace Utils
{
#ifdef RUNNING_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>
	inline std::string getWorkingDirectory() {
		HMODULE hModule = GetModuleHandle(nullptr);
		if (hModule) {
			char path[256];
			GetModuleFileName(hModule, path, sizeof(path));
			PathRemoveFileSpec(path);
			strcat_s(path, "\\");
			return std::string(path);
		}
		return "";
	}
#elif defined RUNNING_LINUX
#include <unistd.h>
	inline std::string getWorkingDirectory()
	{
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != nullptr) return std::string(cwd) + std::string("/");
		return "";
	}
#elif defined(RUNNING_MACOS) || defined(__APPLE__)
#include <unistd.h>
#include <limits.h>
#include <mach-o/dyld.h>
	inline std::string getWorkingDirectory()
	{
		char path[PATH_MAX];
		uint32_t size = sizeof(path);
		if (_NSGetExecutablePath(path, &size) == 0) {
			std::string fullPath(path);
			auto pos = fullPath.find_last_of('/');
			if (pos != std::string::npos) {
				return fullPath.substr(0, pos + 1);
			}
		}
		return "";
	}
	inline std::string getAssetsDirectory() {
		return getWorkingDirectory() + "../Resources/assets/";
	}
#endif
}