#pragma once
#include "../../Pointers.hpp"
#include <cstdint>
#include "../../JSON.hpp"
#include "../../Fibers/script.hpp"
#include "../../Fibers/script_mgr.hpp"
namespace base
{
	class api_service
	{
	public:
		api_service();
		~api_service();

		// Returns if the Player is online or offline
		bool isRidOnline(std::uint64_t rid);
		// Returns true if an valid profile matching his username has been found
		int nameToRid(const std::string& name);
		// Returns true if an valid profile matching his rid has been found
		std::string ridToName(uint64_t rid);
		// Returns true if the message has been successfully sended to the target
		bool send_socialclub_message(uint64_t rid, std::string_view message);

		//void ridJoinViaInvite(std::uint64_t rid);

		//void ridJoinCFR(uint64_t rid);

		void join_session(const rage::rlSessionInfo& info);

		void ridJoinViaSession(std::uint64_t rid);

		uint64_t g_tmpRid{};
		std::string g_tmpFbrName{};

		bool isQueued;
		class rage::rlGamerHandle gamerHandle { 0 };
		class rage::rlSessionInfo info;

	private:
		const std::vector<std::string> languages = { "unset", "en", "fr", "de", "it", "es", "pt", "pl", "ru", "es-mx" };
	};

	inline api_service* g_api_service;
}