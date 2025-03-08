#pragma once
#include "../../Pointers.hpp"
#include "../../GTAV-Classes-master/network/CNetGamePlayer.hpp"
#include "../../GTAV-Classes-master/socialclub/FriendRegistry.hpp"
#include "../../GtaUtill.hpp"
namespace base
{
	extern std::string getFriendStateStr(std::uint32_t state, uint32_t joinable);
	class friends_service final
	{
	public:
		friends_service();
		~friends_service();

		friends_service(const friends_service&) = delete;
		friends_service(friends_service&&) noexcept = delete;
		friends_service& operator=(const friends_service&) = delete;
		friends_service& operator=(friends_service&&) noexcept = delete;

		[[nodiscard]] static bool is_friend(CNetGamePlayer* net_player);

	};
	inline friends_service* g_friends_service{};
}