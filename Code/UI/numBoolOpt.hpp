#pragma once
#include "../opt.hpp"
#include "stringStream.hpp"

namespace base::listUiTypes {
	template <typename numType = int>
	class numBoolOpt : public opt<numBoolOpt<numType>> {
	public:
		numBoolOpt& setMinMax(numType min, numType max) {
			m_min = min;
			m_max = max;
			return *this;
		}
		numBoolOpt& setMin(numType value) {
			m_min = value;
			return *this;
		}
		numBoolOpt& setMax(numType value) {
			m_max = value;
			return *this;
		}
		numBoolOpt& setStep(numType value) {
			m_step = value;
			return *this;
		}
		numBoolOpt& setNum(numType& value) {
			m_num = &value;
			return *this;
		}
		numBoolOpt& setNumValue(numType value) {
			*m_num = value;
			return *this;
		}
		numBoolOpt& setBool(bool& togg) {
			m_bool = &togg;
			return *this;
		}
		numBoolOpt& setBoolValue(bool v) {
			*m_bool = v;
			return *this;
		}
		auto getMin() {
			return m_min;
		}
		auto getMax() {
			return m_max;
		}
		auto getStep() {
			return m_step;
		}
		numType getNum() {
			return *m_num;
		}
		auto getBool() {
			return *m_bool;
		}
	public:
		std::string getRight() {
			const bool isTypeFloat = typeid(*m_num).hash_code() == 0xAF64004C296629F4;
			auto floatStr = isTypeFloat ? std::format("{:.1f}", float(*m_num)) : std::to_string(float(*m_num));
			const auto& str = std::format("{} {}", floatStr.c_str(), (*m_bool) ? "a" : "b");
			m_baseT::setRight(str);
			return m_baseT::getRight();
		}
		void handleAction(eOptAction action) {
			switch (action) {
			case eOptAction::EnterPress: {
				*m_bool = !(*m_bool);
				m_baseT::handleAction(action);
			} break;
			case eOptAction::LeftPress: {
				if (*m_num > m_min)
					*m_num -= m_step;
				else
					*m_num = m_max;
			} break;
			case eOptAction::RightPress: {
				if (*m_num < m_max)
					*m_num += m_step;
				else
					*m_num = m_min;
			} break;
			}
		}
		eOptType getOptType() {
			return eOptType::NumBoolOpt;
		}
	private:
		bool* m_bool;
		numType* m_num;
		numType m_min;
		numType m_max;
		numType m_step;
		using m_baseT = opt<numBoolOpt<numType>>;
	};
}