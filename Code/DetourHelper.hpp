#pragma once
#include "Common.hpp"
#include "Logger.hpp"
#include <MinHook/MinHook.hpp>

namespace base {
	class MinHook {
	public:
		MinHook() { MH_Initialize(); }
		~MinHook() { MH_Uninitialize(); }
	};
	class detouring {
	public:
		detouring() = default;
		detouring(std::string const& name, void* ptr, void* dtr) : m_name(name), m_dtr(dtr), m_ptr(ptr) {
			if (auto st = MH_CreateHook(m_ptr, m_dtr, &m_og); st != MH_OK && !m_name.empty()) {
				g_logger->send(logColor::red, "Detours", "Failed to create hook '{}' at 0x{:X} (error: {})", m_name, uintptr_t(m_ptr), MH_StatusToString(st));
			}
		}
		~detouring() {
			if (auto st = MH_RemoveHook(m_ptr); st != MH_OK && !m_name.empty()) {
				g_logger->send(logColor::red, "Detours", "Failed to remove hook '{}' at 0x{:X} (error: {})", m_name, uintptr_t(m_ptr), m_name);
			}
		}
		void enable() {
			if (auto st = MH_EnableHook(m_ptr); st != MH_OK && !m_name.empty()) {
				g_logger->send(logColor::red, "Detours", "Failed to enable hook 0x{:X} ({})", uintptr_t(m_ptr), MH_StatusToString(st));
			}
		}
		void disable() {
			if (auto st = MH_DisableHook(m_ptr); st != MH_OK && !m_name.empty()) {
				g_logger->send(logColor::red, "Detours", "Failed to disabled hook {}.", m_name);
			}
		}
		template <typename type>
		type getOg() {
			return type(m_og);
		}
		std::string m_name;
		void* m_dtr{};
		void* m_ptr{};
		void* m_og{};
	};
}