#pragma once
#include "rlGamerHandle.hpp"
#include "rlSessionInfo.hpp"

namespace rage
{
	class rlPresenceEventInviteAccepted {
	public:
		rlPresenceEventInviteAccepted(rlGamerInfo& gamerInfo, rlSessionInfo& sessionInfo, rlGamerHandle& gamerHandle) : m_gamer_info(gamerInfo), m_session_info(sessionInfo), m_gamer_handle(gamerHandle) {
			m_self = this;
		}

		void* m_vft; //0x0000
		rlPresenceEventInviteAccepted* m_self; //0x0008
		rlGamerInfo m_gamer_info; //0x0010
		rlSessionInfo m_session_info; //0x0108
		rlGamerHandle m_gamer_handle; //0x01D8
	}; //Size: 0x01E8
	//static_assert(sizeof(rlPresenceEventInviteAccepted) == 0x1E8);
	class rlSessionByGamerTaskResult
	{
	public:
		rlGamerHandle m_gamer_handle{ 0 };
		rlSessionInfo m_session_info;
	};
}