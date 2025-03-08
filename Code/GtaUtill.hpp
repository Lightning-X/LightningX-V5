#pragma once
#include "GTAV-Classes-master/script/scrThread.hpp"
#include "GTAV-Classes-master/script/tlsContext.hpp"
#include "Pointers.hpp"
#include "GTAV-Classes-master/network/CNetworkPlayerMgr.hpp"
#include "GTAV-Classes-master/ped/CPedFactory.hpp"
#include "GTAV-Classes-master/script/scrProgramTable.hpp"
#include "GTAV-Classes-master/network/Network.hpp"
using namespace base;
namespace gta_util
{
	inline CPed* get_local_ped()
	{
		if (auto ped_factory = *g_pointers->m_pedFactory)
		{
			return ped_factory->m_local_ped;
		}

		return nullptr;
	}

	inline CVehicle* get_local_vehicle()
	{
		if (const auto ped = get_local_ped(); ped)
		{
			if (const auto veh = ped->m_vehicle; veh)
			{
				return veh;
			}
		}
		return nullptr;
	}

	inline CPlayerInfo* get_local_playerinfo()
	{
		if (auto ped_factory = *g_pointers->m_pedFactory)
		{
			if (auto ped = ped_factory->m_local_ped)
			{
				return ped->m_player_info;
			}
		}

		return nullptr;
	}

	inline CNetworkPlayerMgr* get_network_player_mgr()
	{
		return *g_pointers->m_netPlayerMgr;
	}

	inline Network* get_network()
	{
		return *g_pointers->m_network;
	}

	template<typename F, typename... Args>
	void execute_as_script(rage::scrThread* thread, F&& callback, Args&&... args)
	{
		auto tls_ctx = rage::tlsContext::get();
		auto og_thread = tls_ctx->m_script_thread;

		tls_ctx->m_script_thread = thread;
		tls_ctx->m_is_script_thread_active = true;

		std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

		tls_ctx->m_script_thread = og_thread;
		tls_ctx->m_is_script_thread_active = og_thread != nullptr;
	}

	template<typename F, typename... Args>
	void execute_as_script(rage::joaat_t script_hash, F&& callback, Args&&... args)
	{
		for (auto thread : *g_pointers->m_gtaThreads)
		{
			if (!thread || !thread->m_serialised.m_thread_id || thread->m_serialised.m_script_hash != script_hash)
				continue;

			execute_as_script(thread, callback, args...);

			return;
		}
	}

	inline GtaThread* find_script_thread(rage::joaat_t hash)
	{
		for (auto thread : *g_pointers->m_gtaThreads)
		{
			if (thread && thread->m_serialised.m_thread_id && thread->m_handler && thread->m_script_hash == hash)
			{
				return thread;
			}
		}

		return nullptr;
	}

	inline GtaThread* find_script_thread_by_id(std::uint32_t id)
	{
		for (auto thread : *g_pointers->m_gtaThreads)
		{
			if (thread && thread->m_handler && thread->m_serialised.m_thread_id == id)
			{
				return thread;
			}
		}

		return nullptr;
	}

	inline rage::scrProgram* find_script_program(rage::joaat_t hash)
	{
		for (auto& script : *g_pointers->m_scriptProgramTable)
		{
			if (script.m_program && script.m_program->m_name_hash == hash)
				return script.m_program;
		}

		return nullptr;
	}

	inline const std::optional<uint32_t> get_code_location_by_pattern(rage::scrProgram* program, const signature& pattern)
	{
		std::uint32_t code_size = program->m_code_size;
		for (std::uint32_t i = 0; i < (code_size - pattern.m_bytes.size()); i++)
		{
			for (std::uint32_t j = 0; j < pattern.m_bytes.size(); j++)
				if (pattern.m_bytes[j].has_value())
					if (pattern.m_bytes[j].value() != *program->get_code_address(i + j))
						goto incorrect;

			return i;
		incorrect:
			continue;
		}

		return std::nullopt;
	}

	inline CNetGamePlayer* getCNetGamePlayerViaPlayerIndex(uint16_t conId) {
		if (auto cNetworkPlayerMgr = *g_pointers->m_netPlayerMgr; cNetworkPlayerMgr) {
			if (auto& cNetGamePlayer = cNetworkPlayerMgr->m_player_list[conId]; cNetGamePlayer && cNetGamePlayer->IsConnected()) {
				return cNetGamePlayer;
			}
		}
		return nullptr;
	}
}