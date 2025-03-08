#pragma once
#include "Common.hpp"
#include "GTAV-Classes-master/player/CPlayerInfo.hpp"
#include "GTAV-Classes-master/socialclub/FriendRegistry.hpp"
#include "GTAV-Classes-master/socialclub/ScInfo.hpp"
#include "GTAV-Classes-master/network/CNetworkPlayerMgr.hpp"
#include "GTAV-Classes-master/ped/CPedFactory.hpp"
#include "GTAV-Classes-master/script/GtaThread.hpp"
#include "GTAV-Classes-master/rage/rlSessionByGamerTaskResult.hpp"
#include "GTAV-Classes-master/network/Network.hpp"
#include "GTAV-Classes-master/script/scrProgramTable.hpp"
#include "GTAV-Classes-master/network/CCommunications.hpp"
#include "BytePatch.hpp"
#include "CReplyInterface.hpp"
#include "GTAV-Classes-master/network/RemoteGamerInfoMsg.hpp"
#include "GTAV-Classes-master/netsync/nodes/player/CPlayerGameStateDataNode.hpp"
#include "GTAV-Classes-master/rage/rlTaskStatus.hpp"
#include "Memory.hpp"
#include "NetObjectMgr.hpp"
#include "GTAV-Classes-master/netsync/nodes/vehicle/CVehicleGadgetDataNode.hpp"
#include "GTAV-Classes-master/script/types.hpp"
#include "GTAV-Classes-master/network/netTime.hpp"
#include "NetArray.hpp"
#include "GTAV-Classes-master/script/scriptHandler.hpp"
namespace rage {
	struct scrNativeRegistrationTable;
	struct scrNativeCallContext;
	using scrNativeHandler = void(*)(scrNativeCallContext*);
	class netEventMgr;
	class netConnectionPeer;
}
namespace funcTypes {
	using scriptVirtualMachineT = rage::eThreadState(*)(rage::scrValue* stack, rage::scrValue** globals, rage::scrProgram* program, rage::scrThread::Serialised* ser);
	using receivedEventT = void(*)(rage::netEventMgr* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, int buffer_size, rage::datBitBuffer* buffer);
	using getNativeHandlerT = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable*, std::uint64_t);
	using fixVectorsT = void(*)(rage::scrNativeCallContext*);
	using triggerScriptEventT = void(*)(int event_group, std::int64_t* args, int arg_count, int player_bits);
	using getNetPlayerT = CNetGamePlayer * (*)(std::int32_t playerID);
	using getPlayerNameT = const char* (*)(std::int32_t playerID);
	using pointerToHandleT = std::int32_t(*)(void* ptr);
	using handleToPointerT = rage::CDynamicEntity* (*)(std::int32_t);
	using requestControlT = void (*)(rage::netObject* net_object);
	using request_ragdollT = void (*)(uint16_t object_id);
	using getGameplayCamCoordsT = Vector3(*)();
	using getScreenCoordsForWorldCoordsT = bool(*)(float* worldCoords, float* outX, float* outY);
	using getSessionByGamerHandleT = bool(*)(int metricMgr, rage::rlGamerHandle* handles, int count, rage::rlSessionByGamerTaskResult* result, int unk, bool* success, rage::rlTaskStatus* state);
	using joinSessionByInfoT = bool(*)(Network* network, rage::rlSessionInfo* info, int unk, int flags, rage::rlGamerHandle* handles, int handlecount);
	using sendInviteAcceptedPresenceEventT = void(*)(void* presenceStruct, rage::rlPresenceEventInviteAccepted* invite, int flags);
	using readBitbufDwordT = bool(*)(rage::datBitBuffer* buffer, PVOID read, int bits);
	using readBitbufArrayT = bool(*)(rage::datBitBuffer* buffer, PVOID read, int bits, int unk);
	using readBitbufStringT = bool (*)(rage::datBitBuffer* buffer, char* read, int bits);
	using readBitbufBoolT = bool (*)(rage::datBitBuffer* buffer, bool* read, int bits);
	using writeBitbufQwordT = bool (*)(rage::datBitBuffer* buffer, uint64_t val, int bits);
	using writeBitbufDwordT = bool (*)(rage::datBitBuffer* buffer, uint32_t val, int bits);
	using writeBitbufInt64T = bool (*)(rage::datBitBuffer* buffer, int64_t val, int bits);
	using writeBitbufInt32T = bool (*)(rage::datBitBuffer* buffer, int32_t val, int bits);
	using writeBitbufBoolT = bool (*)(rage::datBitBuffer* buffer, bool val, int bits);
	using writeBitbufArrayT = bool (*)(rage::datBitBuffer* buffer, void* val, int bits, int unk);
	using sendEventAckT = void(*)(rage::netEventMgr* eventMgr, CNetGamePlayer* source, CNetGamePlayer* target, int idx, int handledBitset);
	using generate_uuidT = bool(*)(std::uint64_t* uuid);
	using getGxtLabelFromTableT = const char* (*)(void* _This, const char* label);
	using getJoaatedGxtLabelFromTableT = const char* (*)(void* _This, rage::joaat_t hash);
	using getConnectionPeerT = netConnectionPeer * (*)(rage::netConnectionManager* manager, int peer_id);
	using sendRemoveGamerCmdT = void(*)(rage::netConnectionManager* net_connection_mgr, netConnectionPeer* player, int connection_id, rage::snMsgRemoveGamersFromSessionCmd* cmd, int flags);
	using clearPedTasksNetworkT = void(*)(CPed* ped, bool immediately);
	using migrateObjectT = void (*)(CNetGamePlayer* player, rage::netObject* object, int type);
	using writePlayerGameStateDataNodeT = bool(*)(rage::netObject* plr, CPlayerGameStateDataNode* node);
	using taskJumpConstructorT = __int64 (*)(uint64_t _This, int Flags);
	using FallTaskConstructorT = void* (*)(uint64_t _This, uint32_t Flags);
	using handleRemoveGamerCmdT = void* (*)(rage::snSession* session, rage::snPlayer* origin, rage::snMsgRemoveGamersFromSessionCmd* cmd);
	//Sync signatures START
	using get_sync_type_infoT = const char* (*)(uint16_t sync_type, char a2);
	using get_sync_tree_for_typeT = rage::netSyncTree* (*)(CNetworkObjectMgr* mgr, uint16_t sync_type);
	using get_net_objectT = rage::netObject* (*)(CNetworkObjectMgr* mgr, int16_t id, bool can_delete_be_pending);
	using get_net_object_for_playerT = rage::netObject* (*)(CNetworkObjectMgr*, int16_t, CNetGamePlayer*, bool);
	using read_bitbuffer_into_sync_treeT = void (*)(rage::netSyncTree* tree, uint64_t flag, uint32_t flag2, rage::datBitBuffer* buffer, uint64_t netLogStub);
	//Sync signatures END
	using get_entity_attached_toT = rage::CDynamicEntity* (*)(rage::CDynamicEntity* entity);
	using reset_network_complaintsT = void (*)(CNetComplaintMgr* mgr);
	using send_network_damageT = void (*)(rage::CEntity* source, rage::CEntity* target, rage::fvector3* position, int hit_component, bool override_default_damage, int weapon_type, float override_damage, int tire_index, int suspension_index, int flags, std::uint32_t action_result_hash, std::int16_t action_result_id, int action_unk, bool hit_weapon, bool hit_weapon_ammo_attachment, bool silenced, bool unk, rage::fvector3* impact_direction);
	//using get_next_carriageT = void* (*)(void* carriage);
	//using get_vehicle_gadget_array_sizeT = int (*)(eVehicleGadgetType type);
	using get_host_array_handler_by_indexT = rage::netArrayHandlerBase* (*)(rage::CGameScriptHandlerNetComponent* component, int index);
}
namespace base {
	class pointers {
	public:
		explicit pointers();
		~pointers();
		funcTypes::scriptVirtualMachineT m_scriptVirtualMachine{};
		funcTypes::receivedEventT m_receivedEvent{};
		funcTypes::getNativeHandlerT m_getNativeHandler{};
		funcTypes::fixVectorsT m_fixVectors{};
		funcTypes::triggerScriptEventT m_triggerScriptEvent{};
		funcTypes::getNetPlayerT m_getNetPlayer{};
		funcTypes::getPlayerNameT m_getPlayerName{};
		funcTypes::pointerToHandleT m_pointerToHandle{};
		funcTypes::handleToPointerT m_handleToPointer{};
		funcTypes::requestControlT m_requestControl{};
		funcTypes::request_ragdollT m_request_ragdoll;
		funcTypes::getGameplayCamCoordsT m_getGameplayCamCoords{};
		funcTypes::getScreenCoordsForWorldCoordsT m_getScreenCoordsForWorldCoords{};
		funcTypes::getSessionByGamerHandleT m_getSessionByGamerHandle{};
		funcTypes::joinSessionByInfoT m_joinSessionByInfo{};
		funcTypes::sendInviteAcceptedPresenceEventT m_sendInviteAcceptedPresenceEvent{};
		funcTypes::readBitbufDwordT m_readBitbufDword{};
		funcTypes::readBitbufArrayT m_readBitbufArray{};
		funcTypes::readBitbufBoolT m_readBitbufBool{};
		funcTypes::readBitbufStringT m_readBitbufString{};
		funcTypes::writeBitbufQwordT m_writeBitbufQword{};
		funcTypes::writeBitbufDwordT m_writeBitbufDword{};
		funcTypes::writeBitbufInt64T m_writeBitbufInt64{};
		funcTypes::writeBitbufInt32T m_writeBitbufInt32{};
		funcTypes::writeBitbufBoolT m_writeBitbufBool{};
		funcTypes::writeBitbufArrayT m_writeBitbufArray{};
		funcTypes::sendEventAckT m_sendEventAck{};
		funcTypes::generate_uuidT m_generateUuid{};
		funcTypes::getGxtLabelFromTableT m_getGxtLabelFromTable{};
		funcTypes::getJoaatedGxtLabelFromTableT m_getJoaatedGxtLabelFromTable{};
		funcTypes::getConnectionPeerT m_getConnectionPeer{};
		funcTypes::sendRemoveGamerCmdT m_sendRemoveGamerCmd{};
		funcTypes::clearPedTasksNetworkT m_clearPedTasksNetwork{};
		funcTypes::writePlayerGameStateDataNodeT m_writePlayerGameStateDataNode{};
		funcTypes::migrateObjectT m_migrateObject{};
		funcTypes::taskJumpConstructorT m_taskJumpConstructor;
		funcTypes::FallTaskConstructorT m_fallTaskConstructor;
		funcTypes::handleRemoveGamerCmdT m_handleRemoveGamerCmd{};
		funcTypes::reset_network_complaintsT m_reset_network_complaints{};
		funcTypes::send_network_damageT m_send_network_damage;
		//Sync Signatures START
		PVOID m_received_clone_create{};
		PVOID m_received_clone_sync{};
		PVOID m_can_apply_data{};
		funcTypes::get_sync_tree_for_typeT m_get_sync_tree_for_type{};
		funcTypes::get_sync_type_infoT m_get_sync_type_info{};
		funcTypes::get_net_objectT m_get_net_object{};
		funcTypes::read_bitbuffer_into_sync_treeT m_read_bitbuffer_into_sync_tree{};
		//Sync Signatures END
		funcTypes::get_entity_attached_toT m_get_entity_attached_to{};
		funcTypes::get_host_array_handler_by_indexT m_get_host_array_handler_by_index;
		//funcTypes::get_next_carriageT m_get_next_carriage{};
		//funcTypes::get_vehicle_gadget_array_sizeT m_get_vehicle_gadget_array_size{};
		rage::atArray<GtaThread*>* m_gtaThreads{};
		void* m_gxtLabels{};
		void* m_presecneStruct{};
		bool* m_isSessionActive{};
		const char* m_game_version{};
		const char* m_online_version{};
		PVOID m_receiveNetMessage{};
		PVOID m_get_network_event_data{};
		PVOID m_assignPhysicalIndex{};
		PVOID m_network_player_mgr_init{};
		PVOID m_network_player_mgr_shutdown{};
		PVOID m_getModelInfo{};
		PVOID m_writePlayerCameraDataNode{};
		PVOID m_queue_dependency{};
		PVOID m_interval_check_func{};
		PVOID m_init_native_tables{};
		PVOID m_invalid_mods_crash_detour{};
		PVOID m_invalid_decal_crash{};
		PVOID m_task_parachute_object_0x270{};
		PVOID m_fragment_physics_crash_2{};
		PVOID m_model_spawn_bypass{};
		//PVOID m_infinite_train_crash{};
		//PVOID m_serialize_ped_inventory_data_node{};
		//PVOID m_serialize_vehicle_gadget_data_node{};
		PVOID m_serialize_take_off_ped_variation_task{};
		PVOID m_write_vehicle_proximity_migration_data_node{};
		//PVOID* m_presenceData{};
		Network** m_network{};
		rage::netTime** m_network_time;
		eGameState* m_gameState{};
		CPedFactory** m_pedFactory{};
		CNetworkPlayerMgr** m_netPlayerMgr{};
		CNetworkObjectMgr** m_network_object_mgr{};
		FriendRegistry* m_friendRegistry{};
		ScInfo* m_socialClubInfo{};
		CReplayInterface** m_replayInterface{};
		uint32_t* m_frameCount{};
		IDXGISwapChain** m_swapchain{};
		rage::scrNativeRegistrationTable* m_nativeRegTbl{};
		HashTable<CBaseModelInfo*>* m_modelTable{};
		rage::scrProgramTable* m_scriptProgramTable{};
		rage::scrValue** m_globalBase{};
		std::uint64_t* m_host_token{};
		rage::rlGamerInfo* m_profile_gamer_info{};     // per profile gamer info
		rage::rlGamerInfo* m_player_info_gamer_info{}; // the gamer info that is applied to CPlayerInfo
		CCommunications** m_communications{};
		HWND m_hwnd{};
		//BytePatches
		mem m_is_matchmaking_session_valid{};
		mem m_broadcast_patch{};
		//mem m_ntqvm_caller{};
		mem m_sound_overload_detour{};
		mem m_disable_collision{};
		mem m_crash_trigger{};
		mem m_max_wanted_level{};
		mem m_blame_explode{};
		mem m_explosion_patch{};
		mem m_script_vm_patch_1{};
		mem m_script_vm_patch_2{};
		mem m_script_vm_patch_3{};
		mem m_script_vm_patch_4{};
		mem m_script_vm_patch_5{};
		mem m_script_vm_patch_6{};
		PVOID m_free_event_error{};
	};
	inline pointers* g_pointers{};
}