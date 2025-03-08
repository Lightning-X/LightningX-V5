#include "NativeHooking.hpp"
#include "GtaUtill.hpp"
#include "GTAV-Classes-master/script/scrProgram.hpp"
#include "GTAV-Classes-master/script/scrProgramTable.hpp"
#include "Crossmap.hpp"
#include "AllScripts.hpp"
namespace base
{
	static bool map_native(rage::scrNativeHash* hash)
	{
		for (auto const& mapping : g_crossmap)
		{
			if (mapping.first == *hash)
			{
				*hash = mapping.second;
				return true;
			}
		}

		return false;
	}

	native_hook::native_hook(rage::scrProgram* program, const ankerl::unordered_dense::map<rage::scrNativeHash, rage::scrNativeHandler>& native_replacements)
	{
		hook_instance(program, native_replacements);
	}

	native_hook::~native_hook()
	{
		if (m_handler_hook)
		{
			m_handler_hook->disable();
			m_handler_hook.reset();
		}

		if (m_vmt_hook)
		{
			m_vmt_hook->disable();
			m_vmt_hook.reset();
		}
	}

	void native_hook::hook_instance(rage::scrProgram* program, const ankerl::unordered_dense::map<rage::scrNativeHash, rage::scrNativeHandler>& native_replacements)
	{
		m_program = program;
		m_vmt_hook = std::make_unique<vmt_hook>(m_program, 9);
		m_vmt_hook->hook(6, &scrprogram_dtor);
		m_vmt_hook->enable();

		m_handler_hook = std::make_unique<vmt_hook>(&m_program->m_native_entrypoints, m_program->m_native_count);
		m_handler_hook->enable();

		ankerl::unordered_dense::segmented_map<rage::scrNativeHandler, rage::scrNativeHandler> handler_replacements;

		for (auto& [replacement_hash, replacement_handler] : native_replacements)
		{
			auto native = replacement_hash;
			map_native(&native);

			auto og_handler = g_pointers->m_getNativeHandler(g_pointers->m_nativeRegTbl, native);
			if (!og_handler)
				continue;

			handler_replacements[og_handler] = replacement_handler;
		}

		for (int i = 0; i < m_program->m_native_count; i++)
		{
			if (auto it = handler_replacements.find((rage::scrNativeHandler)program->m_native_entrypoints[i]);
				it != handler_replacements.end())
			{
				m_handler_hook->hook(i, it->second);
			}
		}
	}

	void native_hook::scrprogram_dtor(rage::scrProgram* this_, char free_memory)
	{
		if (auto it = g_native_hooks->m_native_hooks.find(this_); it != g_native_hooks->m_native_hooks.end())
		{
			auto og_func = it->second->m_vmt_hook->get_original<decltype(&native_hook::scrprogram_dtor)>(6);
			it->second->m_vmt_hook->disable();
			it->second->m_vmt_hook.reset();
			it->second->m_handler_hook->disable();
			it->second->m_handler_hook.reset();
			g_native_hooks->m_native_hooks.erase(it);
			og_func(this_, free_memory);
		}
		else
		{
			g_logger->send(red, TRANSLATE("NATIVE_HOOK"), TRANSLATE("UNABLE_TO_FIND_HOOK_FOR_PROGRAM"));
		}
	}

	constexpr auto ALL_SCRIPT_HASH = "ALL_SCRIPTS"_joaat;

	native_hooks::native_hooks()
	{
		add_native_detour("main_persistent"_joaat, 0x767FBC2AC802EF3D, allscripts::STAT_GET_INT);
		add_native_detour(0x812595A0644CE1DE, allscripts::IS_DLC_PRESENT);
		add_native_detour("maintransition"_joaat, 0x6F3D4ED9BEE4E61D, network::NETWORK_SESSION_HOST);
		add_native_detour("freemode"_joaat, 0x95914459A87EBA28, network::NETWORK_BAIL);
		add_native_detour("maintransition"_joaat, 0x95914459A87EBA28, network::NETWORK_BAIL);
		add_native_detour(0x580CE4438479CC61, network::NETWORK_CAN_BAIL);
		add_native_detour(0xADF692B254977C0C, allscripts::SET_CURRENT_PED_WEAPON);
		add_native_detour(0xFE99B66D079CF6BC, allscripts::DISABLE_CONTROL_ACTION);
		add_native_detour(0xEB354E5376BC81A7, allscripts::HUD_FORCE_WEAPON_WHEEL);
		add_native_detour(0x158C16F5E4CF41F8, allscripts::RETURN_TRUE); //bypass casino country restrictions
		add_native_detour(0x40EB1EFD921822BC, allscripts::DO_NOTHING); // SECURITY::REGISTER_SCRIPT_VARIABLE
		add_native_detour(0x340A36A700E99699, allscripts::DO_NOTHING); // SECURITY::UNREGISTER_SCRIPT_VARIABLE
		add_native_detour(0x8E580AB902917360, allscripts::DO_NOTHING); // SECURITY::FORCE_CHECK_SCRIPT_VARIABLES
		add_native_detour(0x6BFB12CE158E3DD4, maintransition::SC_TRANSITION_NEWS_SHOW);       // Stops news.
		add_native_detour(0xFE4C1D0D3B9CC17E, maintransition::SC_TRANSITION_NEWS_SHOW_TIMED); // Stops news.
		add_native_detour("shop_controller"_joaat, 0x34616828CD07F1A1, allscripts::RETURN_FALSE); // prevent exploit reports
		add_native_detour("freemode"_joaat, 0x767FBC2AC802EF3D, freemode::STAT_GET_INT);
		add_native_detour("freemode"_joaat, 0x5E9564D8246B909A, freemode::IS_PLAYER_PLAYING);
		add_native_detour("freemode"_joaat, 0xEA1C610A04DB6BBB, freemode::SET_ENTITY_VISIBLE);
		add_native_detour("freemode"_joaat, 0x5D10B3795F3FC886, freemode::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA);
		add_native_detour("fmmc_launcher"_joaat, 0x5D10B3795F3FC886, freemode::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA);
		add_native_detour("shop_controller"_joaat, 0xDC38CC1E35B6A5D7, shop_controller::SET_WARNING_MESSAGE_WITH_HEADER);
		add_native_detour("am_launcher"_joaat, 0xB8BA7F44DF1575E1, am_launcher::START_NEW_SCRIPT_WITH_ARGS);
		add_native_detour("am_launcher"_joaat, 0x5D10B3795F3FC886, freemode::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA);
		add_native_detour("maintransition"_joaat, 0x933BBEEB8C61B5F4, maintransition::IS_SWITCH_TO_MULTI_FIRSTPART_FINISHED); // This hook lets you stop player-switch in "Pre-HUD Checks"
		add_native_detour("maintransition"_joaat, 0x198F77705FA0931D, maintransition::SET_FOCUS_ENTITY); // Prevets map from unloading.
		add_native_detour("maintransition"_joaat, 0x719FF505F097FD20, maintransition::HIDE_HUD_AND_RADAR_THIS_FRAME); // Draw hud and radar in transition. (Doesn't work.)
		add_native_detour("maintransition"_joaat, 0xEF01D36B9C9D0C7B, maintransition::ACTIVATE_FRONTEND_MENU); // Let's you controll your ped when going sp to mp.
		add_native_detour("maintransition"_joaat, 0x10706DC6AD2D49C0, maintransition::RESTART_FRONTEND_MENU); // Let's you controll your ped when going sp to mp.
		add_native_detour("maintransition"_joaat, 0xDFC252D8A3E15AB7, maintransition::TOGGLE_PAUSED_RENDERPHASES); // Prevents the game from freezing your screen.
		add_native_detour("maintransition"_joaat, 0xEA1C610A04DB6BBB, maintransition::SET_ENTITY_VISIBLE); // Makes you visible.
		add_native_detour("maintransition"_joaat, 0x06843DA7060A026B, maintransition::SET_ENTITY_COORDS_NO_OFFSET); // Prevents the game from teleporting you.
		add_native_detour("maintransition"_joaat, 0x1A9205C1B9EE827F, maintransition::SET_ENTITY_COLLISION); // Prevents you from falling.
		add_native_detour("maintransition"_joaat, 0x8D32347D6D4C40A2, maintransition::SET_PLAYER_CONTROL); // Allows controll in session switch.
		add_native_detour("maintransition"_joaat, 0x428CA6DBD1094446, maintransition::FREEZE_ENTITY_POSITION); // Allows controll in session switch.
		add_native_detour("maintransition"_joaat, 0xEA23C49EAA83ACFB, maintransition::NETWORK_RESURRECT_LOCAL_PLAYER); // Prevents player from teleporting after switch.
		add_native_detour("maintransition"_joaat, 0x8D30F648014A92B5, maintransition::GET_EVER_HAD_BAD_PACK_ORDER); // Prevent weird reloading when using custom dlcs.

		for (auto& entry : *g_pointers->m_scriptProgramTable)
			if (entry.m_program)
				hook_program(entry.m_program);

		g_native_hooks = this;
	}

	native_hooks::~native_hooks()
	{
		m_native_hooks.clear();
		g_native_hooks = nullptr;
	}

	void native_hooks::add_native_detour(rage::scrNativeHash hash, rage::scrNativeHandler detour)
	{
		add_native_detour(ALL_SCRIPT_HASH, hash, detour);
	}

	void native_hooks::add_native_detour(rage::joaat_t script_hash, rage::scrNativeHash hash, rage::scrNativeHandler detour)
	{
		if (const auto& it = m_native_registrations.find(script_hash); it != m_native_registrations.end())
		{
			it->second.emplace_back(hash, detour);
			return;
		}

		m_native_registrations.emplace(script_hash, std::vector<native_detour>({ {hash, detour} }));
		//g_logger->send(lightBlue, "[Natives]", "Hooked: {} | {} | {}", script_hash, hash, detour);
		//std::cout << "[Natives]: Hooked: " << script_hash << " with hash of " << hash << " and detour " << detour << "\n";
	}

	void native_hooks::hook_program(rage::scrProgram* program)
	{
		ankerl::unordered_dense::map<rage::scrNativeHash, rage::scrNativeHandler> native_replacements;
		const auto script_hash = program->m_name_hash;

		// Functions that need to be detoured for all scripts
		if (const auto& pair = m_native_registrations.find(ALL_SCRIPT_HASH); pair != m_native_registrations.end())
			for (const auto& native_hook_reg : pair->second)
				native_replacements.insert(native_hook_reg);

		// Functions that only need to be detoured for a specific script
		if (const auto& pair = m_native_registrations.find(script_hash); pair != m_native_registrations.end())
			for (const auto& native_hook_reg : pair->second)
				native_replacements.insert(native_hook_reg);

		if (!native_replacements.empty())
		{
			m_native_hooks.emplace(program, std::make_unique<native_hook>(program, native_replacements));
		}
	}
}