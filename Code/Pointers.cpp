#include "Pointers.hpp"
#include "Memory.hpp"
#include "Invoker.hpp"
#define printSig(ptr) //g_logger->send(logColor::white, "Debug", stringifyFn(ptr) " GTA5.exe+0x{:X}", (uintptr_t)ptr - uintptr_t(GetModuleHandleA(NULL)))
#define printSigNonRelative(ptr) //g_logger->send(logColor::white, "Debug", stringifyFn(ptr) " 0x{:X}", (uintptr_t)ptr)

namespace base {
	pointers::pointers() {
		static patternBatch mainBatch{};
		auto start = std::chrono::steady_clock::now();
		//MAIN
		mainBatch.add("GS", "83 3D ? ? ? ? ? 75 17 8B 43 20 25", [this](mem ptr) {
			m_gameState = ptr.add(2).rip().add(1).as<decltype(m_gameState)>();
			});
		mainBatch.add("SC", "48 8B 3D ? ? ? ? 3B C8", [this](mem ptr) {
			m_swapchain = ptr.add(3).rip().as<decltype(m_swapchain)>();
			});
		mainBatch.add("SVM", "4A 89 3C 39", [this](mem ptr) {
			m_scriptVirtualMachine = ptr.sub(0x26).rip().as<decltype(m_scriptVirtualMachine)>();
			});
		mainBatch.add("FC", "8B 15 ? ? ? ? 41 FF CF", [this](mem ptr) {
			m_frameCount = ptr.add(2).rip().as<decltype(m_frameCount)>();
			});
		mainBatch.add("STR", "F5 8B FD 73", [this](mem ptr) {
			m_gtaThreads = ptr.add(8).rip().as<decltype(m_gtaThreads)>();
			});
		mainBatch.add("NRT/GNH", "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A", [this](mem ptr) {
			m_nativeRegTbl = ptr.add(3).rip().as<decltype(m_nativeRegTbl)>();
			m_getNativeHandler = ptr.add(12).rip().as<decltype(m_getNativeHandler)>();
			m_init_native_tables = ptr.sub(37).as<decltype(m_init_native_tables)>();
			});
		mainBatch.add("FV", "83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA", [this](mem ptr) {
			m_fixVectors = ptr.as<decltype(m_fixVectors)>();
			});
		mainBatch.add("PTH", "48 8B F9 48 83 C1 10 33 DB", [this](mem ptr) {
			m_pointerToHandle = ptr.sub(0x15).as<decltype(m_pointerToHandle)>();
			});
		mainBatch.add("HTP", "83 F9 FF 74 31 4C 8B 0D", [this](mem ptr) {
			m_handleToPointer = ptr.as<decltype(m_handleToPointer)>();
			});
		mainBatch.add("TSE", "45 8B F0 41 8B F9 48 8B EA", [this](mem ptr) {
			m_triggerScriptEvent = ptr.sub(0x1C).as<decltype(m_triggerScriptEvent)>();
			});
		//SCREEN
		mainBatch.add("GSCFWC", "E8 ? ? ? ? 84 C0 74 19 F3 0F 10 44 24", [this](mem ptr) {
			m_getScreenCoordsForWorldCoords = ptr.add(1).rip().as<decltype(m_getScreenCoordsForWorldCoords)>();
			});
		mainBatch.add("GGCC", "8B 90 ? ? ? ? 89 13", [this](mem ptr) {
			m_getGameplayCamCoords = ptr.sub(0xE).as<decltype(m_getGameplayCamCoords)>();
			});
		//BITBUFFERS
		mainBatch.add("RBD", "48 89 74 24 ? 57 48 83 EC 20 48 8B D9 33 C9 41 8B F0 8A", [this](mem ptr) {
			m_readBitbufDword = ptr.sub(5).as<decltype(m_readBitbufDword)>();
			});
		mainBatch.add("RBA", "48 89 5C 24 ? 57 48 83 EC 30 41 8B F8 4C", [this](mem ptr) {
			m_readBitbufArray = ptr.as<decltype(m_readBitbufArray)>();
			});
		mainBatch.add("RBS", "48 89 5C 24 08 48 89 6C 24 18 56 57 41 56 48 83 EC 20 48 8B F2 45", [this](mem ptr) {
			m_readBitbufString = ptr.as<decltype(m_readBitbufString)>();
			});
		mainBatch.add("RBB", "E8 ? ? ? ? 84 C0 74 41 48 8D 56 2C", [this](mem ptr) {
			m_readBitbufBool = ptr.add(1).rip().as<decltype(m_readBitbufBool)>();
			});
		mainBatch.add("WBD", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 8B EA BF 01", [this](mem ptr) {
			m_writeBitbufDword = ptr.as<decltype(m_writeBitbufDword)>();
			});
		mainBatch.add("WBQ", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B EA 48 8B D9 41 83 F8 20", [this](mem ptr) {
			m_writeBitbufQword = ptr.as<decltype(m_writeBitbufQword)>();
			});
		mainBatch.add("WBI64", "E8 ? ? ? ? 8A 53 39 48 8B CF", [this](mem ptr) {
			m_writeBitbufInt64 = ptr.add(1).rip().as<decltype(m_writeBitbufInt64)>();
			});
		mainBatch.add("WBI32", "E8 ? ? ? ? 8A 53 74", [this](mem ptr) {
			m_writeBitbufInt32 = ptr.add(1).rip().as<decltype(m_writeBitbufInt32)>();
			});
		mainBatch.add("WBB", "E8 ? ? ? ? 8A 57 39", [this](mem ptr) {
			m_writeBitbufBool = ptr.add(1).rip().as<decltype(m_writeBitbufBool)>();
			});
		mainBatch.add("WBA", "E8 ? ? ? ? 01 7E 08", [this](mem ptr) {
			m_writeBitbufArray = ptr.add(1).rip().as<decltype(m_writeBitbufArray)>();
			});
		mainBatch.add("SEA", "48 8B DA 44 8B CD", [this](mem ptr) {
			m_sendEventAck = ptr.add(0x14).rip().as<decltype(m_sendEventAck)>();
			});
		//PED && VEHICLES
		mainBatch.add("GMI", "41 3B 0A 74 54", [this](mem ptr) {
			m_getModelInfo = ptr.sub(0x2E).as<decltype(m_getModelInfo)>();
			});
		mainBatch.add("PF", "48 8B 05 ? ? ? ? 8A D1", [this](mem ptr) {
			m_pedFactory = ptr.add(3).rip().as<decltype(m_pedFactory)>();
			});
		mainBatch.add("MHT", "4C 03 05 ? ? ? ? EB 03", [this](mem ptr) {
			m_modelTable = ptr.add(3).rip().as<decltype(m_modelTable)>();
			});
		mainBatch.add("TJC", "48 89 5C 24 ? 89 54 24 10 57 48 83 EC 30 0F 29 74 24", [this](mem ptr) {
			m_taskJumpConstructor = ptr.as<decltype(m_taskJumpConstructor)>();
			});
		mainBatch.add("TFC", "E8 ? ? ? ? B3 04 08 98 A0", [this](mem ptr) {
			m_fallTaskConstructor = ptr.add(1).rip().as<decltype(m_fallTaskConstructor)>();
			});
		mainBatch.add("WVPMDN", "48 89 4C 24 08 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 68 4C 8B A9", [this](mem ptr) {
			m_write_vehicle_proximity_migration_data_node = ptr.as<decltype(m_write_vehicle_proximity_migration_data_node)>();
			});
		//NETWORK
		mainBatch.add("GB", "48 8B 8D ? ? ? ? 4C 8D 4D 08", [this](mem ptr) {
			m_globalBase = ptr.add(0xE).rip().as<decltype(m_globalBase)>();
			});
		mainBatch.add("HRGC", "48 85 D2 0F 84 0E 04", [this](mem ptr) {
			m_handleRemoveGamerCmd = ptr.as<decltype(m_handleRemoveGamerCmd)>();
			});
		mainBatch.add("API", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 41 8A E8", [this](mem ptr) {
			m_assignPhysicalIndex = ptr.as<decltype(m_assignPhysicalIndex)>();
			});
		mainBatch.add("RCC", "48 8B C4 66 44 89 48", [this](mem ptr) {
			m_received_clone_create = ptr.as<decltype(m_received_clone_create)>();
			});
		mainBatch.add("CAD", "E8 ? ? ? ? 84 C0 0F 84 AF 01 00 00 48 8B 03", [this](mem ptr) {
			m_can_apply_data = ptr.add(1).rip().as<decltype(m_can_apply_data)>();
			});
		mainBatch.add("RCS/GSTFT/GNOFP/GNO/GSTI", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 40 4C 8B EA", [this](mem ptr) {
			m_received_clone_sync = ptr.as<decltype(m_received_clone_sync)>();
			m_get_sync_tree_for_type = ptr.add(0x2F).add(1).rip().as<decltype(m_get_sync_tree_for_type)>();
			m_get_net_object = ptr.add(0x109).add(1).rip().as<decltype(m_get_net_object)>();
			m_get_sync_type_info = ptr.add(0x11F).add(1).rip().as<decltype(m_get_sync_type_info)>();
			});
		mainBatch.add("NPMI", "41 56 48 83 EC ? 48 8B F1 B9 ? ? ? ? 49 8B F9 41 8B E8 4C 8B F2 E8", [this](mem ptr) {
			m_network_player_mgr_init = ptr.sub(0x13).as<decltype(m_network_player_mgr_init)>();
			});
		mainBatch.add("NPMS", "48 8D 9F ? ? ? ? EB ? 48 8B 13 48 85 D2 74 ? 48 8B CB E8 ? ? ? ? 48 83 7B ? ? 75 ? 48 8D 9F", [this](mem ptr) {
			m_network_player_mgr_shutdown = ptr.sub(0x1A).as<decltype(m_network_player_mgr_shutdown)>();
			});
		mainBatch.add("CNOM", "48 8B 0D ? ? ? ? 45 33 C0 E8 ? ? ? ? 33 FF 4C 8B F0", [this](mem ptr) {
			m_network_object_mgr = ptr.add(3).rip().as<decltype(m_network_object_mgr)>();
			});
		mainBatch.add("GEAT", "48 83 EC 28 48 8B 51 50 48 85 D2 74 04", [this](mem ptr) {
			m_get_entity_attached_to = ptr.as<decltype(m_get_entity_attached_to)>();
			});
		mainBatch.add("CFR", "41 8B F4 3B C5", [this](mem ptr) {
			m_friendRegistry = ptr.sub(9).rip().as<decltype(m_friendRegistry)>();
			});
		mainBatch.add("SCI", "48 8D 05 ? ? ? ? 48 63 D1", [this](mem ptr) {
			m_socialClubInfo = ptr.add(3).rip().as<decltype(m_socialClubInfo)>();
			});
		mainBatch.add("GCP&SRGC", "8D 42 FF 83 F8 FD 77 3D", [this](mem ptr) {
			m_getConnectionPeer = ptr.add(0x17).rip().as<decltype(m_getConnectionPeer)>();
			m_sendRemoveGamerCmd = ptr.add(0x41).rip().as<decltype(m_sendRemoveGamerCmd)>();
			});
		mainBatch.add("FEE", "48 8B 5C 24 40 48 8B 6C 24 48 48 8B 74 24 50 48 8B 7C 24 58 48 83 C4 30 41 5E C3 48 8B 0D", [this](mem ptr) {
			m_free_event_error = ptr.add(0x31).as<decltype(m_free_event_error)>();
			});
		mainBatch.add("SIAPE", "48 83 EC 38 F6 82", [this](mem ptr) {
			m_sendInviteAcceptedPresenceEvent = ptr.as<decltype(m_sendInviteAcceptedPresenceEvent)>();
			});
		mainBatch.add("RE", "66 41 83 F9 ? 0F 83", [this](mem ptr) {
			m_receivedEvent = ptr.as<decltype(m_receivedEvent)>();
			});
		mainBatch.add("RC", "E8 ? ? ? ? EB 3E 48 8B D3", [this](mem ptr) {
			m_requestControl = ptr.add(1).rip().as<decltype(m_requestControl)>();
			});
		mainBatch.add("RR", "E8 ? ? ? ? 09 B3 ? ? ? ? 48 8B 5C 24", [this](mem ptr) {
			m_request_ragdoll = ptr.add(1).rip().as<decltype(m_request_ragdoll)>();
			});
		mainBatch.add("SND", "E8 ? ? ? ? E9 E9 01 00 00 48 8B CB", [this](mem ptr) {
			m_send_network_damage = ptr.add(1).rip().as<decltype(m_send_network_damage)>();
			});
		mainBatch.add("RI", "0F B7 44 24 ? 66 89 44 4E", [this](mem ptr) {
			m_replayInterface = ptr.add(0x1F).rip().as<decltype(m_replayInterface)>();
			});
		mainBatch.add("JSBI", "E8 ? ? ? ? 0F B6 CB 84 C0 41 0F 44 CD", [this](mem ptr) {
			m_joinSessionByInfo = ptr.add(1).rip().as<decltype(m_joinSessionByInfo)>();
			});
		mainBatch.add("SPT", "48 8B 1D ? ? ? ? 41 83 F8 FF", [this](mem ptr) {
			m_scriptProgramTable = ptr.add(3).rip().as<decltype(m_scriptProgramTable)>();
			});
		mainBatch.add("GNP", "48 83 EC 28 33 C0 38 05 ? ? ? ? 74 0A", [this](mem ptr) {
			m_getNetPlayer = ptr.as<decltype(m_getNetPlayer)>();
			});
		mainBatch.add("PS", "48 8D 0D ? ? ? ? 8B F8 E8 ? ? ? ? 3B F8", [this](mem ptr) {
			m_presecneStruct = ptr.add(3).rip().as<decltype(m_presecneStruct)>();
			});
		mainBatch.add("GPN", "40 53 48 83 EC 20 80 3D ? ? ? ? ? 8B D9 74 22", [this](mem ptr) {
			m_getPlayerName = ptr.as<decltype(m_getPlayerName)>();
			});
		mainBatch.add("GU", "E8 ? ? ? ? 84 C0 74 0C 48 8B 44 24 ? 48 89 03", [this](mem ptr) {
			m_generateUuid = ptr.add(1).rip().as<decltype(m_generateUuid)>();
			});
		mainBatch.add("HT", "48 8B 05 ? ? ? ? 48 83 F8 FF", [this](mem ptr) {
			m_host_token = ptr.add(3).rip().as<decltype(m_host_token)>();
			});
		mainBatch.add("PGI", "48 8D 05 ? ? ? ? 48 8B FE", [this](mem ptr) {
			m_profile_gamer_info = ptr.add(3).rip().as<decltype(m_profile_gamer_info)>();
			});
		mainBatch.add("PIGI", "E8 ? ? ? ? 48 8D 4D 20 48 8B D0 E8 ? ? ? ? 41 8A CF", [this](mem ptr) {
			m_player_info_gamer_info = ptr.add(1).rip().add(3).rip().as<decltype(m_player_info_gamer_info)>();
			});
		mainBatch.add("C", "48 8B 1D ? ? ? ? 48 8D 4C 24 30", [this](mem ptr) {
			m_communications = ptr.add(3).rip().as<decltype(m_communications)>();
			});
		mainBatch.add("CPTN", "E8 ? ? ? ? EB 28 48 8B 8F A0 10 00 00", [this](mem ptr) {
			m_clearPedTasksNetwork = ptr.add(1).rip().as<decltype(m_clearPedTasksNetwork)>();
			});
		mainBatch.add("RENC", "E8 ? ? ? ? 8B 8B ? ? ? ? 03 CF", [this](mem ptr) {
			m_reset_network_complaints = ptr.add(1).rip().as<decltype(m_reset_network_complaints)>();
			});
		mainBatch.add("NT", "48 8B 0D ? ? ? ? E8 ? ? ? ? 33 DB 84 C0 74 41", [this](mem ptr) {
			m_network_time = ptr.add(3).rip().as<decltype(m_network_time)>();
			});
		mainBatch.add("MO", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 41 8B F8 48", [this](mem ptr) {
			m_migrateObject = ptr.as<decltype(m_migrateObject)>();
			});
		mainBatch.add("ISA", "40 38 35 ? ? ? ? 75 0E 4C 8B C3 49 8B D7 49 8B CE", [this](mem ptr) {
			m_isSessionActive = ptr.add(3).rip().as<decltype(m_isSessionActive)>();
			});
		mainBatch.add("NPM", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", [this](mem ptr) {
			m_netPlayerMgr = ptr.add(3).rip().as<decltype(m_netPlayerMgr)>();
			});
		mainBatch.add("GSBGH", "E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B 05 ? ? ? ? 48 8D 4C 24", [this](mem ptr) {
			m_getSessionByGamerHandle = ptr.add(1).rip().as<decltype(m_getSessionByGamerHandle)>();
			});
		mainBatch.add("N", "48 8B 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 84 C0 75 17 48 8B 0D ? ? ? ? 48 8B D7", [this](mem ptr) {
			m_network = ptr.add(3).rip().as<decltype(m_network)>();
			});
		mainBatch.add("IMCD", "E8 ? ? ? ? 40 88 7C 24 ? 49 89 9C 24", [this](mem ptr) {
			m_invalid_mods_crash_detour = ptr.add(1).rip().as<decltype(m_invalid_mods_crash_detour)>();
			});
		mainBatch.add("IDC", "E8 ? ? ? ? 8B 9C 24 B8 00 00 00 4C 8B AC 24 A8 00 00 00", [this](mem ptr) {
			m_invalid_decal_crash = ptr.add(1).rip().as<decltype(m_invalid_decal_crash)>();
			});
		mainBatch.add("TPO270", "0F 88 ? ? ? ? 75 34", [this](mem ptr) {
			m_task_parachute_object_0x270 = ptr.sub(6).as<decltype(m_task_parachute_object_0x270)>();
			});
		mainBatch.add("STOPVT", "40 55 53 57 41 56 48 8B EC 48 83 EC 68", [this](mem ptr) {
			m_serialize_take_off_ped_variation_task = ptr.as<decltype(m_serialize_take_off_ped_variation_task)>();
			});
		mainBatch.add("FPC2", "E8 ? ? ? ? 84 C0 75 0B 41 FF CF", [this](mem ptr) {
			m_fragment_physics_crash_2 = ptr.add(1).rip().as<decltype(m_fragment_physics_crash_2)>();
			});
		mainBatch.add("RNM", "48 83 EC 20 4C 8B 71 50 33 ED", [this](mem ptr) {
			m_receiveNetMessage = ptr.sub(0x19).as<decltype(m_receiveNetMessage)>();
			});
		mainBatch.add("GHAHBI", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 8A 81 8F", [this](mem ptr) {
			m_get_host_array_handler_by_index = ptr.as<decltype(m_get_host_array_handler_by_index)>();
			});
		mainBatch.add("CTSHP", "48 8B C8 FF 52 30 84 C0 74 05 48", [this](mem ptr) { //Model spawn bypass
			m_model_spawn_bypass = ptr.add(8).as<decltype(m_model_spawn_bypass)>();
			});
		//mainBatch.add("PD", "48 8D 05 ? ? ? ? 48 8B F1 48 89 01 48 83 C1 08 E8 ? ? ? ? 33 ED 48 8D 8E 68 5B 00 00", [this](mem ptr) {
		//	m_presenceData = ptr.add(3).rip().as<decltype(m_presenceData)>();
		//});
		//Joaats
		mainBatch.add("GNED", "53 43 52 49 50 54 5F 4E 45 54 57 4F 52 4B", [this](mem ptr) {
			m_get_network_event_data = *ptr.sub(0x38).as<PVOID*>();
			});
		mainBatch.add("GL", "48 8D 0D ? ? ? ? E8 ? ? ? ? 8B 0D ? ? ? ? 48 8B 5C 24", [this](mem ptr) {
			m_gxtLabels = ptr.add(3).rip().as<decltype(m_gxtLabels)>();
			});
		mainBatch.add("GGTFT", "E8 ? ? ? ? 48 8D 4B 0B", [this](mem ptr) {
			m_getGxtLabelFromTable = ptr.add(1).rip().as<decltype(m_getGxtLabelFromTable)>();
			});
		mainBatch.add("GJGLFT", "48 83 EC 28 E8 ? ? ? ? 48 85 C0 75 34 8B 0D ? ? ? ? 65 48 8B 04 25 ? ? ? ? BA ? ? ? ? 48 8B 04 C8 8B 0C 02 D1", [this](mem ptr) {
			m_getJoaatedGxtLabelFromTable = ptr.as<decltype(m_getJoaatedGxtLabelFromTable)>();
			});
		//DATANODES
		mainBatch.add("WPGSDN", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 30 0F B7 81", [this](mem ptr) {
			m_writePlayerGameStateDataNode = ptr.as<decltype(m_writePlayerGameStateDataNode)>();
			});
		mainBatch.add("WPCDN", "48 8B C4 48 89 58 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 B0 48 81 EC 50 01 00 00 4C", [this](mem ptr) {
			m_writePlayerCameraDataNode = ptr.as<decltype(m_writePlayerCameraDataNode)>();
			});
		//OTHER
		mainBatch.add("QD", "48 89 5C 24 ? 57 48 83 EC ? 0F B6 99", [this](mem ptr) {
			m_queue_dependency = ptr.as<decltype(m_queue_dependency)>();
			});
		mainBatch.add("ICF", "48 8D 0D ? ? ? ? 88 05 ? ? ? ? 48 8D 05", [this](mem ptr) {
			m_interval_check_func = ptr.add(3).rip().as<decltype(m_interval_check_func)>();
			});
		// game version + online version
		mainBatch.add("GVOV", "8B C3 33 D2 C6 44 24 20", [this](mem ptr) {
			m_game_version = ptr.add(0x24).rip().as<decltype(m_game_version)>();
			m_online_version = ptr.add(0x24).rip().add(0x20).as<decltype(m_online_version)>();
			});
		// Is Matchmaking Session Valid
		mainBatch.add("IMSV", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 45 0F", [this](mem ptr) {
			m_is_matchmaking_session_valid = ptr;
			});
		// Broadcast Net Array Patch
		mainBatch.add("BP", "74 73 FF 90 ? ? ? ? 8B D5 4C 8B 00 48 8B C8 41 FF 50 30", [this](mem ptr) {
			m_broadcast_patch = ptr;
			});
		// Blame Explosion
		mainBatch.add("BE", "0F 85 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 48 08 E8", [this](mem ptr) {
			m_blame_explode = ptr;
			});
		// Explosion Patch
		mainBatch.add("BE", "E8 ? ? ? ? 48 8D 4C 24 20 E8 ? ? ? ? 4C 8D 9C 24 80 01 00 00", [this](mem ptr) {
			m_explosion_patch = ptr;
			});
		// Sound Overload Detour
		mainBatch.add("SOD", "66 45 3B C1 74 38", [this](mem ptr) {
			m_sound_overload_detour = ptr;
			});
		// Crash Trigger
		mainBatch.add("CT", "48 3B F8 74 ? 8B 1D", [this](mem ptr) {
			m_crash_trigger = ptr;
			});
		// Max Wanted Lvl
		mainBatch.add("MWL", "8B 43 6C 89 05", [this](mem ptr) {
			m_max_wanted_level = ptr;
			});
		//VIRTUAL MACHINE
		mainBatch.add("SVM1", "3B 0A 0F 83 ? ? ? ? 48 FF C7", [this](mem ptr) {
			m_script_vm_patch_1 = ptr;
			});
		mainBatch.add("SVM2", "3B 0A 0F 83 ? ? ? ? 49 03 FA", [this](mem ptr) {
			m_script_vm_patch_2 = ptr;
			});
		mainBatch.add("SVM3&4", "3B 11 0F 83 ? ? ? ? 48 FF C7", [this](mem ptr) {
			m_script_vm_patch_3 = ptr;
			m_script_vm_patch_4 = ptr.add(0x1C);
			});
		mainBatch.add("SVM5&6", "3B 11 0F 83 ? ? ? ? 49 03 FA", [this](mem ptr) {
			m_script_vm_patch_5 = ptr;
			m_script_vm_patch_6 = ptr.add(0x26);
			});
		m_hwnd = FindWindowA("grcWindow", nullptr);
		if (!m_hwnd)
			throw std::runtime_error("Failed to find the game's window.");
		mainBatch.run();
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> diff = end - start;
		g_logger->send(logColor::white, "Signatures", "{} found and {} failed out of {} sigs. Scan time took {:2f}s with a per sig avg. of {:2f}s", g_foundSigCount, g_failedSigCount, g_totalSigCount, diff.count(), diff.count() / g_totalSigCount);
		g_pointers = this;
	}
	pointers::~pointers() {
		memory::byte_patch::restore_all();
		g_pointers = nullptr;
	}
}