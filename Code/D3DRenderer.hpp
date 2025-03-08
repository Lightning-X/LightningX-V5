#pragma once
#include "Common.hpp"
#include "Logger.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

namespace base {
	struct vec2 { int x, y; };
	struct img { unsigned char* data; vec2 v2; };
	struct imgData { unsigned char* imgBytes; int imgLen; int frameDelay; };
	struct frameData { int frameDelay; ID3D11ShaderResourceView* resView; };
	class rendererQueue {
	public:
		void tick() {
			for (auto& fn : m_funcs) {
				fn.second();
			}
		}
		void createInputLockFiber(std::string id);
		void removeInputLockFiber(std::string id);
		void add(std::string id, std::function<void()> func) {
			if (m_funcs.empty())
				m_funcs.insert({ id, std::move(func) });
		}
		void remove(std::string id) {
			m_funcs.erase(id);
			removeInputLockFiber(id);
		}
	public:
		std::map<std::string, std::function<void()>> m_funcs;
	};
	inline rendererQueue g_rendererQueue;
	class renderer {
	public:
		renderer();
		~renderer();
		ID3D11ShaderResourceView* createTexture(fs::path filePath);
		std::map<int, frameData> createGifTexture(fs::path path);
		ID3D11ShaderResourceView* createResourceView(unsigned char* imgData, vec2 imgSize);
		void present();
		void wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		int m_headerFrame = 0;
		std::map<int, frameData> m_header;
		ImFontConfig m_fontCfg;
		ImFont* m_font{};
		ImFont* m_big_font{};
		ImFont* m_leftOptionFont{};
		ImFont* m_overlayFont{};
		ImFont* m_notifyFont{};
		ImFont* m_headerFont{};
		ImFont* m_subtitleFont{};
		//ImFont* m_iconFont{};
		//ImFont* m_bigIconFont{};
		ImFont* m_bigfontawesome{};
		ImFont* m_footerFont{};
		ImFont* m_infoboxFont{};
		ImFont* m_iconFontMain{};
		ImFont* m_notificationFont{};
		ImFont* m_RD2Font{};
		ImFont* m_ClicKTitleFont{};
		comPtr<IDXGISwapChain> m_swapchain;
		comPtr<ID3D11Device> m_device;
		comPtr<ID3D11DeviceContext> m_context;
	};
	inline std::unique_ptr<renderer> g_renderer;
}