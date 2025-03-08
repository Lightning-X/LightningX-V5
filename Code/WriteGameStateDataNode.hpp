#pragma once
#include "Hooking.hpp"
#include "ScriptGlobal.hpp"
#include "Features.hpp"
#include "HashList.hpp"
#include "GtaUtill.hpp"
#include "Services/Player/player_service.hpp"
#include "MainScript.hpp"
namespace base
{
	bool hooks::write_player_game_state_data_node(rage::netObject* player, CPlayerGameStateDataNode* node) {
		auto ret = g_hooking->m_writePlayerGameStateDataNodeHk.getOg<decltype(&write_player_game_state_data_node)>()(player, node);
		//Spectate
		if (features::spoof_hide_spectate) {
			node->m_is_spectating = false;
			node->m_spectating_net_id = NULL;
		}
		//Frame Flags
		if (SuperJumpType == 1 || SuperJumpType == 2) {
			node->m_super_jump = false;
		}
		// No Collision
		if (features::selfNoCollision) {
			node->m_no_collision = false;
		}
		//God mode
		if (features::spoof_hide_godmode && !g_mainScript.is_in_cutscene(g_player_service->get_self()) && !g_mainScript.is_in_interior(g_player_service->get_self())) {
			node->m_is_invincible = false;
			node->m_bullet_proof = false;
			node->m_collision_proof = false;
			node->m_explosion_proof = false;
			node->m_fire_proof = false;
			node->m_melee_proof = false;
			node->m_steam_proof = false;
			node->m_water_proof = false;
		}
		return ret;
	}
}