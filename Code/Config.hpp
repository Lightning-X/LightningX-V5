#pragma once
#include "Common.hpp"
#include "JSON.hpp"

class config {
public:
	bool load(std::string lang);
	bool save();
public:
	nlohmann::json m_json{};
public:
	std::string m_currentLanguage{};
	fs::path m_path{ "C:\\" };
};
inline config g_config{};