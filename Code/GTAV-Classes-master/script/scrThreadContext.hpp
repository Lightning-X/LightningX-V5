#pragma once
#include <cstdint>

#include "../rage/joaat.hpp"

namespace rage
{
	enum class eThreadState : std::uint32_t
	{
		running,
		sleeping,
		killed,
		paused,
		breakpoint
	};

#pragma pack(push, 8)
	class scrThreadContext {
	public:
		uint32_t m_thread_id; //0x0000
		joaat_t m_script_hash; //0x0004
		eThreadState m_state; //0x0008
		uint32_t m_pointer_count; //0x000C
		uint32_t m_frame_pointer; //0x0010
		uint32_t m_stack_pointer; //0x0014
		float m_timer_a; //0x0018
		float m_timer_b; //0x001C
		float m_wait; //0x0020
		int32_t m_min_pc; //0x0024
		int32_t m_max_pc; //0x0028
		char m_tls[36]; //0x002C
		uint32_t m_stack_size; //0x0050
		uint32_t m_catch_pointer_count; //0x0054
		uint32_t m_catch_frame_pointer; //0x0058
		uint32_t m_catch_stack_pointer; //0x005C
		uint32_t m_priority; //0x0060
		uint8_t m_call_depth; //0x0060
		uint8_t unk_0061; //0x0061
		uint16_t unk_0062; //0x0062
		char m_callstack[16]; //0x0068
	}; //Size: 0x0078
	static_assert(sizeof(scrThreadContext) == 0x78);
#pragma pack(pop)
}