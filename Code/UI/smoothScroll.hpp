#pragma once
#include <cmath>
class smoothScroll {
public:
	smoothScroll(float startCurrent, float speed) : m_target(startCurrent), m_current(startCurrent), m_speed(speed) {}
	smoothScroll() = default;
	smoothScroll& update(float speed = 0.f) {
		if (speed != 0.f)
			m_speed = speed;

		if (m_current != m_target)
			m_current += (m_target - m_current) * m_speed;
		else if (std::abs(m_current - m_target) < 0.0005f)
			m_current = m_target;

		return *this;
	}
	smoothScroll& setTarget(float newTarget) {
		m_target = newTarget;
		return *this;
	}
	float getCurrent() const {
		return m_current;
	}
private:
	float m_target{};
	float m_current{};
	float m_speed{};
};