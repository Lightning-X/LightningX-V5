#pragma once
#include "GuiScript.hpp"
#include "UI/regularOpt.hpp"
#include "UI/subOpt.hpp"
#include "UI/baseSub.hpp"
#include "UI/boolOpt.hpp"
#include "UI/numBoolOpt.hpp"
#include "UI/numOpt.hpp"
#include "UI/vecOpt.hpp"
#include "UI/dxUiManager.hpp"
#include "Services/Player/player_service.hpp"
#include "GTAV-Classes-master/script/globals/GlobalPlayerBD.hpp"
namespace base {
	struct remote_player_teleport {
		std::int16_t m_player_net_id;
		rage::fvector3 m_position;
	};
	class mainScript {
	public:
		void initUi();
	public:
		bool m_listUi = false;
		ankerl::unordered_dense::map<std::uint16_t, remote_player_teleport> m_remote_player_teleports;
		bool is_in_cutscene(player_ptr player) const {
			return player->is_valid() && player->get_player_info() && player->get_player_info()->m_game_state == eGameState::InMPCutscene;
		}
		std::int32_t get_interior_from_player(Player Splayer) const {
			return globals(2657704).as<GlobalPlayerBD*>()->Entries[Splayer].CurrentInteriorIndex;
		}
		bool is_in_interior(player_ptr player) const {
			return player->is_valid() && get_interior_from_player(player->id()) != 0;
		}
	};
	inline mainScript g_mainScript;
}