#pragma once

#include <map>

namespace base {
	class lblMgr {
	public:
		void add(const char* lbl, std::string contents) {
			m_labelList.insert(std::make_pair(lbl, contents));
		}
		void remove(const char* lbl) {
			m_labelList.erase(lbl);
		}
		const char* getLabels(const char* label) {
			if (auto lbl = m_labelList.find(label); lbl != m_labelList.end()) {
				if (lbl->first == label)
					return lbl->second.c_str();
			}
			return "lblNotExist";
		}
	public:
		std::map<const char*, std::string> m_labelList = {
			{ "MP_CHAT_ALL", "COCK" },
			{ "MP_CHAT_TEAM", "COCK" },
			{ "HUD_JOINING", "Loading..." },
		};
	};
	inline lblMgr g_lblMgr;
}