#pragma once
#include "Hooking.hpp"
#include "Notify.hpp"
#include "Translations.hpp"
#include "Features.hpp"
#include "../GTAV-Classes-master/network/netPlayer.hpp"
#include "Services/Player/player_service.hpp"
#include "Services/Friend/friends_service.hpp"
#include "KicksAndCrashes.hpp"
namespace base {
	const inline ankerl::unordered_dense::segmented_set<std::uint64_t> Advertisers = { 212928513, 204133460, 175773823 };
	const inline ankerl::unordered_dense::segmented_set<std::uint64_t> admin_rids = { 12435, 63457, 216820, 6597634, 9284553, 10552062, 10833148, 13934382, 14870763, 16395773, 16395782, 16395801, 16395840, 16395850, 16396080, 16396091, 16396096, 16396107, 16396118, 16396126, 16396133, 16396141, 16396148, 16396157, 16396170, 16417718, 16439132, 18965281, 20158751, 20158753, 20158757, 20158759, 21088063, 21765545, 22577121, 22577440, 22577458, 23659342, 23659351, 23659353, 23659354, 24037237, 24646485, 25667546, 25695975, 26658154, 27691740, 28776717, 28823614, 29454165, 31586721, 39573295, 41352312, 46469110, 49770174, 50850475, 53309582, 54445058, 54462116, 54468359, 54815152, 54815524, 56176623, 56321667, 56583239, 56778561, 57233573, 60331599, 61522786, 62409944, 62739248, 64074298, 64234321, 64499496, 64624133, 65428266, 67241866, 69079775, 69325516, 69991900, 70527952, 70703467, 70841434, 74716313, 75632221, 76384414, 76385407, 77205006, 78229643, 78934592, 80527019, 81691532, 85593421, 88047835, 88435236, 88435319, 88435362, 88435916, 88439202, 88440582, 89288299, 89705641, 89705672, 89705758, 89730037, 89730175, 89730345, 89797943, 90580674, 91003708, 91031119, 93759248, 93759254, 93759280, 93759401, 93759425, 93759462, 93800162, 93800269, 94028919, 99453882, 99922796, 100641297, 102519620, 103054099, 103318524, 103814653, 104041189, 104213911, 104432921, 105474714, 105919953, 106480467, 107713060, 107713114, 111262316, 111377226, 111439945, 112362134, 113097651, 114982881, 115641569, 115642993, 115643538, 115649691, 115670847, 115671687, 116815567, 117639172, 117639190, 119266383, 119958356, 121238364, 121397532, 121698158, 121708718, 121943600, 121970978, 123017343, 123849404, 124006884, 126156972, 126756566, 127403483, 127448079, 129159629, 130291511, 130291558, 130972490, 131037988, 131973478, 132258220, 132521200, 132826927, 133709045, 134385206, 134412628, 134933117, 134998109, 135811063, 136552330, 136553211, 136554213, 136554798, 137579070, 137601710, 137663665, 137667349, 137714280, 137851207, 138075198, 138097967, 138273823, 138302559, 138660450, 138831719, 139813495, 141594080, 141860986, 141884823, 142099256, 142536132, 142582982, 144372813, 146452200, 146999560, 147111499, 147405780, 147457094, 147604980, 149203647, 151018852, 151061557, 151158004, 151158634, 151159677, 151972200, 151975489, 152451452, 153034481, 153219155, 155527062, 156336742, 156436871, 156442381, 156575528, 159587479, 168226907, 170727774, 171093866, 171094021, 171480091, 171621041, 173200071, 173213117, 173229864, 173426004, 173709268, 173712102, 173717622, 174156763, 174194059, 174247774, 174607564, 174623904, 174623946, 174623951, 174624061, 174625194, 174625307, 174625407, 174625552, 174625647, 174626867, 174754789, 174875493, 176599903, 178440917, 179606757, 179607078, 179608067, 179654627, 179659205, 179848153, 179848203, 179848415, 179930265, 179936743, 179936852, 179942496, 179965835, 180015293, 180096359, 180096468, 182438142, 182516442, 182860908, 183314955, 183565013, 183746202, 183970446, 184269528, 184359255, 184360405, 185393703, 185405431, 186057650, 186058299, 186325468, 188498026, 192118902, 192796203, 193919365, 193947342, 193971479, 193971567, 193971908, 193972138, 193973221, 193975449, 194002589, 194003533, 194004216, 194060881, 194116470, 194125886, 194497396, 195246292, 195314410, 195404163, 195489237, 196222661, 196269807, 196270383, 196270581, 196271217, 196271907, 196524776, 196584699, 196588801, 197800858, 197872508, 197872817, 198439589, 198475036, 199788352, 199819506, 200121238, 200595661, 200613777, 201661227, 201663320, 201663467, 201664501, 201693153, 201693551, 201698392, 201701643, 201726181, 201727585, 201767917, 201983184, 201983223, 203209892, 203289934, 203294845, 203695911, 203720325, 203723447, 204061479, 204071275, 204339633, 204344395, 205005075, 205333914, 205904977, 205951017, 206624869, 207118252, 207819508, 208186004, 208206831, 208598427, 208854942, 208978576, 209191450, 209226460, 209260139, 209260714, 209260788, 209768906, 209842220, 209901120, 210340207, 210495239, 210495352, 210736452, 210993156, 210993185, 211136447, 211515349, 211532217, 211679972, 211680281, 211702584, 211750362, 212138766, 212464051, 212821044, 213182009, 213390165, 213498323, 213500078, 213550108, 213550783, 213560223, 213611914, 213612663, 213615794, 213616333, 214116086, 214250188, 214327469, 214516585, 214878420, 214921670, 215232482, 215629466, 216429141, 216753945, 217097347, 217254302, 217254350, 217255149, 217261805, 217353908, 217368373, 217858968, 217861358, 217861775, 217870124, 218035890, 218151680, 218152225, 218226925, 218308954, 218310715, 218487111, 218901089, 219365086, 219850254, 219850752, 219853753, 219855754, 219857662, 219857719, 219857745, 219866336, 220121187, 220143764, 220239069, 220506626, 220517406, 220538568, 220569666, 220591655, 220594438, 220930761, 221991933, 222524313, 222531414, 222549274, 222956471, 223787165, 223787707, 223843537, 223846253, 224166708, 224500725, 225107350 };
	void* hooks::assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, std::uint8_t new_index) {
		const auto result = g_hooking->m_assignPhysicalIndexHk.getOg<decltype(&assign_physical_index)>()(netPlayerMgr, player, new_index);
		if (new_index == static_cast<uint8_t>(-1)) {
			if (const auto* net_player_data = player->GetGamerInfo()) {
				const auto message = friends_service::is_friend(player) ? TRANSLATE("FRIEND_LEFT") : TRANSLATE("PLAYER_LEFT");
				MainNotification(ImGuiToastType_PlayerLeft, 5000, TRANSLATE("PLAYER_LIST"), message, net_player_data->m_name);
			}
			m_spoofed_peer_ids.erase(player->GetGamerInfo()->m_host_token);
			g_player_service->player_leave(player);
			return result;
		}
		if (const auto* net_player_data = player->GetGamerInfo()) {
			if (player->IsConnected()) {
				bool is_host = g_player_service->get_self()->is_host();
				if (features::admin_check) {
					if (admin_rids.contains(net_player_data->m_gamer_handle.m_rockstar_id)) {
						const auto message = std::make_format_args(net_player_data->m_name);
						MainNotification(ImGuiToastType_Warning, 10000, TRANSLATE("PLAYER_LIST"), TRANSLATE("POTENTIAL_ADMIN_FOUND"), message, player->m_player_id, net_player_data->m_gamer_handle.m_rockstar_id);
						g_player_service->get_by_id(player->m_player_id)->is_admin = true;
						if (features::kick_admins) {
							if (is_host) {
								NETWORK::NETWORK_SESSION_KICK_PLAYER(player->m_player_id);
							}
							else {
								Toxic::BailKick(g_player_service->get_by_id(player->m_player_id));
								Toxic::NullFunctionKick(g_player_service->get_by_id(player->m_player_id));
								Toxic::ScriptHostKick(g_player_service->get_by_id(player->m_player_id));
								Toxic::EndSessionKick(g_player_service->get_by_id(player->m_player_id));
								DesyncKickType = 0; Toxic::DesyncKick(player);
							}
							MainNotification(ImGuiToastType_Info, 10000, TRANSLATE("PLAYER_LIST"), TRANSLATE("KICKED_ADMIN"), net_player_data->m_name);
						}
					}
				}
				if (Advertisers.contains(net_player_data->m_gamer_handle.m_rockstar_id)) {
					g_player_service->get_by_id(player->m_player_id)->is_spammer = true;
					if (features::kick_advertisers) {
						if (is_host) {
							NETWORK::NETWORK_SESSION_KICK_PLAYER(player->m_player_id);
						}
						else {
							Toxic::BailKick(g_player_service->get_by_id(player->m_player_id));
							Toxic::NullFunctionKick(g_player_service->get_by_id(player->m_player_id));
							Toxic::ScriptHostKick(g_player_service->get_by_id(player->m_player_id));
							Toxic::EndSessionKick(g_player_service->get_by_id(player->m_player_id));
							DesyncKickType = 0; Toxic::DesyncKick(player);
						}
						MainNotification(ImGuiToastType_Warning, 10000, TRANSLATE("PLAYER_LIST"), TRANSLATE("KICKED_ADVERTISER"), net_player_data->m_name);
					}
				}
				if (features::blockKickedPlayers)
				{
					if (kicked_Players.contains(player->GetName())) {
						g_player_service->get_by_id(player->m_player_id)->block_join = true;
						if (is_host) {
							NETWORK::NETWORK_SESSION_KICK_PLAYER(player->m_player_id);
						}
						else {
							Toxic::BailKick(g_player_service->get_by_id(player->m_player_id));
							Toxic::NullFunctionKick(g_player_service->get_by_id(player->m_player_id));
							Toxic::ScriptHostKick(g_player_service->get_by_id(player->m_player_id));
							Toxic::EndSessionKick(g_player_service->get_by_id(player->m_player_id));
							DesyncKickType = 0; Toxic::DesyncKick(player);
						}
						MainNotification(ImGuiToastType_Warning, 10000, TRANSLATE("PLAYER_LIST"), TRANSLATE("KICKED_PLAYER_TRIED_TO_JOIN"), net_player_data->m_name);
					}
				}
			}
			const auto message = friends_service::is_friend(player) ? TRANSLATE("FRIEND_JOINED") : TRANSLATE("PLAYER_JOINED");
			MainNotification(ImGuiToastType_PlayerJoined, 5000, TRANSLATE("PLAYER_LIST"), message, net_player_data->m_name);
		}
		g_player_service->player_join(player);
		return result;
	}
}