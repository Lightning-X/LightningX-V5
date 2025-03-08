#pragma once
#include "scrThread.hpp"

class GtaThread : public rage::scrThread {
public:
	uint32_t m_script_hash; //0x0128
	uint32_t m_pointer_count_cleanup; //0x012C
	uint32_t m_unk_130; //0x0130
	uint32_t m_unk_134; //0x0134
	uint32_t m_unk_138; //0x0138
	int32_t m_unk_13C; //0x013C
	int32_t m_instance_id; //0x0140
	uint32_t m_base_instance_id; //0x0144
	uint8_t m_flag; //0x0148
	bool m_safe_for_network_game; //0x0149
	bool m_allow_host_broadcast; //0x014A
	bool m_unk_14B; //0x014A
	bool m_is_minigame_script; //0x014C
	char pad_014D[2]; //0x014D
	bool m_can_be_paused; //0x014F
	bool m_can_remove_blips_from_other_scripts; //0x0150
	bool unk_0151; //0x0151
	bool m_allow_attachment; //0x0152
	bool m_detached; //0x0153
	uint8_t m_flag_2; //0x0154
	char pad_0155[11]; //0x0155
public:
	static GtaThread* get() { return *reinterpret_cast<GtaThread**>(*(uintptr_t*)__readgsqword(0x58) + offsetof(rage::tlsContext, m_script_thread)); }
};
static_assert(sizeof(GtaThread) == 0x160);