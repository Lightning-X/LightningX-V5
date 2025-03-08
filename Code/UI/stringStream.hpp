#pragma once
#include "../Common.hpp"

namespace base::ui::mem {
	class stringStreamBuf : public std::streambuf {
	public:
		stringStreamBuf(char* arr, std::size_t arrSize) {
			std::fill_n(arr, arrSize, '\0');
			std::streambuf::setp(arr, arr + arrSize - 1);
		}
		template <std::size_t arrSize>
		stringStreamBuf(char(&array)[arrSize]) : stringStreamBuf(&array[0], arrSize) {}
	};
	class stringStream : virtual stringStreamBuf, public std::iostream {
	public:
		stringStream(char* arr, std::size_t arrSize) : stringStreamBuf(arr, arrSize), std::iostream(static_cast<stringStreamBuf*>(this)) {}
		template <std::size_t arrSize>
		stringStream(char(&arr)[arrSize]) : stringStreamBuf(arr), std::iostream(static_cast<stringStreamBuf*>(this)) {}
	public:
	};
	template <std::size_t bufSize>
	class stringStreamWithBuf : public stringStream {
	public:
		stringStreamWithBuf() : stringStream(m_buffer) {}
	public:
		char m_buffer[bufSize];
	};
}