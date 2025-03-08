#pragma once
#include <chrono>

namespace base {
	class timer {
	public:
		timer(std::chrono::milliseconds timerDelay) :
			m_timer(std::chrono::high_resolution_clock::now()),
			m_delay(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(timerDelay)) {}
		~timer() = default;
	public:
		bool update() {
			auto currentTime = std::chrono::high_resolution_clock::now();
			if ((currentTime.time_since_epoch() - m_timer.time_since_epoch()).count() >= m_delay.count()) {
				m_timer = currentTime;
				return true;
			}
			return false;
		}
	public:
		void setDelay(std::chrono::milliseconds delay) {
			m_delay = delay;
		}
		void setDelay(int delay) {
			m_delay = std::chrono::milliseconds(delay);
		}
	public:
		std::chrono::high_resolution_clock::time_point m_timer;
		std::chrono::high_resolution_clock::duration m_delay;
	};
	class simpleTimer {
	public:
		void start(uint64_t ticks) {
			if (m_tick) {
				m_readyAt = GetTickCount64() + ticks;
				m_tick = false;
			}
		}
		bool isReady() { return GetTickCount64() > m_readyAt; }
		void reset() { m_tick = true; }
	public:
		uint64_t m_readyAt;
		bool m_tick;
	};
}