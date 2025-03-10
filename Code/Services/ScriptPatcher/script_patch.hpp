#pragma once
#include "../../Memory.hpp"
#include "../../GTAV-Classes-master/rage/joaat.hpp"
namespace base
{
	struct script_data;

	class script_patch
	{
		rage::joaat_t m_script;
		const signature m_pattern;
		int32_t m_offset;
		std::vector<uint8_t> m_patch;
		std::vector<uint8_t> m_original;
		bool* m_bool;
		int32_t m_ip;

		static std::uint8_t* get_code_address(script_data* data, std::uint32_t index);
		static const std::optional<uint32_t> get_code_location_by_pattern(script_data* data, const signature& pattern);
		void enable(script_data* data);
		void disable(script_data* data);

	public:
		inline rage::joaat_t get_script() const
		{
			return m_script;
		}

		script_patch(rage::joaat_t script, const signature pattern, int32_t offset, std::vector<std::uint8_t> patch, bool* enable_bool);
		void update(script_data* data);
	};
}