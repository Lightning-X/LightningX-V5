#pragma once
#include <functional>
#include "../opt.hpp"
#include "../x64Disassmbler.hpp"

namespace base::listUiTypes {
	class regularOpt : public opt<regularOpt> {
	public:
		eOptType getOptType() {
			return eOptType::RegularOpt;
		}
	};
}