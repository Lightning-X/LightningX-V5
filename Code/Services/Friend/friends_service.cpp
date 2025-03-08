#include "friends_service.hpp"
#include "../../Translations.hpp"
namespace base
{
	std::string getFriendStateStr(std::uint32_t state, uint32_t joinable) {
		switch (state) {
		case 0: return TRANSLATE("F_OFFLINE"); break;
		case 1: return TRANSLATE("F_SINGLEPLAYER"); break;
		case 2: return TRANSLATE("F_ONLINE"); break;
		case 3: return joinable ? TRANSLATE("F_JOINABLE") : TRANSLATE("F_MULTIPLAYER"); break;
		}
		return TRANSLATE("F_UNKNOWN");
	}
	friends_service::friends_service()
	{
		g_friends_service = this;
	}

	friends_service::~friends_service()
	{
		g_friends_service = nullptr;
	}

	bool friends_service::is_friend(CNetGamePlayer* net_player)
	{
		if (net_player == nullptr)
			return false;

		const auto rockstar_id = net_player->GetGamerInfo()->m_gamer_handle.m_rockstar_id;
		for (std::uint32_t i = 0; i < g_pointers->m_friendRegistry->m_friend_count; i++)
			if (rockstar_id == g_pointers->m_friendRegistry->get(i)->m_rockstar_id)
				return true;
		return false;
	}
}