#pragma once
#include "scriptHandler.hpp"
#include "scriptHandlerNetComponent.hpp"
#include "scrThreadContext.hpp"
#include "scriptHandler.hpp"
#include "tlsContext.hpp"
#include "types.hpp"
namespace rage
{
	class scrThread {
	public:
		virtual ~scrThread() = default;                   //0 (0x00)
		virtual eThreadState reset(uint32_t script_hash, void* args, uint32_t arg_count) { return m_serialised.m_state; }     //1 (0x08)
		virtual eThreadState run() { return m_serialised.m_state; }                //2 (0x10)
		virtual eThreadState tick(uint32_t ops_to_execute) { return m_serialised.m_state; }          //3 (0x18)
		virtual void kill() {}                //4 (0x20)
		static scrThread** getPointer() {
			auto tls = uint64_t(rage::tlsContext::get());
			return reinterpret_cast<scrThread**>(tls + offsetof(rage::tlsContext, m_script_thread));
		}
		static scrThread* get() {
			return rage::tlsContext::get()->m_script_thread;
		}
	public:
		class Info {
		public:
			Info() = default;
			Info(scrValue* resultPtr, int parameterCount, scrValue* params) :
				ResultPtr(resultPtr), ParamCount(parameterCount), Params(params), BufferCount(0) { }
			// Return result, if applicable
			scrValue* ResultPtr;
			// Parameter count
			int ParamCount;
			// Pointer to parameter values
			scrValue* Params;

			int BufferCount;
			uint32_t* Orig[4];
			scrVector Buffer[4];
			void CopyReferencedParametersOut() {
				int32_t bc{ BufferCount };
				while (bc--) {
					uint32_t* dst = Orig[bc];
					uint32_t* src = (uint32_t*)&Buffer[bc].x;
					dst[0] = src[0];
					dst[1] = src[1];
					dst[2] = src[2];
				}
			}
		};
		class Serialised {
		public:
			uint32_t m_thread_id; //0x0000
			uint32_t m_script_hash; //0x0004
			eThreadState m_state; //0x0008
			uint32_t m_pointer_count; //0x000C
			uint32_t m_frame_pointer; //0x0010
			uint32_t m_stack_pointer; //0x0014
			int32_t m_timer_a; //0x0018
			int32_t m_timer_b; //0x001C
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
		} m_serialised; //0x0000
		char unk_0078[48]; //0x0078
		//scrThreadContext m_context; // 0x08
		scrValue* m_stack; //0x00B0
		uint32_t unk_00B8; //0x00B8
		uint32_t m_arg_size; //0x00BC
		uint32_t m_arg_loc; //0x00C0
		uint32_t unk_00C4; //0x00C4
		const char* m_exit_message; //0x00C8
		uint32_t unk_00D0; //0x00D0
		char m_name[64]; //0x00D4
		class scriptHandler* m_handler; //0x0110
		class CGameScriptHandlerNetComponent* m_net_component; //0x0118
	}; //Size: 0x0128
	static_assert(sizeof(scrThread) == 0x128);
}