#pragma once

#include "../rage/vector.hpp"

#include <cstdint>

namespace rage
{
	class nonPhysicalPlayerDataBase
	{
	public:
		virtual ~nonPhysicalPlayerDataBase();
		virtual void read();
		virtual void write();
		virtual void calculate_size();
		virtual void log();
	}; //Size: 0x0008
	static_assert(sizeof(nonPhysicalPlayerDataBase) == 0x8);
}

#pragma pack(push, 4)
class CNonPhysicalPlayerData : public rage::nonPhysicalPlayerDataBase
{
public:
	int32_t m_bubble_id; //0x0008
	int32_t m_player_id; //0x000C
	rage::fvector3 m_position; //0x0010
}; //Size: 0x001C
static_assert(sizeof(CNonPhysicalPlayerData) == 0x1C);
#pragma pack(pop)