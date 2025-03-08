#pragma once
#include "../Common.hpp"
#include "Fibers/Script.hpp"
#include "Threading.hpp"
namespace base {
	class dxFiberMgr {
	public:
		void add(std::unique_ptr<script> fbr, char const* fbrId) {
			std::lock_guard lck(m_mutex);
			m_fibers.insert({ fbrId, std::move(fbr) });
		}
		void remove(char const* fbrId) {
			std::lock_guard lck(m_mutex);
			m_fibers.erase(fbrId);
		}
		void removeAll() {
			std::lock_guard lck(m_mutex);
			m_fibers.clear();
		}
		void scrTick() {
			executeUnderHybridThr(std::mem_fn(&dxFiberMgr::tick), this);
		}
		void tick() {
			if (static bool ensureMainFbr = (ConvertThreadToFiber(nullptr), true); !ensureMainFbr) {
				g_logger->send(Magenta, "DxFbr", "Failed to ensureMainFbr");
				return;
			}
			std::lock_guard lck(m_mutex);
			for (auto& scr : m_fibers)
				scr.second->tick();
		}
		std::recursive_mutex m_mutex;
		std::map<char const*, std::unique_ptr<script>> m_fibers{};
	};
	inline dxFiberMgr g_dxFiberMgr;
}