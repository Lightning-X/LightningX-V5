#include "player_service.hpp"

#include "../../GtaUtill.hpp"

#include "../../GTAV-Classes-master/network/CNetworkPlayerMgr.hpp"
#include "../../Threading.hpp"
namespace base
{
	player_service::player_service() :
		m_self(nullptr),
		m_selected_player(m_dummy)
	{
		g_player_service = this;

		const auto network_player_mgr = gta_util::get_network_player_mgr();
		if (!network_player_mgr)
			return;

		m_self = &network_player_mgr->m_local_net_player;

		for (uint16_t i = 0; i < network_player_mgr->m_player_limit; ++i)
			player_join(network_player_mgr->m_player_list[i]);
	}

	player_service::~player_service()
	{
		g_player_service = nullptr;
	}

	void player_service::do_cleanup()
	{
		m_player_to_use_end_session_kick.reset();
		m_player_to_use_complaint_kick.reset();
		m_selected_player = m_dummy;
		m_players.clear();
	}

	player_ptr player_service::get_by_msg_id(uint32_t msg_id) const
	{
		for (const auto& [_, player] : m_players)
			if (player->get_net_game_player()->m_msg_id == msg_id)
				return player;
		return nullptr;
	}

	player_ptr player_service::get_by_name(std::string name) const
	{
		for (const auto& [_, player] : m_players)
			if (player->get_name() == name)
				return player;
		return nullptr;
	}

	player_ptr player_service::get_by_id(uint32_t id) const
	{
		for (const auto& [_, player] : m_players)
			if (player->id() == id)
				return player;
		return nullptr;
	}

	player_ptr player_service::get_by_host_token(uint64_t token) const
	{
		for (const auto& [_, player] : m_players)
			if (player->get_net_data()->m_host_token == token)
				return player;
		return nullptr;
	}

	player_ptr player_service::get_selected()
	{
		if (m_selected_player)
			return m_selected_player;
		return get_self();
	}

	player_ptr player_service::get_self()
	{
		if (!m_self_ptr || !m_self_ptr->equals(*m_self))
		{
			m_self_ptr = std::make_shared<player>(*m_self);
			//m_self_ptr->command_access_level = CommandAccessLevel::ADMIN;
		}

		return m_self_ptr;
	}

	void player_service::player_join(CNetGamePlayer* net_game_player)
	{
		if (net_game_player == nullptr || net_game_player == *m_self)
			return;

		auto plyr = std::make_shared<player>(net_game_player);
		m_players.insert({ plyr->get_name(), std::move(plyr) });
	}

	void player_service::player_leave(CNetGamePlayer* net_game_player)
	{
		if (net_game_player == nullptr)
			return;

		if (m_selected_player && m_selected_player->equals(net_game_player))
			m_selected_player = m_dummy;

		if (auto it = std::find_if(m_players.begin(),
			m_players.end(),
			[net_game_player](const auto& p) {
				return p.second->id() == net_game_player->m_player_id;
			});
			it != m_players.end())
		{
			if (m_player_to_use_end_session_kick == it->second)
				m_player_to_use_end_session_kick = std::nullopt;

			if (m_player_to_use_complaint_kick == it->second)
				m_player_to_use_complaint_kick = std::nullopt;

			m_players.erase(it);
		}
	}

	void player_service::set_selected(player_ptr plyr)
	{
		m_selected_player = plyr;
	}

	std::string player_service::GetPlayerIdentifications(player_ptr plyr) const {
		std::string identifications;
		identifications.reserve(128); // Reserve memory for better performance
		identifications += plyr->get_name();
		identifications += " [";
		if (plyr->id() == g_player_service->get_self()->id())
			identifications += "You, ";
		if (plyr->is_host())
			identifications += "Host, ";
		GtaThread* thread = getThrUsingHash("freemode"_joaat);
		if (thread && thread->m_net_component) {
			if (thread->m_net_component->is_player_host(plyr->get_net_game_player()))
				identifications += "Script Host, ";
		}
		if (plyr->is_friend())
			identifications += "Friend, ";
		if (plyr->is_modder)
			identifications += "Modder, ";
		if (plyr->is_spammer)
			identifications += "Spammer, ";
		if (plyr->is_admin)
			identifications += "Admin, ";
		if (identifications == plyr->get_name() + (std::string)" [")
			return plyr->get_name();
		else
			return identifications.substr(0, identifications.size() - 2) + "]";
	}
}