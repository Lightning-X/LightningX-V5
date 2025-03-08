#pragma once
#include "Common.hpp"
#include "VMTHook.hpp"
#include "Pointers.hpp"
#include "LabelManager.hpp"
#include "D3DRenderer.hpp"
#include "GuiScript.hpp"
#include "Natives.hpp"
#include "Protections.hpp"
#include "Logger.hpp"
#include "DetourHelper.hpp"
#include "GTAV-Classes-master/netsync/nodes/player/CPlayerCameraDataNode.hpp"
#include "Enums.hpp"
#include "GTAV-Classes-master/netsync/nodes/ped/CPedInventoryDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/vehicle/CVehicleGadgetDataNode.hpp"
#include "NetGameEvent.hpp"
#include "GTAV-Classes-master/netsync/nodes/task/ClonedTakeOffPedVariationInfo.hpp"
#include "GTAV-Classes-master/netsync/nodes/vehicle/CVehicleProximityMigrationDataNode.hpp"
inline ankerl::unordered_dense::segmented_set<std::string> kicked_Players;
inline ankerl::unordered_dense::segmented_map<std::uint64_t, std::uint64_t> m_spoofed_peer_ids;
inline CNetGamePlayer* m_syncing_player = nullptr;
namespace base {
	struct hooks {
		static rage::eThreadState script_vm(rage::scrValue* stack, rage::scrValue** globals, rage::scrProgram* program, rage::scrThread::Serialised* ser);
		static bool init_native_tables(rage::scrProgram* program);
		// Network
		static void* assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, std::uint8_t new_index);
		static bool receive_net_message(void* netConnectionManager, void* a2, rage::netConnection::InFrame* frame);
		static void received_event(rage::netEventMgr* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, int unk, rage::datBitBuffer* bit_buffer);
		static bool network_player_mgr_init(CNetworkPlayerMgr* _this, std::uint64_t a2, std::uint32_t a3, std::uint32_t a4[4]);
		static void network_player_mgr_shutdown(CNetworkPlayerMgr* _this);
		static CBaseModelInfo* get_model_info(rage::joaat_t hash, uint32_t* a2);
		static __int64 task_jump_constructor(std::uint64_t _This, int Flags);
		static void* FallTaskConstructor(std::uint64_t _this, std::uint32_t flags);
		static bool write_player_game_state_data_node(rage::netObject* player, CPlayerGameStateDataNode* node);
		static void write_player_camera_data_node(rage::netObject* player, CPlayerCameraDataNode* node);
		static void write_vehicle_proximity_migration_data_node(rage::netObject* veh, CVehicleProximityMigrationDataNode* node);
		static void get_network_event_data(int64_t unk, rage::CEventNetwork* net_event);
		//Protections
		static bool received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, int32_t object_id, int32_t object_flag, rage::datBitBuffer* buffer, int32_t timestamp);
		static eAckCode received_clone_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, uint16_t object_id, rage::datBitBuffer* bufer, uint16_t unk, uint32_t timestamp);
		static bool can_apply_data(rage::netSyncTree* tree, rage::netObject* object);
		static void invalid_mods_crash_detour(int64_t a1, int64_t a2, int a3, char a4);
		static uint64_t invalid_decal(uintptr_t a1, int a2);
		static int task_parachute_object_0x270(uint64_t _this, int a2, int a3);
		static void serialize_take_off_ped_variation_task(ClonedTakeOffPedVariationInfo* info, rage::CSyncDataBase* serializer);
		static bool fragment_physics_crash_2(float* a1, float* a2);
		//others
		static void queue_dependency(void* dependency);
		//static int nt_query_virtual_memory(void* _this, HANDLE handle, PVOID base_addr, int info_class, MEMORY_BASIC_INFORMATION* info, int size, size_t* return_len);
		static LPVOID convertThreadToFiber(LPVOID param);
		static LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static HRESULT present(IDXGISwapChain* dis, UINT syncInterval, UINT flags);
		static HRESULT resizeBuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
	};
	class hooking {
	public:
		friend struct hooks;
		hooking();
		~hooking();
		void hook();
		void unhook();
		vmt_hook m_dxHk;
		MinHook m_mhKeepalive{};
		detouring m_ScriptVirtualMachineHk{};
		detouring m_InitNativeTablesHk{};
		detouring m_getModelInfoHk{};
		detouring m_assignPhysicalIndexHk{};
		detouring m_receiveNetMessageHk{};
		detouring m_receiveEventHk{};
		detouring m_networkPlayerMgrInitHk{};
		detouring m_networkPlayerMgrShutdownHk{};
		detouring m_taskJumpConstructorHk{};
		detouring m_taskFallConstructorHk{};
		detouring m_writePlayerGameStateDataNodeHk{};
		detouring m_writePlayerCameraDataNodeHk{};
		detouring m_writeVehicleProximityMigrationDataNodeHk{};
		detouring m_getNetworkEventDataHk{};
		//Protections
		detouring m_receivedCloneCreateHk{};
		detouring m_receivedCloneSyncHk{};
		detouring m_canApplyDataHk{};
		detouring m_invalidModsCrashDetourHk{};
		detouring m_invalidDecalHk{};
		detouring m_taskParachuteObject0x270Hk{};
		detouring m_serializeTakeOffPedVariationTaskHk{};
		detouring m_fragmentPhysicsCrash2Hk{};
		//Others
		detouring m_queueDependencyHk{};
		detouring m_convertThreadToFiberHk{};
		WNDPROC m_ogWndProc{};
	};
	inline std::unique_ptr<hooking> g_hooking;
}