#pragma once
#include "Common.hpp"
#include "HybridThread.hpp"
#include "Pointers.hpp"
namespace base {
	template <typename call, typename ...arguments>
		requires isInvokableWithArgs<call, arguments...>
	bool executeUnderScr(GtaThread* scr, call&& callback, arguments&&... args) {
		auto tlsCtx = rage::tlsContext::get();
		if (!scr || !scr->m_serialised.m_thread_id)
			return false;
		auto ogThr = tlsCtx->m_script_thread;
		tlsCtx->m_script_thread = scr;
		tlsCtx->m_is_script_thread_active = true;
		std::invoke(std::forward<call>(callback), std::forward<arguments>(args)...);
		tlsCtx->m_script_thread = ogThr;
		tlsCtx->m_is_script_thread_active = ogThr != nullptr;
		return true;
	}
	inline rageAlt::scrThread** g_cachedScrThread{};
	inline rageAlt::sysMemAllocator** g_cachedScrAllocator{};
	inline void** g_cachedTlsEntry{};
	inline bool callThreadCacheInsideANonScriptHook{};
	inline void cacheThreadHandles() {
		g_cachedScrAllocator = rageAlt::sysMemAllocator::getPointer();
		g_cachedTlsEntry = rageAlt::sysMemAllocator::getEntryPointer();
		g_cachedScrThread = rageAlt::scrThread::getPointer();
	}
	template <typename call, typename ...arguments>
		requires isInvokableWithArgs<call, arguments...>
	void executeUnderHybridThr(call&& callback, arguments&&... args) {
		if (!g_cachedScrAllocator || !g_cachedTlsEntry || !g_cachedScrThread) {
			if (!callThreadCacheInsideANonScriptHook)
				callThreadCacheInsideANonScriptHook = true;
			return;
		}
		static auto tlsCtx = rageAlt::tlsContext::get();
		if (g_cachedScrAllocator != nullptr) {
			auto ogThr = rageAlt::scrThread::get();
			auto ogAlloc = rageAlt::sysMemAllocator::get();
			auto ogATlsEntry = rageAlt::sysMemAllocator::getEntry();
			tlsCtx->m_tls_entry = (rageAlt::sysMemAllocator*)*g_cachedTlsEntry;
			tlsCtx->m_script_thread = *g_cachedScrThread;
			tlsCtx->m_is_script_thread_active = true;
			tlsCtx->m_allocator = *g_cachedScrAllocator;
			std::invoke(std::forward<call>(callback), std::forward<arguments>(args)...);
			tlsCtx->m_tls_entry = (rageAlt::sysMemAllocator*)ogATlsEntry;
			tlsCtx->m_script_thread = ogThr;
			tlsCtx->m_is_script_thread_active = ogThr != nullptr;
			tlsCtx->m_allocator = ogAlloc;
		}
	}
	inline auto getThr = [=](char const* name) -> GtaThread* {
		for (auto& thr : *g_pointers->m_gtaThreads) {
			if (thr->m_name == name)
				return thr;
		}
		return nullptr;
	};
	inline auto getThrUsingHash = [=](uint32_t hash) -> GtaThread* {
		for (auto& thr : *g_pointers->m_gtaThreads) {
			if (thr && thr->m_instance_id && thr->m_handler && thr->m_script_hash == hash)
				return thr;
		}
		return nullptr;
	};
}