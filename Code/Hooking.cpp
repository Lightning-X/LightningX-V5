#include "Hooking.hpp"
#include "DetourHelper.hpp"
#include "ui/dxUiManager.hpp"
//Hooks
#include "ScriptVirtualMachine.hpp"
#include "WriteGameStateDataNode.hpp"
#include "WritePlayerCameraDataNode.hpp"
#include "WriteVehicleMigrationDataNode.hpp"
#include "AssignPhysicalIndex.hpp"
#include "GetModelInfo.hpp"
#include "TaskJumpAndFallConstructor.hpp"
#include "InitNativeTable.hpp"
#include "NetworkPlayerMgr.hpp"
#include "ReceiveNetMessage.hpp"
#include "ReceivedCloneCreate.hpp"
#include "ReceivedCloneSync.hpp"
#include "CanApplyData.hpp"
#include "GetNetworkEventData.hpp"
#include "InvalidCrashes.hpp"
#include "SerializeDataNode.hpp"
#include "ReceiveEvent.hpp"
#include <Psapi.h>
int rage::CGameScriptHandlerNetComponent::get_participant_index(CNetGamePlayer* player) {
	if (player == (*g_pointers->m_netPlayerMgr)->m_local_net_player)
		return m_local_participant_index;
	if (m_num_participants <= 1)
		return -1;
	for (decltype(m_num_participants) i{}; i != m_num_participants - 1; ++i) {
		if (m_participants[i] && m_participants[i]->m_net_game_player == player)
			return m_participants[i]->m_participant_index;
	}
	return -1;
}
bool rage::CGameScriptHandlerNetComponent::force_host() {
	if (auto& cNetworkPlayerMgr = *g_pointers->m_netPlayerMgr; cNetworkPlayerMgr) {
		for (int32_t i{}; !is_local_player_host(); ++i) {
			if (i > 200)
				return false;

			send_host_migration_event(cNetworkPlayerMgr->m_local_net_player);
			send_host_migration_event(cNetworkPlayerMgr->m_local_net_player);
		}
		return is_local_player_host();
	}
	return false;
}
namespace base {
	bool inline is_address_in_game_region(uint64_t address) {
		static uint64_t moduleBase = NULL;
		static uint64_t moduleSize = NULL;
		if ((!moduleBase) || (!moduleSize)) {
			MODULEINFO info;
			if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info))) {
				g_logger->send(red, "", "GetModuleInformation failed!");
				return true;
			}
			else {
				moduleBase = (uint64_t)GetModuleHandle(0);
				moduleSize = (uint64_t)info.SizeOfImage;
			}
		}
		return address > moduleBase && address < (moduleBase + moduleSize);
	}
	bool is_jump(__int64 fptr) {
		if (!is_address_in_game_region(fptr))
			return false;
		auto value = *(std::uint8_t*)(fptr);
		return value == 0xE9;
	}
	bool is_unwanted_dependency(__int64 cb) {
		auto f1 = *(__int64*)(cb + 0x60);
		auto f2 = *(__int64*)(cb + 0x100);
		auto f3 = *(__int64*)(cb + 0x1A0);
		if (!is_address_in_game_region(f1) || !is_address_in_game_region(f2) || !is_address_in_game_region(f3))
			return false;
		return is_jump(f1) || is_jump(f2) || is_jump(f3);
	}
	void hooks::queue_dependency(void* dependency) {
		if (is_unwanted_dependency((__int64)dependency)) {
			return;
		}
		return g_hooking->m_queueDependencyHk.getOg<decltype(&queue_dependency)>()(dependency);
	}
	LRESULT hooks::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		g_renderer->wndProc(hWnd, msg, wParam, lParam);
		return CallWindowProcW(g_hooking->m_ogWndProc, hWnd, msg, wParam, lParam);
	}
	HRESULT hooks::present(IDXGISwapChain* dis, UINT syncInterval, UINT flags) {
		g_renderer->present();
		return g_hooking->m_dxHk.get_original<decltype(&present)>(8)(dis, syncInterval, flags);
	}
	HRESULT hooks::resizeBuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
		ImGui_ImplDX11_InvalidateDeviceObjects();
		auto hr = g_hooking->m_dxHk.get_original<decltype(&resizeBuffers)>(13)(dis, bufferCount, width, height, newFormat, swapChainFlags);
		if (SUCCEEDED(hr))
			ImGui_ImplDX11_CreateDeviceObjects();
		return hr;
	};
	LPVOID hooks::convertThreadToFiber(LPVOID param) {
		if (IsThreadAFiber())
			return GetCurrentFiber();
		return g_hooking->m_convertThreadToFiberHk.getOg<decltype(&convertThreadToFiber)>()(param);
	}
	auto kernel32Handle = GetModuleHandleA("kernel32.dll");
	hooking::hooking() :
		m_dxHk(*g_pointers->m_swapchain, 19),
		m_ScriptVirtualMachineHk("SVM", g_pointers->m_scriptVirtualMachine, &hooks::script_vm),
		m_InitNativeTablesHk("INT", g_pointers->m_init_native_tables, &hooks::init_native_tables),
		//m_getModelInfoHk("GMI", g_pointers->m_getModelInfo, &hooks::get_model_info),
		m_assignPhysicalIndexHk("API", g_pointers->m_assignPhysicalIndex, &hooks::assign_physical_index),
		m_receiveNetMessageHk("RNM", g_pointers->m_receiveNetMessage, &hooks::receive_net_message),
		m_receiveEventHk("RE", g_pointers->m_receivedEvent, &hooks::received_event),
		m_networkPlayerMgrInitHk("NPMI", g_pointers->m_network_player_mgr_init, &hooks::network_player_mgr_init),
		m_networkPlayerMgrShutdownHk("NPMS", g_pointers->m_network_player_mgr_shutdown, &hooks::network_player_mgr_shutdown),
		m_taskJumpConstructorHk("TJC", g_pointers->m_taskJumpConstructor, &hooks::task_jump_constructor),
		m_taskFallConstructorHk("TFC", g_pointers->m_fallTaskConstructor, &hooks::FallTaskConstructor),
		m_writePlayerGameStateDataNodeHk("WPGSDN", g_pointers->m_writePlayerGameStateDataNode, &hooks::write_player_game_state_data_node),
		m_writePlayerCameraDataNodeHk("WPCDN", g_pointers->m_writePlayerCameraDataNode, &hooks::write_player_camera_data_node),
		m_writeVehicleProximityMigrationDataNodeHk("WVPMDN", g_pointers->m_write_vehicle_proximity_migration_data_node, &hooks::write_vehicle_proximity_migration_data_node),
		m_getNetworkEventDataHk("GNED", g_pointers->m_get_network_event_data, &hooks::get_network_event_data),
		//Protections
		m_receivedCloneCreateHk("RCC", g_pointers->m_received_clone_create, &hooks::received_clone_create),
		m_receivedCloneSyncHk("RCS", g_pointers->m_received_clone_sync, &hooks::received_clone_sync),
		m_canApplyDataHk("CAD", g_pointers->m_can_apply_data, &hooks::can_apply_data),
		m_invalidModsCrashDetourHk("IMCD", g_pointers->m_invalid_mods_crash_detour, &hooks::invalid_mods_crash_detour),
		m_invalidDecalHk("ID", g_pointers->m_invalid_decal_crash, &hooks::invalid_decal),
		m_taskParachuteObject0x270Hk("TPO0X270", g_pointers->m_task_parachute_object_0x270, &hooks::task_parachute_object_0x270),
		m_serializeTakeOffPedVariationTaskHk("STOPVT", g_pointers->m_serialize_take_off_ped_variation_task, &hooks::serialize_take_off_ped_variation_task),
		m_fragmentPhysicsCrash2Hk("FPC2", g_pointers->m_fragment_physics_crash_2, &hooks::fragment_physics_crash_2),
		//Others
		m_queueDependencyHk("QD", g_pointers->m_queue_dependency, &hooks::queue_dependency),
		m_convertThreadToFiberHk("CTTF", GetProcAddress(kernel32Handle, "ConvertThreadToFiber"), &hooks::convertThreadToFiber)
	{
		m_dxHk.hook(8, &hooks::present);
		m_dxHk.hook(13, &hooks::resizeBuffers);
	}
	hooking::~hooking() {
		m_dxHk.unhook(8);
		m_dxHk.unhook(13);
	}
	void hooking::hook() {
		m_ogWndProc = (WNDPROC)SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, (LONG_PTR)&hooks::wndProc);
		m_dxHk.enable();
		m_ScriptVirtualMachineHk.enable();
		m_InitNativeTablesHk.enable();
		//m_getModelInfoHk.enable();
		m_assignPhysicalIndexHk.enable();
		m_receiveNetMessageHk.enable();
		m_receiveEventHk.enable();
		m_getNetworkEventDataHk.enable();
		m_networkPlayerMgrInitHk.enable();
		m_networkPlayerMgrShutdownHk.enable();
		m_taskJumpConstructorHk.enable();
		m_taskFallConstructorHk.enable();
		m_writePlayerGameStateDataNodeHk.enable();
		m_writePlayerCameraDataNodeHk.enable();
		m_writeVehicleProximityMigrationDataNodeHk.enable();
		m_receivedCloneCreateHk.enable();
		m_receivedCloneSyncHk.enable();
		m_canApplyDataHk.enable();
		// m_invalidModsCrashDetourHk.enable(); crashes?
		m_invalidDecalHk.enable();
		m_taskParachuteObject0x270Hk.enable();
		m_serializeTakeOffPedVariationTaskHk.enable();
		m_fragmentPhysicsCrash2Hk.enable();
		m_queueDependencyHk.enable();
		m_convertThreadToFiberHk.enable();
		MH_ApplyQueued();
	}
	void hooking::unhook() {
		SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_ogWndProc);
		m_dxHk.disable();
		m_ScriptVirtualMachineHk.disable();
		m_InitNativeTablesHk.disable();
		//m_getModelInfoHk.disable();
		m_assignPhysicalIndexHk.disable();
		m_receiveNetMessageHk.disable();
		m_receiveEventHk.disable();
		m_getNetworkEventDataHk.disable();
		m_networkPlayerMgrInitHk.disable();
		m_networkPlayerMgrShutdownHk.disable();
		m_taskJumpConstructorHk.disable();
		m_taskFallConstructorHk.disable();
		m_writePlayerGameStateDataNodeHk.disable();
		m_writePlayerCameraDataNodeHk.disable();
		m_writeVehicleProximityMigrationDataNodeHk.disable();
		m_receivedCloneCreateHk.disable();
		m_receivedCloneSyncHk.disable();
		m_canApplyDataHk.disable();
		// m_invalidModsCrashDetourHk.disable();
		m_invalidDecalHk.disable();
		m_taskParachuteObject0x270Hk.disable();
		m_serializeTakeOffPedVariationTaskHk.disable();
		m_fragmentPhysicsCrash2Hk.disable();
		m_queueDependencyHk.disable();
		m_convertThreadToFiberHk.disable();
		MH_ApplyQueued();
	}
}