#include "script_mgr.hpp"
#include "../Common.hpp"
#include "../GTAV-Classes-master/script/scrThread.hpp"
#include "../GTAV-Classes-master/script/tlsContext.hpp"
#include "../Invoker.hpp"
#include "../Pointers.hpp"
#include "../GtaUtill.hpp"
namespace base
{
	script* script_mgr::add_script(std::unique_ptr<script> script)
	{
		std::lock_guard lock(m_mutex);
		auto* ret = script.get();
		m_scripts.push_back(std::move(script));
		return ret;
	}

	void script_mgr::remove_script(script* scr)
	{
		std::lock_guard lock(m_mutex);
		std::erase_if(m_scripts, [scr](auto& iter) {
			return iter.get() == scr;
			});
	}

	void script_mgr::remove_all_scripts()
	{
		std::lock_guard lock(m_mutex);
		m_scripts.clear();
	}

	bool script_mgr::doesFiberExist(script* scr)
	{
		auto it = std::find_if(m_scripts.begin(), m_scripts.end(), [scr](const auto& ptr) {
			return ptr.get() == scr;
			});

		return it != m_scripts.end();
	}

	script_list& script_mgr::scripts()
	{
		return m_scripts;
	}

	void script_mgr::tick()
	{
		gta_util::execute_as_script("main_persistent"_joaat, std::mem_fn(&script_mgr::tick_internal), this);
	}

	void script_mgr::ensure_main_fiber()
	{
		ConvertThreadToFiber(nullptr);
		m_can_tick = true;
	}

	void script_mgr::tick_internal()
	{
		static bool ensure_it = (ensure_main_fiber(), true);

		std::lock_guard lock(m_mutex);
		for (auto const& script : m_scripts)
			if (script->is_enabled())
				script->tick();
	}
}