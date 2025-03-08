#include "Pointers.hpp"
#include "Hooking.hpp"
#include "Invoker.hpp"
#include "LabelManager.hpp"
#include "D3DRenderer.hpp"
#include "Protections.hpp"
#include "GuiScript.hpp"
#include "ui/dxUiManager.hpp"
#include "Features.hpp"
#include "ThreadPool.hpp"
#include "NativeHooking.hpp"
#include "exceptionHandler.hpp"
#include "Translations.hpp"
#include "SpinnerService.hpp"
#include "DxFiber.hpp"
#include "Fibers/fiber_pool.hpp"
#include "Fibers/script_mgr.hpp"
#include "Services/Player/player_service.hpp"
#include "BytePatchMgr.hpp"
#include "Services/ScriptPatcher/script_patcher_service.hpp"
#include "Services/Api/api_service.hpp"
#include "MainScript.hpp"
#include "ScriptPatches.hpp"
BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID) {
	using namespace base;
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hmod);
		g_module = hmod;
		g_main_thread = CreateThread(nullptr, 0, [](PVOID) -> DWORD {
			exceptions::initExceptionHandler();
			g_logger = std::make_unique<logger>();
			auto thread_pool_instance = std::make_unique<thread_pool>();
			auto pointers_instance = std::make_unique<pointers>();
			auto byte_patch_manager_instance = std::make_unique<byte_patch_manager>();
			if (*g_pointers->m_gameState != eGameState::Playing) {
				g_logger->send(logColor::white, "Game State", "GTA5 has not loaded or fully initialized, thread paused.");
				while (*g_pointers->m_gameState != eGameState::Playing)
					std::this_thread::sleep_for(100ms);
				g_logger->send(logColor::white, "Game State", "GTA5 has finished loading, thread resumed.");
			}
			g_invoker.map();
			g_renderer = std::make_unique<renderer>();
			g_hooking = std::make_unique<hooking>();
			auto fiber_pool_instance = std::make_unique<fiber_pool>(11);
			auto api_service_instance = std::make_unique<api_service>();
			auto player_service_instance = std::make_unique<player_service>();
			auto script_patcher_service_instance = std::make_unique<script_patcher_service>();
			g_TranslationManager = std::make_unique<TranslationManager>();
			g_TranslationManager->LoadTranslations("English");
			auto spinner_service_instance = std::make_unique<spinner_service>();
			g_script_mgr.add_script(std::make_unique<script>(&base::guiScript::tick, "GUI", false));
			g_script_mgr.add_script(std::make_unique<script>(&features::tick, "Features", false));
			g_script_mgr.add_script(std::make_unique<script>(&ScriptPatchLoop, "SPLoop", false));
			g_dxFiberMgr.add(std::make_unique<script>([] {
				while (true) {
					g_dxUiManager.handleDrawing();
					if (g_dxUiManager.m_open && g_mainScript.m_listUi)
						g_rendererQueue.tick();
					script::get_current()->yield();
				}
				}), "dS");
			g_hooking->hook();
			auto native_hooks_instance = std::make_unique<native_hooks>();
			g_logger->send(logColor::white, "Welcome", "{} has fully initialized.", brandingName);
			while (g_running) {
				if (GetAsyncKeyState(VK_END))
					g_running = false;
				std::this_thread::sleep_for(500ms);
			}
			g_logger->send(white, TRANSLATE("GOODBYE"), TRANSLATE("MENU_IS_NOW_UNLOADING"), brandingName);
			g_hooking->unhook();
			native_hooks_instance.reset();
			g_script_mgr.remove_all_scripts();
			g_dxFiberMgr.removeAll();
			spinner_service_instance.reset();
			thread_pool_instance->destroy();
			api_service_instance.reset();
			script_patcher_service_instance.reset();
			player_service_instance.reset();
			g_hooking.reset();
			fiber_pool_instance.reset();
			g_renderer.reset();
			byte_patch_manager_instance.reset();
			pointers_instance.reset();
			thread_pool_instance.reset();
			g_TranslationManager.reset();
			exceptions::uninitExceptionHandler();
			g_logger.reset();
			CloseHandle(g_main_thread);
			FreeLibraryAndExitThread(g_module, 0);
			}, nullptr, NULL, &g_main_thread_id);
	}
	return TRUE;
}