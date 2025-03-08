#pragma once
#include <string>

namespace base::listUiTypes {
	enum class eOptAction : int32_t {
		InvalidAction = -1,
		LeftPress,
		RightPress,
		EnterPress
	};
	enum class eOptFlag : int32_t {
		InvalidFlag = -1,
		Horizontal = (1 << 0),
		Enterable = (1 << 1)
	};
	enum class eOptType : int32_t {
		InvalidOpt = -1,
		RegularOpt,
		SubOpt,
		BoolOpt,
		NumOpt,
		NumBoolOpt,
		VecOpt,
		VecBoolOpt,
		ColorOpt,
		BreakOpt,
		IconOpt,
	};
	class abstractOpt {
	public:
		virtual ~abstractOpt() = default;
		virtual std::string getLeft() { return ""; }
		virtual std::string getRight() { return ""; }
		virtual std::string getDescription() { return ""; }
		virtual void handleAction(eOptAction action) {}
		virtual eOptFlag getFlag() { return eOptFlag::InvalidFlag; }
		virtual eOptType getOptType() { return eOptType::InvalidOpt; }
	};
}