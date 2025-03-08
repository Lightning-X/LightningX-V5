#pragma once
#include "Pointers.hpp"
#include "Crossmap.hpp"
#include "Logger.hpp"
#include "GTAV-Classes-master/script/scrNativeHandler.hpp"
#include "GTAV-Classes-master/script/scrNativeRegistrationTable.hpp"
#include "GTAV-Classes-master/script/scrNativeRegistration.hpp"
#include "RobinHood.hpp"
namespace base {
	struct nativeCtx : public rage::scrNativeCallContext {
		nativeCtx() : scrNativeCallContext() {
			m_return_value = &m_return_stack[0];
			m_args = &m_arg_stack[0];
		}
	private:
		std::uint64_t m_return_stack[10] = {};
		std::uint64_t m_arg_stack[100] = {};
	};
	class invoker {
	public:
		void map() {
			for (const rage::scrNativePair& mapping : g_crossmap) {
				rage::scrNativeHandler handler = g_pointers->m_getNativeHandler(g_pointers->m_nativeRegTbl, mapping.second);
				m_cache.emplace(mapping.first, handler);
			}
		}
		void begin() { m_context.reset(); }
		void end(rage::scrNativeHash hash) {
			if (auto it = m_cache.find(hash); it != m_cache.end()) {
				rage::scrNativeHandler handler = it->second;
				[this, hash, handler] {
					__try
					{
						// return address checks are no longer a thing
						handler(&m_context);
						g_pointers->m_fixVectors(&m_context);
					}
					__except (EXCEPTION_EXECUTE_HANDLER)
					{
						[hash]() {
							g_logger->send(red, "Invoker", "Exception caught while trying to call: 0x{:X} native.", hash);
						}();
					}
				}();
			}
			else
			{
				[hash]() {
					g_logger->send(red, "Invoker", "Failed to find 0x{:X} native's handler.", hash);
				}();
			}
		}
		template<typename T>
		void push_arg(T&& value) {
			m_context.push_arg(std::forward<T>(value));
		}
		template<typename T>
		T& get_return_value() {
			return *m_context.get_return_value<T>();
		}
	public:
		ankerl::unordered_dense::map<rage::scrNativeHash, rage::scrNativeHandler> m_cache;
		nativeCtx m_context;
	};
	inline invoker g_invoker;
}