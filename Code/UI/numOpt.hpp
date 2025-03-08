#pragma once
#include "../opt.hpp"

namespace base::listUiTypes {
	template <typename numType = int>
	class numOpt : public opt<numOpt<numType>> {
	public:
		numOpt& setMinMax(numType min, numType max) {
			m_min = min;
			m_max = max;
			return *this;
		}
		numOpt& setMin(numType value) {
			m_min = value;
			return *this;
		}
		numOpt& setMax(numType value) {
			m_max = value;
			return *this;
		}
		numOpt& setStep(numType value) {
			m_step = value;
			return *this;
		}
		numOpt& setNum(numType& value) {
			m_num = &value;
			return *this;
		}
		numOpt& setNumValue(numType value) {
			*m_num = value;
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
	public:
		std::string getRight() {
			std::ostringstream ss;
			ss << float(*m_num);
			m_baseT::setRight(ss.str().c_str());
			return m_baseT::getRight();
		}
		void handleAction(eOptAction action) {
			switch (action) {
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
			default: {
				m_baseT::handleAction(action);
			} break;
			}
		}
		eOptType getOptType() {
			return eOptType::NumOpt;
		}
	private:
		numType* m_num;
		numType m_min;
		numType m_max;
		numType m_step;
		using m_baseT = opt<numOpt<numType>>; //A simple type define to avoid VS dying because of the custom typename
	};
}