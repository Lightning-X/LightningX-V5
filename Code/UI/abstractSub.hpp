#pragma once
#include <string>
#include "abstractOpt.hpp"
#include "../JSON.hpp"
#include "../Config.hpp"
namespace base::listUiTypes {
	inline auto g_splitStr = [](std::string str, char split) -> std::vector<std::string> {
		std::vector<std::string> fields{};
		std::string field{};
		for (int i = 0; i < str.length(); i++) {
			if (str.at(i) == split) {
				fields.push_back(field);
				field.erase();
			}
			else {
				field += str.at(i);
				if (i == str.length() - 1)
					fields.push_back(field);
			}
		}
		return fields;
	};
	struct subId {
	public:
		uint32_t id;
		std::string_view idStr;
	public:
		bool cmp(subId& subid) {
			return subid.id == id;
		}
		auto getId() {
			return id;
		}
		auto getIdAsString() {
			return std::string(idStr);
		}
		auto getSubTrail() {
			std::vector<std::string> splits{ g_splitStr(getIdAsString(), '.') };
			std::vector<std::string> trail{};
			for (int32_t i{ 0 }; i != splits.size(); ++i) {
				if (splits[i].find("_") == std::string::npos) {
					auto str = splits[i];
					char beginStr[2] = { char(toupper(str.c_str()[0])), '\0' };
					trail.push_back(std::format("{}{}", beginStr, std::string(&(str.c_str()[1]))));
				}
				else {
					auto str = g_splitStr(splits[i], '_');
					char beginStr[2] = { char(toupper(str[0].c_str()[0])), '\0' };
					trail.push_back(std::format("{}{}{}{}", beginStr, std::string(&(str[0].c_str()[1])), str[0] == "ammu" ? "-" : " ", str[1]));
				}
			}
			for (auto& v : trail) {
				if (v.find("Script") != std::string::npos || v.find("Ysc") != std::string::npos || v.find("Ammu") != std::string::npos) {
					if (!v.compare("Ysc")) {
						v = "YSC";
					}
					else if (!v.compare("Ip")) {
						v = "IP";
					}
					else if (!v.compare("Rid")) {
						v = "RID";
					}
					else if (!v.compare("Rid tools")) {
						v = "RID Tools";
					}
				}
			}
			return trail;
		}
		void setData(nlohmann::json& json, std::string name, std::string desc, std::string type = "sub", std::string right = "") {
			if (json.is_null() || json.empty()) {
				if (!type.compare("sub")) {
					json = {
						{ "opName", name },
						{ "opDesciption", desc.empty() ? "" : desc },
						{ "opType", type },
						{ "opTable", {} },
					};
				}
				else if (type.find("Opt") != std::string::npos) {
					json = {
						{ "opName", name },
						{ "opRight", right },
						{ "opDesciption", desc.empty() ? "" : desc },
						{ "opType", type }
					};
				}
			}
		}
		//Retarded fucking json with it's retarded fucking nesting system, the main developer can shove a fat cock up his ass
		nlohmann::json& getJsonTrailAtIndex(size_t idx) {
			auto subTrail = getSubTrail();
			switch (idx) {
			case 1: {
				nlohmann::json& json = g_config.m_json[subTrail[0]];
				return json;
			} break;
			case 2: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]];
				return json;
			} break;
			case 3: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]]["opTable"][subTrail[2]];
				return json;
			} break;
			case 4: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]]["opTable"][subTrail[2]]["opTable"][subTrail[3]];
				return json;
			} break;
			case 5: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]]["opTable"][subTrail[2]]["opTable"][subTrail[3]]["opTable"][subTrail[4]];
				return json;
			} break;
			case 6: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]]["opTable"][subTrail[2]]["opTable"][subTrail[3]]["opTable"][subTrail[4]]["opTable"][subTrail[5]];
				return json;
			} break;
			case 7: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]]["opTable"][subTrail[2]]["opTable"][subTrail[3]]["opTable"][subTrail[4]]["opTable"][subTrail[5]];
				return json;
			} break;
			case 8: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]]["opTable"][subTrail[2]]["opTable"][subTrail[3]]["opTable"][subTrail[4]]["opTable"][subTrail[5]]["opTable"][subTrail[6]];
				return json;
			} break;
			case 9: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]]["opTable"][subTrail[2]]["opTable"][subTrail[3]]["opTable"][subTrail[4]]["opTable"][subTrail[5]]["opTable"][subTrail[6]]["opTable"][subTrail[7]];
				return json;
			} break;
			case 10: {
				nlohmann::json& json = g_config.m_json[subTrail[0]]["opTable"][subTrail[1]]["opTable"][subTrail[2]]["opTable"][subTrail[3]]["opTable"][subTrail[4]]["opTable"][subTrail[5]]["opTable"][subTrail[6]]["opTable"][subTrail[7]]["opTable"][subTrail[8]];
				return json;
			} break;
			}
		}
		nlohmann::json& getJson(std::string desc) {
			auto subTrail = getSubTrail();
			for (size_t i{ 1 }; i != subTrail.size() + 1; i++) {
				setData(getJsonTrailAtIndex(i), subTrail[i - 1], desc);
			}
			return getJsonTrailAtIndex(subTrail.size());
		}
		nlohmann::json& getJson(std::string desc, std::string right, std::string type) {
			auto subTrail = getSubTrail();
			for (size_t i{ 1 }; i != subTrail.size() + 1; i++) {
				setData(getJsonTrailAtIndex(i), subTrail[i - 1], desc, right, type);
			}
			return getJsonTrailAtIndex(subTrail.size());
		}
	};
	inline consteval subId operator ""_id(const char* str, size_t) {
		return { constexprJoaat(str), str };
	}
	class abstractSub {
	public:
		//Deconstructor
		virtual ~abstractSub() = default;
		//Executing option(s)
		virtual void executeOptAdding() = 0;
		virtual void resetOptCount() = 0;

		//Getting sub id(s) and sub name(s)
		virtual std::string getName() = 0;
		virtual subId getId() = 0;
		virtual size_t getNumberOfOptions() = 0;
		virtual size_t getNumberOfBreakOptions() = 0;
		virtual size_t getSelectedOpt() = 0;

		virtual void setSelectedOpt(size_t opPos) = 0;
	public:
		void moveDown() {
			if (getSelectedOpt() < getNumberOfOptions() - 1)
				setSelectedOpt(getSelectedOpt() + 1);
			else
				setSelectedOpt(0);
		}
		void moveUp() {
			if (getSelectedOpt() > 0)
				setSelectedOpt(getSelectedOpt() - 1);
			else
				setSelectedOpt(getNumberOfOptions() - 1);
		}
	};
}