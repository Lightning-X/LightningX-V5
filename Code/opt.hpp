#pragma once
#include "ui/abstractOpt.hpp"
#include "exceptionHandler.hpp"
namespace base::listUiTypes {
	inline void exceptionHandler(PEXCEPTION_POINTERS exceptionInfo) {
		auto& ctx = exceptionInfo->ContextRecord;
		uint8_t* ripPtr = reinterpret_cast<uint8_t*>(ctx->Rip);
		const auto len = x64::getInstructionLength(ripPtr);
		if (len) {
			ctx->Rip += len;
		}
	}
	template <typename T>
	class opt : public abstractOpt {
	public:
		std::string getLeft() {
			return m_left;
		}
		std::string getRight() {
			return m_right;
		}
		std::string getDescription() {
			return m_description;
		}
		std::string getOptionTypeString() {
			switch (getOptType()) {
			case eOptType::RegularOpt: return "regularOpt"; break;
			case eOptType::SubOpt: return "sub"; break;
			case eOptType::BoolOpt: return "boolOpt"; break;
			case eOptType::NumOpt: return "numOpt"; break;
			case eOptType::NumBoolOpt: return "numBoolOpt"; break;
			case eOptType::VecOpt: return "vecOpt"; break;
			case eOptType::VecBoolOpt: return "vecBoolOpt"; break;
			case eOptType::ColorOpt: return "colorOpt"; break;
			case eOptType::BreakOpt: return "breakOpt"; break;
			case eOptType::IconOpt: return "iconOpt"; break;
			default: { return "invalid"; } break;
			}
		}
		void handleAction(base::listUiTypes::eOptAction action) {
			__try {
				if (action == eOptAction::EnterPress) {
					if (const auto& act = m_action; act)
						act();
				}
			}
			__except (exceptionHandler(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER) {}
		}
		T& setLeft(std::string text) {
			m_left = text;
			return static_cast<T&>(*this);
		}
		T& setRight(std::string text) {
			m_right = text;
			return static_cast<T&>(*this);
		}
		T& setDesc(std::string text) {
			m_description = text;
			return static_cast<T&>(*this);
		}
		T& setAction(std::function<void()> action) {
			m_action = std::move(action);
			return static_cast<T&>(*this);
		}
	private:
		std::string m_left;
		std::string m_right;
		std::string m_description;
		std::function<void()> m_action;
	};
}