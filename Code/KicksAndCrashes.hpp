#pragma once
#include "Services/Player/player_service.hpp"
using namespace base;
namespace Toxic {
	inline bool show_cheating_message = false;
	void BreakUpKick(player_ptr player);
	void DesyncKick(CNetGamePlayer* player);
	void HostKick(player_ptr player);
	void NullFunctionKick(player_ptr player);
	void BreakFragmentCrash(player_ptr player);
	void InvalidPedCrash(player_ptr player);
	void ScriptHostKick(player_ptr player);
	void BailKick(player_ptr player);
	void EndSessionKick(player_ptr player);
	//OTher
	struct explosion_anti_cheat_bypass
	{
		inline static memory::byte_patch* m_can_blame_others;
		inline static memory::byte_patch* m_can_use_blocked_explosions;
	};
	struct police
	{
		inline static memory::byte_patch* m_max_wanted_level;
		inline static memory::byte_patch* m_max_wanted_level_2;
	};
	//Other
	void blame_explode_player(player_ptr to_blame, player_ptr target, eExplosionTag explosion_type, float damage, bool is_audible, bool is_invisible, float camera_shake);
}