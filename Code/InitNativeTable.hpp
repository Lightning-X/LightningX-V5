#pragma once
#include "Hooking.hpp"
#include "NativeHooking.hpp"
#include "Services/ScriptPatcher/script_patcher_service.hpp"
namespace base {
	bool hooks::init_native_tables(rage::scrProgram* program) {
		bool ret = g_hooking->m_InitNativeTablesHk.getOg<decltype(&init_native_tables)>()(program);
		g_script_patcher_service->on_script_load(program);
		g_native_hooks->hook_program(program);
		return ret;
	}
}