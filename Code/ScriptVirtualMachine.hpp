#pragma once
#include "GTAV-Classes-master/script/scrThreadContext.hpp"
#include "Hooking.hpp"
#include "Fibers/script_mgr.hpp"
#include "GTAV-Classes-master/script/globals/GlobalPlayerBD.hpp"
#include "Features.hpp"
#include "GtaUtill.hpp"
#include "Services/ScriptPatcher/script_patcher_service.hpp"
bool in_script_vm = false;
static int old_cayo_flags;
static int old_shop_index;
namespace base {
	class script_vm_guard
	{
		rage::scrProgram* m_program;
		uint8_t** m_orig_bytecode;

	public:
		script_vm_guard(rage::scrProgram* program) :
			m_program(program)
		{
			m_orig_bytecode = program->m_code_blocks;

			if (auto bytecode = g_script_patcher_service->get_script_bytecode(program->m_name_hash))
				program->m_code_blocks = bytecode;

			if (g_pointers->m_globalBase[0xA])
			{
				globals(2657704).as<GlobalPlayerBD*>()->Entries[PLAYER::PLAYER_ID()].CurrentShopIndex = old_shop_index;
				globals(2657704).as<GlobalPlayerBD*>()->Entries[PLAYER::PLAYER_ID()].CayoPericoFlags = old_cayo_flags;
			}

			in_script_vm = true;
		}

		~script_vm_guard()
		{
			m_program->m_code_blocks = m_orig_bytecode;

			if (g_pointers->m_globalBase[0xA])
			{
				old_shop_index = globals(2657704).as<GlobalPlayerBD*>()->Entries[PLAYER::PLAYER_ID()].CurrentShopIndex;
				old_cayo_flags = globals(2657704).as<GlobalPlayerBD*>()->Entries[PLAYER::PLAYER_ID()].CayoPericoFlags;

				if (features::hide_from_player_list)
				{
					globals(2657704).as<GlobalPlayerBD*>()->Entries[PLAYER::PLAYER_ID()].CurrentShopIndex = -1;
					globals(2657704).as<GlobalPlayerBD*>()->Entries[PLAYER::PLAYER_ID()].CayoPericoFlags = 1;
				}
			}

			in_script_vm = false;
		}
	};
	rage::eThreadState hooks::script_vm(rage::scrValue* stack, rage::scrValue** globals, rage::scrProgram* program, rage::scrThread::Serialised* ser) {
		script_vm_guard guard(program);
		return g_hooking->m_ScriptVirtualMachineHk.getOg<decltype(&script_vm)>()(stack, globals, program, ser);
	}
}