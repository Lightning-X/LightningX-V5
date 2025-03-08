#pragma once
#include "Common.hpp"
#include "Pointers.hpp"
#include "Natives.hpp"
#include "GTAV-Classes-master/vehicle/CVehicleModelInfo.hpp"

enum class eModelClass : uint8_t {
	Compacts,
	Sedans,
	SUVs,
	Coupes,
	Muscle,
	SportsClassics,
	Sports,
	Supers,
	Motorcycles,
	OffRoad,
	Industrial,
	Utility,
	Vans,
	Cycles,
	Boats,
	Helicopters,
	Planes,
	Service,
	Emergency,
	Military,
	Commercial,
	Trains,
	OpenWheel
};

namespace base::util::vehicle {
	class vehicleData {
	public:
		vehicleData(const CVehicleModelInfo* mi, std::string ml, std::string nl) : m_modelInfo(mi), m_manufacturerLabel(ml), m_nameLabel(nl), m_finalLabel(getGXT()) {}
	public:
		const CVehicleModelInfo* m_modelInfo{};
		const std::string m_manufacturerLabel{}, m_nameLabel{}, m_finalLabel{};
	public:
		std::string getLabelAsGXT() {
			std::string displayName{ m_modelInfo->m_name };
			std::string label{ HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(displayName.c_str()) };
			return !label.compare("NULL") ? lStr(!displayName.compare("NULL") ? m_nameLabel : displayName) : label;
		}
		std::string getManufacturerAsGXT() {
			std::string displayName{ m_modelInfo->m_manufacturer };
			std::string label{ HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(displayName.c_str()) };
			return !label.compare("NULL") ? lStr(!displayName.compare("NULL") ? m_nameLabel : displayName) : label;
		}
		std::string getGXT() {
			std::string manufacturerGXT{ getManufacturerAsGXT() };
			std::string labelGXT{ getLabelAsGXT() };
			return manufacturerGXT + " " + labelGXT;
		}
	};
	class models {
	public:
		std::vector<vehicleData>& get(uint8_t idx) {
			switch (static_cast<eModelClass>(idx)) {
			case eModelClass::Compacts: { return m_compacts; } break;
			case eModelClass::Sedans: { return m_sedans; } break;
			case eModelClass::SUVs: { return m_suvs; } break;
			case eModelClass::Coupes: { return m_coupes; } break;
			case eModelClass::Muscle: { return m_muscle; } break;
			case eModelClass::SportsClassics: { return m_sportsClassics; } break;
			case eModelClass::Sports: { return m_sports; } break;
			case eModelClass::Supers: { return m_supers; } break;
			case eModelClass::Motorcycles: { return m_motorcycles; } break;
			case eModelClass::OffRoad: { return m_offRoad; } break;
			case eModelClass::Industrial: { return m_industrial; } break;
			case eModelClass::Utility: { return m_utility; } break;
			case eModelClass::Vans: { return m_vans; } break;
			case eModelClass::Cycles: { return m_cycles; } break;
			case eModelClass::Boats: { return m_boats; } break;
			case eModelClass::Helicopters: { return m_helicopters; } break;
			case eModelClass::Planes: { return m_planes; } break;
			case eModelClass::Service: { return m_service; } break;
			case eModelClass::Emergency: { return m_emergency; } break;
			case eModelClass::Military: { return m_military; } break;
			case eModelClass::Commercial: { return m_commercial; } break;
			case eModelClass::Trains: { return m_trains; } break;
			case eModelClass::OpenWheel: { return m_openWheels; } break;
			}
			return m_dummy;
		}
	private:
		std::vector<vehicleData> m_compacts{};
		std::vector<vehicleData> m_sedans{};
		std::vector<vehicleData> m_suvs{};
		std::vector<vehicleData> m_coupes{};
		std::vector<vehicleData> m_muscle{};
		std::vector<vehicleData> m_sportsClassics{};
		std::vector<vehicleData> m_sports{};
		std::vector<vehicleData> m_supers{};
		std::vector<vehicleData> m_motorcycles{};
		std::vector<vehicleData> m_offRoad{};
		std::vector<vehicleData> m_industrial{};
		std::vector<vehicleData> m_utility{};
		std::vector<vehicleData> m_vans{};
		std::vector<vehicleData> m_cycles{};
		std::vector<vehicleData> m_boats{};
		std::vector<vehicleData> m_helicopters{};
		std::vector<vehicleData> m_planes{};
		std::vector<vehicleData> m_service{};
		std::vector<vehicleData> m_emergency{};
		std::vector<vehicleData> m_military{};
		std::vector<vehicleData> m_commercial{};
		std::vector<vehicleData> m_trains{};
		std::vector<vehicleData> m_openWheels{};
	private:
		std::vector<vehicleData> m_dummy{};
	};
	inline size_t g_selectedVehicleFromClass{};
	inline uint8_t g_selectedVehicleClass{};
	inline models g_models{};
	inline std::vector<std::string> g_modelClasses{};
	inline HashTable<CVehicleModelInfo*>& getHashTable() {
		return *reinterpret_cast<HashTable<CVehicleModelInfo*>*>(base::g_pointers->m_modelTable);
	}
	inline std::vector<vehicleData> modelDataSection(uint8_t modelClass) {
		std::vector<vehicleData> vehData{};
		const HashTable<CVehicleModelInfo*>& hashTable{ getHashTable() };
		for (int i{}; i != hashTable.m_size; ++i) {
			for (HashNode* node{ hashTable.m_lookup_table[i] }; node; node = node->m_next) {
				if (const uint16_t tableIdx{ node->m_idx }; tableIdx != hashTable.m_size) {
					if (CVehicleModelInfo* model{ hashTable.m_data[tableIdx] }; model && model->is_type(eModelType::Vehicle) && model->get_class() == modelClass) {
						std::string manufacturerLbl = HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(model->m_manufacturer);
						std::string nameLbl = HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(model->m_name);
						vehData.push_back({ model, manufacturerLbl, model->m_name });
					}
				}
			}
		}
		return vehData;
	}
	inline std::string getVehicleClassName(uint8_t vehClass) {
		const std::string& classId{ std::to_string(vehClass) };
		const std::string& label{ "VEH_CLASS_" + classId };
		std::string value{ HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(label.c_str()) };
		if (!value.compare("NULL")) {
			value = "Unknown Class (" + classId + ")";
		}
		return value;
	}
	inline void cacheModelTable() {
		for (uint8_t i{}; i != 23; ++i) {
			g_modelClasses.push_back(getVehicleClassName(i));
			g_models.get(i) = modelDataSection(i);
		}
	}
}