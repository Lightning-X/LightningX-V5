#include "hooking.hpp"
#include "Notify.hpp"
#include "Translations.hpp"

namespace base
{
	eAckCode hooks::received_clone_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, uint16_t object_id, rage::datBitBuffer* buffer, uint16_t unk, uint32_t timestamp) {
		if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN) {
			MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("OUT_OF_BOUNDS_OBJECT_TYPE"), src->GetName());
			return eAckCode::ACKCODE_FAIL;
		}
		if (auto net_obj = g_pointers->m_get_net_object(mgr, object_id, true); !net_obj || net_obj->m_object_type != (int16_t)object_type) {
			MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INCORRECT_OBJECT_TYPE"), src->GetName());
			return eAckCode::ACKCODE_FAIL;
		}
		auto plyr = g_player_service->get_by_id(src->m_player_id);
		if (plyr && plyr->block_clone_create)
			return eAckCode::ACKCODE_FAIL;
		m_syncing_player = src;
		return g_hooking->m_receivedCloneSyncHk.getOg<decltype(&received_clone_sync)>()(mgr, src, dst, object_type, object_id, buffer, unk, timestamp);
	}
}