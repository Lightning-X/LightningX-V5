#include "Hooking.hpp"
#include "Notify.hpp"
#include "Translations.hpp"
namespace base
{
	bool hooks::received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, int32_t object_id, int32_t object_flag, rage::datBitBuffer* buffer, int32_t timestamp) {
		if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN) {
			MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("OUT_OF_BOUNDS_OBJECT_TYPE"), src->GetName());
			return true;
		}
		auto plyr = g_player_service->get_by_id(src->m_player_id);
		if (plyr && plyr->block_clone_create)
			return true;
		m_syncing_player = src;
		return g_hooking->m_receivedCloneCreateHk.getOg<decltype(&received_clone_create)>()(mgr, src, dst, object_type, object_id, object_flag, buffer, timestamp);
	}
}