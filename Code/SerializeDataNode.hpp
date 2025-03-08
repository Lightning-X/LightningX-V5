#pragma once
#include "Hooking.hpp"
#include "Translations.hpp"
#include "Notify.hpp"
#include "Pointers.hpp"
namespace base
{
	constexpr std::array<std::uint32_t, 8> valid_parachute_models = {
		"p_parachute_s"_joaat,
		"vw_p_para_bag_vine_s"_joaat,
		"reh_p_para_bag_reh_s_01a"_joaat,
		"xm_prop_x17_scuba_tank"_joaat,
		"lts_p_para_bag_pilot2_s"_joaat,
		"lts_p_para_bag_lts_s"_joaat,
		"p_para_bag_tr_s_01a"_joaat,
		"p_para_bag_xmas_s"_joaat,
	};
	bool is_valid_parachute_model(rage::joaat_t hash) {
		for (auto& model : valid_parachute_models)
			if (model == hash)
				return true;

		return false;
	}
	void hooks::serialize_take_off_ped_variation_task(ClonedTakeOffPedVariationInfo* info, rage::CSyncDataBase* serializer) {
		g_hooking->m_serializeTakeOffPedVariationTaskHk.getOg<decltype(&serialize_take_off_ped_variation_task)>()(info, serializer);
		if (!is_valid_parachute_model(info->m_prop_hash)) {
			g_player_service->get_by_id(m_syncing_player->m_player_id)->is_modder = true;
			MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_PARACHUTE_MODEL"));
			info->m_prop_hash = 0;
		}
	}
}