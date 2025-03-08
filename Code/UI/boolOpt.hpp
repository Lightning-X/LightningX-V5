#pragma once
#include "../opt.hpp"
#include <functional>

namespace base::listUiTypes {
	class boolOpt : public opt<boolOpt> {
	public:
		boolOpt& setBool(bool& togg) {
			m_bool = &togg;
			return *this;
		}
		boolOpt& setBoolValue(bool v) {
			*m_bool = v;
			return *this;
		}
		auto getBool() {
			return *m_bool;
		}
	public:
		std::string getRight() {
			setRight((*m_bool) ? "a" : "b");
			return opt::getRight();
		}
		void handleAction(eOptAction action) {
			switch (action) {
			case eOptAction::EnterPress: {
				*m_bool = !(*m_bool);
				opt::handleAction(action);
			} break;
			}
		}
		eOptType getOptType() {
			return eOptType::BoolOpt;
		}
	private:
		bool* m_bool;
	};
}