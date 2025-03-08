#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WIN7

//Windows Includes (SDKs)
#include <winsdkver.h>
#include <sdkddkver.h>
#include <Windows.h>
#include <winternl.h>
#include <D3D11.h>
#include <cassert>
#include <cinttypes>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <type_traits>
#include <map>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>
#include <libloaderapi.h>
#include "StackWalker/StackWalker.hpp"

//Menu Macros
#define stringifyFn(x) #x
#define brandingName "Lightning X"
#define upperBrandingName "LIGHTNING-X"
#define lowerBrandingName "lightning-x"
#define shortBrandingName "LX"
#define global inline

//Ease Of Life Defines
using namespace std::chrono_literals;
namespace fs = std::filesystem;
template <typename type>
using comPtr = Microsoft::WRL::ComPtr<type>;

//Base Defines
namespace base {
	inline HANDLE g_main_thread{};
	inline DWORD g_main_thread_id{};
	inline HMODULE g_module{};
	inline bool g_running = true;
	inline std::int32_t g_selectedPlayer; //Player
	inline int g_selectedFriend;
}
template <typename invokable, typename ...invokableArgs>
concept isInvokableWithArgs = requires(invokable callable, invokableArgs const& ...vaArgs) {
	callable(vaArgs...);
};

template <typename fT>
inline fT nullsub() {}

inline bool isNumber(const std::string& str) {
	for (char const& c : str)
		if (std::isdigit(c) == 0)
			return false;
	return true;
}

template <typename t> requires std::is_function_v<t>
using fnptr = t(*);

// Disables exception handler for extensive debugging
#define disableExceptionHandler false

// Disables R*'s telemetry from being sent out
#define blockTelemetryEndpoints true

// Disable all auth related things, such as string encryption, debug protection, module altering, and pointer encryption
#define disableSecurityMeasures true

// Disables the translation system
#define DISABLE_TRANSLATIONS false

// Disables the config part of the translations
#define DISBABLE_CONFIG_SECTION true

inline std::string lStr(std::string str) {
	std::string t{ str };
	std::transform(t.begin(), t.end(), t.data(), [](char c) { return tolower(c); });
	return t;
}