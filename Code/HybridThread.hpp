#pragma once
#include "Common.hpp"
#define DEFINE_AT_RTTI(T) \
	virtual T* GetIdentifier() { return nullptr; }; \
	virtual T* GetIdentifier_2() { return nullptr; }; \
	virtual uint32_t GetTypeHash() { return NULL; }; \
	virtual T* GetIfIsOfType(T* vft) { return vft; }; \
	virtual void IsOfType(T* vft) {}; \
	virtual void IsOfTypeObject(T* object) {};
namespace rageAlt
{
#pragma pack(push, 1)
	class tlsContext {
	public:
		char pad_0000[180]; //0x0000
		uint32_t m_unknown_byte; //0x00B4
		class sysMemAllocator* m_allocator; //0x00B8
		class sysMemAllocator* m_tls_entry; //0x00C0
		class sysMemAllocator* m_unk_allocator; //0x00C0
		uint32_t m_console_handle; //0x00D0
		char pad_00D4[188]; //0x00D4
		uint64_t unk_0190; //0x0190
		char pad_0194[1712]; //0x0194
		class scrThread* m_script_thread; //0x0848
		bool m_is_script_thread_active; //0x00850
		static tlsContext* get() { return *(tlsContext**)(__readgsqword(0x58)); }
		static tlsContext** getPointer() { return (tlsContext**)(__readgsqword(0x58)); }
	}; //Size: 0x0850
	static_assert(sizeof(tlsContext) == 0x851);
#pragma pack(pop)
	//Thread State
	enum class eThreadState : uint32_t {
		running,
		sleeping,
		killed,
		paused,
		breakpoint
	};
	//Thread
	class scrThreadContext {
	public:
		uint32_t m_thread_id; //0x0000
		uint32_t m_script_hash; //0x0004
		eThreadState m_state; //0x0008
		uint32_t m_instruction_pointer; //0x000C
		uint32_t m_frame_pointer; //0x0010
		uint32_t m_stack_pointer; //0x0014
		float m_timer_a; //0x0018
		float m_timer_b; //0x001C
		float m_timer_c; //0x0020
		char pad_0024[40]; //0x0024
		uint32_t m_stack_size; //0x004C
		uint32_t m_catch_instruction_pointer; //0x0050
		char pad_0054[84]; //0x0054
	}; //Size: 0x00A8
	static_assert(sizeof(scrThreadContext) == 0xA8);
	class scrThread {
	public:
		virtual ~scrThread();                   //0 (0x00)
		virtual void reset(uint32_t script_hash, void* args, uint32_t arg_count);      //1 (0x08)
		virtual eThreadState run();                //2 (0x10)
		virtual eThreadState tick(uint32_t ops_to_execute);          //3 (0x18)
		virtual void kill();                 //4 (0x20)
		static scrThread** getPointer() {
			auto tls = uintptr_t(*(uintptr_t*)__readgsqword(0x58));
			return reinterpret_cast<scrThread**>(tls + offsetof(rageAlt::tlsContext, m_script_thread));
		}
		static scrThread* get() {
			return rageAlt::tlsContext::get()->m_script_thread;
		}
	public:
		class scrThreadContext m_context; //0x0000
		void* m_stack; //0x00A8
		uint32_t unk_00B0; //0x00B0
		uint32_t m_arg_size; //0x00B4
		uint32_t m_arg_loc; //0x00B8
		uint32_t unk_00BC; //0x00BC - Some sort of arg
		const char* m_exit_message; //0x00C0
		uint32_t unk_00C8; //0x00C8
		char m_name[64]; //0x00D0
		class scriptHandler* m_handler; //0x0110
		class CGameScriptHandlerNetComponent* m_net_component; //0x0118
	}; //Size: 0x0128
	static_assert(sizeof(scrThread) == 0x128);

	class GtaThread : public rageAlt::scrThread {
	public:
		uint32_t m_script_hash; //0x0120
		uint32_t m_instruction_pointer_cleanup; //0x0124
		char unk_0128[16]; //0x0128
		int32_t m_instance_id; //0x0138
		uint32_t unk_013C; //0x013C
		uint8_t m_flag1; //0x0140
		bool m_safe_for_network_game; //0x0141
		char pad_0142[2]; //0x0142
		bool m_is_minigame_script; //0x0144
		char pad_0145[2]; //0x0145
		bool m_can_be_paused; //0x0147
		bool m_can_remove_blips_from_other_scripts; //0x0148
		char pad_0149[15]; //0x0149
	public:
		static GtaThread* get() { return *reinterpret_cast<GtaThread**>(*(uintptr_t*)__readgsqword(0x58) + offsetof(rageAlt::tlsContext, m_script_thread)); }
	};
	static_assert(sizeof(GtaThread) == 0x160);

	template <typename T>
	class atRTTI {
	public:
		DEFINE_AT_RTTI(T)
	};
	class sysMemAllocator : public atRTTI<sysMemAllocator> {
	public:
		virtual ~sysMemAllocator() = 0;
		virtual void SetQuitOnFail(bool) = 0;
		virtual void* Allocate(size_t size, size_t align, int subAllocator) = 0;
		virtual void* TryAllocate(size_t size, size_t align, int subAllocator) = 0;
		virtual void Free(void* pointer) = 0;
		virtual void TryFree(void* pointer) = 0;
		virtual void Resize(void* pointer, size_t size) = 0;
		virtual sysMemAllocator* GetAllocator(int allocator) const = 0;
		virtual sysMemAllocator* GetAllocator(int allocator) = 0;
		virtual sysMemAllocator* GetPointerOwner(void* pointer) = 0;
		virtual size_t GetSize(void* pointer) const = 0;
		virtual size_t GetMemoryUsed(int memoryBucket) = 0;
		virtual size_t GetMemoryAvailable() = 0;
	public:
		static sysMemAllocator* UpdateAllocatorValue() {
			auto tls = rageAlt::tlsContext::get();
			auto gtaTlsEntry = *reinterpret_cast<sysMemAllocator**>(uintptr_t(tls) + offsetof(rageAlt::tlsContext, m_allocator));
			if (!gtaTlsEntry)
				tls->m_allocator = gtaTlsEntry;
			tls->m_tls_entry = gtaTlsEntry;
			return gtaTlsEntry;
		}
		static sysMemAllocator* get() {
			auto allocValue = rageAlt::tlsContext::get()->m_allocator;
			if (!allocValue)
				allocValue = UpdateAllocatorValue();
			return allocValue;
		}
		static sysMemAllocator** getPointer() {
			auto tls = uintptr_t(*(uintptr_t*)__readgsqword(0x58));
			return reinterpret_cast<sysMemAllocator**>(tls + offsetof(rageAlt::tlsContext, m_allocator));
		}
		static void* getEntry() {
			auto tls = uintptr_t(*(uintptr_t*)__readgsqword(0x58));
			return *reinterpret_cast<void**>(tls + offsetof(rageAlt::tlsContext, m_tls_entry));
		}
		static void** getEntryPointer() {
			auto tls = uintptr_t(*(uintptr_t*)__readgsqword(0x58));
			return reinterpret_cast<void**>(tls + offsetof(rageAlt::tlsContext, m_tls_entry));
		}
	};
}