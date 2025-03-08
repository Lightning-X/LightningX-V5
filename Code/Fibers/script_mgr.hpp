#pragma once
#include "../Common.hpp"
#include "script.hpp"
#include "../GtaUtill.hpp"
namespace base
{
	using script_list = std::vector<std::unique_ptr<script>>;

	class script_mgr
	{
	public:
		explicit script_mgr() = default;
		~script_mgr() = default;

		script* add_script(std::unique_ptr<script> script);
		void remove_script(script* script);
		void remove_all_scripts();
		bool doesFiberExist(script* script);

		script_list& scripts();

		void tick();

		[[nodiscard]] inline bool can_tick() const
		{
			return m_can_tick;
		}

	private:
		void ensure_main_fiber();
		void tick_internal();
		std::recursive_mutex m_mutex;
		script_list m_scripts;
		bool m_can_tick = false;
	};

	inline script_mgr g_script_mgr;

	class fbrMgr {
	public:
		fbrMgr() = default;
		~fbrMgr() = default;
	public:
		void add(std::shared_ptr<script> fbr, std::string fbrId) {
			std::lock_guard lck(m_mutex);
			m_scripts.insert({ fbrId, std::move(fbr) });
			//g_logger->send(logColor::grey, "Fiber Manager", "Registered fiber {}", fbrId);
		}
		void remove(std::string fbrId) {
			std::lock_guard lck(m_mutex);
			if (auto var = m_scripts.find(fbrId); var != m_scripts.end()) {
				m_scripts.erase(fbrId);
				//g_logger->send(logColor::grey, "Fiber Manager", "Removed fiber {}", fbrId);
			}
		}
		void removeAll() {
			std::lock_guard lck(m_mutex);
			m_scripts.clear();
		}
		void scrTick() {
			gta_util::execute_as_script("main_persistent"_joaat, std::mem_fn(&fbrMgr::tick), this);
		}
		void tick() {
			if (static bool ensureMainFbr = (ConvertThreadToFiber(nullptr), true); !ensureMainFbr)
				return;
			std::lock_guard lck(m_mutex);
			for (auto& scr : m_scripts)
				scr.second->tick();
		}
	public:
		std::recursive_mutex m_mutex;
		std::map<std::string, std::shared_ptr<script>> m_scripts;
	};
	inline fbrMgr g_fbrMgr;
}