#pragma once
#include <string>
#include <vector>
#include "ScriptGlobal.hpp"
#include "Translations.hpp"
#include "Natives.hpp"
#include "Fibers/fiber_pool.hpp"
namespace base {
	struct outfit_t
	{
		int id;
		std::string label;
		int drawable_id = 0;
		int drawable_id_max = 0;
		int texture_id = 0;
		int texture_id_max = 0;
	};

	struct components_t
	{
		std::vector<outfit_t> items = {
			{0, TRANSLATE("OUTFIT_HEAD")},
			{1,TRANSLATE("OUTFIT_BEARD")},
			{2, TRANSLATE("OUTFIT_HAIR")},
			{3, TRANSLATE("OUTFIT_UPPR")},
			{4, TRANSLATE("OUTFIT_LOWR")},
			{5, TRANSLATE("OUTFIT_HAND")},
			{6, TRANSLATE("OUTFIT_FEET")},
			{7, TRANSLATE("OUTFIT_TEEF")},
			{8, TRANSLATE("OUTFIT_ACCS")},
			{9, TRANSLATE("OUTFIT_TASK")},
			{10, TRANSLATE("OUTFIT_DECL")},
			{11, TRANSLATE("OUTFIT_JBIB")} };
	};

	struct props_t
	{
		std::vector<outfit_t> items = {
			{0, TRANSLATE("OUTFIT_HATS")},
			{1, TRANSLATE("OUTFIT_GLASSES")},
			{2, TRANSLATE("OUTFIT_EARS")},
			{3, TRANSLATE("OUTFIT_UNK1")},
			{4, TRANSLATE("OUTFIT_UNK2")},
			{5, TRANSLATE("OUTFIT_UNK3")},
			{6, TRANSLATE("OUTFIT_WATCH")},
			{7, TRANSLATE("OUTFIT_WRIST")},
			{8, TRANSLATE("OUTFIT_UNK4")} };
	};

	inline void check_bounds_drawable(outfit_t* item)
	{
		if (item->drawable_id > item->drawable_id_max)
			item->drawable_id = item->drawable_id_max;
		if (item->drawable_id < -1)
			item->drawable_id = -1;
	}
	inline void check_bounds_texture(outfit_t* item)
	{
		if (item->texture_id > item->texture_id_max)
			item->texture_id = item->texture_id_max;
		if (item->texture_id < -1)
			item->texture_id = -1;
	}

	inline char* get_slot_name_address(int slot)
	{
		return globals(2359296).at(0, 5568).at(681).at(2460).at(slot, 8).as<char*>();
	}

	inline int* get_component_drawable_id_address(int slot, int id)
	{
		return globals(2359296).at(0, 5568).at(681).at(1336).at(slot, 13).at(id, 1).as<int*>();
	}

	inline int* get_component_texture_id_address(int slot, int id)
	{
		return globals(2359296).at(0, 5568).at(681).at(1610).at(slot, 13).at(id, 1).as<int*>();
	}

	inline int* get_prop_drawable_id_address(int slot, int id)
	{
		return globals(2359296).at(0, 5568).at(681).at(1884).at(slot, 10).at(id, 1).as<int*>();
	}

	inline int* get_prop_texture_id_address(int slot, int id)
	{
		return globals(2359296).at(0, 5568).at(681).at(2095).at(slot, 10).at(id, 1).as<int*>();
	}

	inline void set_ped_random_component_variation(Ped ped)
	{
		auto range = [](int lower_bound, int upper_bound) -> int {
			return std::rand() % (upper_bound - lower_bound + 1) + lower_bound;
		};
		components_t components;
		for (auto const& item : components.items)
		{
			int drawable_id_max = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(ped, item.id) - 1;
			if (drawable_id_max == -1)
				continue;
			int drawable_id = range(0, drawable_id_max);
			int texture_id_max = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(ped, item.id, drawable_id) - 1;
			if (texture_id_max == -1)
				continue;
			int texture_id = range(0, texture_id_max);
			PED::SET_PED_COMPONENT_VARIATION(ped, item.id, drawable_id, texture_id, PED::GET_PED_PALETTE_VARIATION(ped, item.id));
		}
	}
}