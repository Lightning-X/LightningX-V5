#include "Hooking.hpp"
#include "GTAV-Classes-master/netsync/nodes/player/CPlayerCameraDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/player/CPlayerGameStateDataNode.hpp"
namespace base {
	void hooks::write_player_camera_data_node(rage::netObject* player, CPlayerCameraDataNode* node) {
		g_hooking->m_writePlayerCameraDataNodeHk.getOg<decltype(&write_player_camera_data_node)>()(player, node);
		if (features::spoof_hide_spectate && features::SpectateBool || features::Freecambool) {
			node->m_free_cam_pos_x += 50.0f;
			node->m_free_cam_pos_y -= 50.0f;
			node->m_camera_x -= 50.0f;
		}
	}
}