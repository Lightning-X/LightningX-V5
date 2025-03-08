#pragma once

#include <cstdint>
#include "fwArchetype.hpp"

#pragma pack(push, 1)
class HashNode {
public:
	int32_t m_hash; //0x0000
	uint16_t m_idx; //0x0004
	char pad_0006[2]; //0x0006
	HashNode* m_next; //0x0008
}; //Size: 0x0010
static_assert(sizeof(HashNode) == 0x10);

template<typename T>
class HashTable {
public:
	T* m_data; //0x0000
	uint16_t m_size; //0x0008
	char pad_000A[14]; //0x000A
	uint64_t m_item_size; //0x0018
	char pad_0020[64]; //0x0020
	HashNode** m_lookup_table; //0x0060
	uint16_t m_lookup_key; //0x0068
}; //Size: 0x006A
static_assert(sizeof(HashTable<void*>) == 0x6A);
#pragma pack(pop)

enum class eModelType : std::uint8_t
{
	Invalid,
	Object,
	MLO,
	Time,
	Weapon,
	Vehicle,
	Ped,
	Destructable,
	WorldObject = 33,
	Sprinkler = 35,
	Unk65 = 65,
	EmissiveLOD = 67,
	Plant = 129,
	LOD = 131,
	Unk132 = 132,
	Unk133 = 133,
	OnlineOnlyPed = 134,
	Building = 161,
	Unk193 = 193
};

#pragma pack(push, 8)
class CBaseModelInfo : public rage::fwArchetype
{
public:
	char pad_0070[8]; //0x0070
	uint64_t unk_0078; //0x0078
	uint64_t unk_0080; //0x0080
	char pad_0088[8]; //0x0088
	uint64_t unk_0090; //0x0090
	char pad_0098[5]; //0x0098
	uint8_t m_model_type; //0x009D
	char pad_009E[6]; //0x009E
	uint64_t unk_00A8; //0x00A8
	uint8_t get_type() {
		return m_model_type & 0xFF;
	}
	bool is_type(eModelType type) {
		return get_type() == static_cast<uint8_t>(type);
	}
}; //Size: 0x00B0
static_assert(sizeof(CBaseModelInfo) == 0xB0);
#pragma pack(pop)