#pragma once
#include "Hooking.hpp"
#include "Features.hpp"
namespace base
{
	struct model_swapper
	{
		std::vector<std::pair<std::string, std::string>> models;
		std::mutex m;
		bool update = false;
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(model_swapper, models)
	} model_swapper{};

		static auto get_hash(const std::string& str) {
			rage::joaat_t hash = 0;
			if (str.substr(0, 2) == "0x")
				std::stringstream(str.substr(2)) >> std::hex >> hash;
			else
				hash = rage::joaat(str.c_str());
			return hash;
		}

		static std::vector<std::pair<rage::joaat_t, rage::joaat_t>> cache_models;

		static void update() {
			std::lock_guard lock(model_swapper.m);
			cache_models.clear();
			for (auto& model : model_swapper.models)
				cache_models.emplace_back(get_hash(model.first), get_hash(model.second));
			model_swapper.update = false;
		}

		CBaseModelInfo* hooks::get_model_info(rage::joaat_t hash, uint32_t* a2) {
			static bool init = ([] { update(); }(), true);
			if (model_swapper.update)
				update();
			for (auto& cache_model : cache_models)
				if (cache_model.first == hash)
					return g_hooking->m_getModelInfoHk.getOg<decltype(&get_model_info)>()(cache_model.second, a2);
			return g_hooking->m_getModelInfoHk.getOg<decltype(&get_model_info)>()(hash, a2);
		}
}