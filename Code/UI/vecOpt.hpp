#pragma once
#include "../opt.hpp"
#include "stringStream.hpp"

namespace base::listUiTypes {
	template <typename dataType, typename posType>
	class vecOpt : public opt<vecOpt<dataType, posType>> {
	public:
		vecOpt() : m_pos(nullptr), m_data(nullptr), m_dataSize(0) {}
	public:
		template <posType pos>
		vecOpt& setArray(dataType(&arr)[pos]) {
			m_data = reinterpret_cast<dataType*>((&arr)[0]); //Cursed, so very cursed...
			m_dataSize = pos;
			return *this;
		}
		template <typename vecType>
		vecOpt& setArray(vecType& vec) {
			m_data = vec.data();
			m_dataSize = vec.size();
			return *this;
		}
		vecOpt& setPos(posType& pos) {
			m_pos = &pos;
			return *this;
		}
	public:
		std::string getRight() {
			char m_rT[128];
			base::ui::mem::stringStream str(m_rT);
			m_baseT::setRight(std::format("{}_[{}/{}]", m_data ? m_data[*m_pos] : "Unk", m_data ? *m_pos + 1 : 0, m_data ? m_dataSize : 0));
			return m_baseT::getRight();
		}
		void handleAction(eOptAction action) {
			switch (action) {
			case eOptAction::LeftPress: {
				if (*m_pos >= 1)
					(*m_pos)--;
				else
					*m_pos = static_cast<posType>(m_dataSize - 1);
			} break;
			case eOptAction::RightPress: {
				if (*m_pos < m_dataSize - 1)
					(*m_pos)++;
				else
					*m_pos = 0;
			} break;
			default: {
				m_baseT::handleAction(action);
			} break;
			}
		}
		eOptType getOptType() {
			return eOptType::VecOpt;
		}
	private:
		posType* m_pos;
		dataType* m_data;
		posType m_dataSize;
		using m_baseT = opt<vecOpt<dataType, posType>>; //A simple type define to avoid VS dying because of the custom typename
	};
}