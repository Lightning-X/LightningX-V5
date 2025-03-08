#include "script_function.hpp"
#include "GtaUtill.hpp"
#include "Pointers.hpp"
#include "GTAV-Classes-master/script/scrProgram.hpp"
#include "GTAV-Classes-master/script/scrProgramTable.hpp"
#include "Fibers/script.hpp"
#include "Fibers/fiber_pool.hpp"
namespace base
{
	script_function::script_function(const std::string& name, const rage::joaat_t script, const std::string& pattern, int32_t offset) :
		m_script(script),
		m_pattern(pattern),
		m_offset(offset),
		m_ip(0),
		m_name(name)
	{
	}

	void script_function::populate_ip()
	{
		if (m_ip == 0)
		{
			auto program = gta_util::find_script_program(m_script);

			if (!program)
				return;

			auto location = gta_util::get_code_location_by_pattern(program, m_pattern);

			if (!location)
				g_logger->send(red, "Script Patcher", "Failed to find pattern {} in script {}", m_name, program->m_name);
			else
				g_logger->send(green, "Script Patcher", "Found pattern {} in script {}", m_name, program->m_name);

			m_ip = location.value() + m_offset;
		}
	}

	void script_function::call(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<rage::scrValue> args)
	{
		auto tls_ctx = rage::tlsContext::get();
		auto stack = thread->m_stack;
		auto og_thread = tls_ctx->m_script_thread;

		tls_ctx->m_script_thread = thread;
		tls_ctx->m_is_script_thread_active = true;

		rage::scrThread::Serialised ser = thread->m_serialised;

		for (auto& arg : args)
			stack[ser.m_stack_pointer++] = arg;

		stack[ser.m_stack_pointer++] = (rage::scrValue)0;
		ser.m_pointer_count = m_ip;
		ser.m_state = rage::eThreadState::running;

		g_pointers->m_scriptVirtualMachine(stack, g_pointers->m_globalBase, program, &ser);

		tls_ctx->m_script_thread = og_thread;
		tls_ctx->m_is_script_thread_active = og_thread != nullptr;
	}

	void script_function::call_latent(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<rage::scrValue> args, bool& done)
	{
		g_fiber_pool->queue_job([this, thread, program, args, &done] {
			auto stack = thread->m_stack;

			rage::eThreadState result = rage::eThreadState::running;

			rage::scrThread::Serialised ser = thread->m_serialised;

			for (auto& arg : args)
				stack[ser.m_stack_pointer++] = arg;

			stack[ser.m_stack_pointer++] = (rage::scrValue)0;
			ser.m_pointer_count = m_ip;
			ser.m_state = rage::eThreadState::running;

			while (result != rage::eThreadState::killed)
			{
				auto tls_ctx = rage::tlsContext::get();
				auto og_thread = tls_ctx->m_script_thread;

				tls_ctx->m_script_thread = thread;
				tls_ctx->m_is_script_thread_active = true;

				auto const& old_ctx = thread->m_serialised;
				thread->m_serialised = ser;
				result = g_pointers->m_scriptVirtualMachine(stack, g_pointers->m_globalBase, program, &thread->m_serialised);
				thread->m_serialised = old_ctx;

				tls_ctx->m_script_thread = og_thread;
				tls_ctx->m_is_script_thread_active = og_thread != nullptr;

				script::get_current()->yield();
			}

			done = true;
			});
	}

	void script_function::static_call(std::initializer_list<rage::scrValue> args)
	{
		populate_ip();

		auto* thread = (rage::scrThread*)new uint8_t[sizeof(rage::scrThread)];
		memcpy(thread, rage::tlsContext::get()->m_script_thread, sizeof(rage::scrThread));

		auto* stack = new rage::scrValue[25000];
		thread->m_stack = stack;
		thread->m_serialised.m_stack_size = 25000;
		thread->m_serialised.m_stack_pointer = 1;

		call(thread, gta_util::find_script_program(m_script), args);

		delete[] stack;
		delete[](uint8_t*) thread; // without the cast it ends up calling the destructor which leads to some pretty funny crashes
	}

	void script_function::operator()(std::initializer_list<rage::scrValue> args)
	{
		populate_ip();

		if (m_ip == 0)
			return;

		auto thread = gta_util::find_script_thread(m_script);
		auto program = gta_util::find_script_program(m_script);

		if (thread && program)
		{
			call(thread, program, args);
		}
	}
}