#pragma once
#include "opt.hpp"
#include "UI/stringStream.hpp"

namespace base::listUiTypes {
	template <typename dataType, typename posType = size_t>
	class vecBoolOpt : public opt<vecBoolOpt<dataType, posType>> {
	public:
		vecBoolOpt() : m_bool(nullptr), m_pos(nullptr), m_data(nullptr), m_dataSize(0) {}
	public:
		template <posType pos>
		vecBoolOpt& setArray(dataType(&arr)[pos]) {
			m_data = reinterpret_cast<dataType*>((&arr)[0]); //Cursed, so very cursed...
			m_dataSize = pos;
			return *this;
		}
		template <typename vecType>
		vecBoolOpt& setArray(vecType& vec) {
			m_data = vec.data();
			m_dataSize = posType(vec.size());
			return *this;
		}
		vecBoolOpt& setPos(posType& pos) {
			m_pos = &pos;
			return *this;
		}
		vecBoolOpt& setBool(bool& togg) {
			m_bool = &togg;
			return *this;
		}
	public:
		std::string getRight() {
			char m_rT[228]{};
			base::ui::mem::stringStream str(m_rT);
			if (m_data) {
				str << m_data[*m_pos];
				str << "_[" << *m_pos + 1 << "/" << m_dataSize << std::format("]_{}", (*m_bool) ? "a" : "b");
			}
			else {
				str << std::format("Unk_[0/0]_{}", (*m_bool) ? "a" : "b");
			}
			m_baseT::setRight(m_rT);
			return m_baseT::getRight();
		}
		void handleAction(eOptAction action) {
			switch (action) {
			case eOptAction::EnterPress: {
				*m_bool = !(*m_bool);
				m_baseT::handleAction(action);
			} break;
			case eOptAction::LeftPress: {
				if (*m_pos > 0)
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
			}
		}
		eOptType getOptType() {
			return eOptType::VecBoolOpt;
		}
	private:
		bool* m_bool;
		posType* m_pos;
		dataType* m_data;
		posType m_dataSize;
		using m_baseT = opt<vecBoolOpt<dataType, posType>>;
	};
}