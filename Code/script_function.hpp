#pragma once
#include "Memory.hpp"
#include "GTAV-Classes-master/rage/joaat.hpp"
#include "GTAV-Classes-master/script/scrProgram.hpp"
#include "GTAV-Classes-master/script/scrThread.hpp"
namespace base
{
	// a lightweight script function wrapper inspired by https://github.com/Parik27/V.Rainbomizer/blob/master/src/mission/missions_YscUtils.hh
	class script_function
	{
		rage::joaat_t m_script;
		const signature m_pattern;
		int32_t m_offset;
		int32_t m_ip;
		std::string m_name;

	public:
		script_function(const std::string& name, const rage::joaat_t script, const std::string& pattern, int32_t offset);
		void populate_ip();
		void call(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<rage::scrValue> args);
		void call_latent(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<rage::scrValue> args, bool& done);

		// for pure functions that do not need access to thread stack
		void static_call(std::initializer_list<rage::scrValue> args);

		void operator()(std::initializer_list<rage::scrValue> args);
	};

	namespace scr_functions
	{
		//static inline script_function join_ceo("JC", rage::joaat("freemode"), "2D 04 1D 00 00 5D", 0);
		static inline script_function set_freemode_session_active("SFSA", "freemode"_joaat, "2D 00 02 00 00 75 5D ? ? ? 50", 0);
		static inline script_function reset_session_data("RSD", "pausemenu_multiplayer"_joaat, "2D 02 7D 00 00", 0);
		//static inline script_function dance_loop("DL", rage::joaat("am_mp_nightclub"), "2D 00 14 00 00 4F ? ? 47 ? ? 5D ? ? ? 56", 0);
		//static inline script_function init_nightclub_script("INS", rage::joaat("am_mp_nightclub"), "2D 00 11 00 00 4F", 0);

		//static inline script_function save_to_datafile("STD", rage::joaat("fm_race_creator"), "2D 01 03 00 00 71 2C", 0);
		//static inline script_function load_from_datafile("LFD", rage::joaat("fm_race_creator"), "2D 04 0D 00 00 71 2C", 0);

		//static inline script_function modshop_loop("ML", rage::joaat("carmod_shop"), "2D 00 07 00 00 71 51", 0);
		//static inline script_function setup_modshop("SM", rage::joaat("carmod_shop"), "2D 04 12 00 00 38 00 51", 0);
	}
}