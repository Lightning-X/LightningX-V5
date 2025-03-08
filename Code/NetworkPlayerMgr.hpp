#pragma once
#include "Hooking.hpp"
#include "Services/Player/player_service.hpp"
#include "Notify.hpp"
#include "MainScript.hpp"
namespace base {
	bool hooks::network_player_mgr_init(CNetworkPlayerMgr* _this, std::uint64_t a2, std::uint32_t a3, std::uint32_t a4[4]) {
		bool result = g_hooking->m_networkPlayerMgrInitHk.getOg<decltype(&network_player_mgr_init)>()(_this, a2, a3, a4);
		g_player_service->player_join(_this->m_local_net_player);
		return result;
	}
	void hooks::network_player_mgr_shutdown(CNetworkPlayerMgr* _this) {
		m_spoofed_peer_ids.clear();
		g_mainScript.m_remote_player_teleports.clear();
		g_player_service->do_cleanup();
		g_hooking->m_networkPlayerMgrShutdownHk.getOg<decltype(&network_player_mgr_shutdown)>()(_this);
	}
}