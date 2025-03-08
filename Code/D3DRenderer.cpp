#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/STBImage.hpp"
#include "STB/STBImageWrite.hpp"
#include "D3DRenderer.hpp"
#include "Pointers.hpp"
#include <D3DX11tex.h>
#include "GuiScript.hpp"
#include "ui/dxUiManager.hpp"
#include "MainScript.hpp"
#include "Fibers/script.hpp"
#include "Fibers/script_mgr.hpp"
#include "DxFiber.hpp"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace base {
	imgData writePngToMem(int x, int y, int comp, const void* data, int strideBytes, int delay) {
		int len;
		unsigned char* png = stbi_write_png_to_mem((const unsigned char*)data, strideBytes, x, y, comp, &len);
		if (!png)
			return {};
		return { png, len, delay };
	}
	img loadImgFromMem(const imgData& data) {
		int width, height;
		auto img = stbi_load_from_memory(data.imgBytes, data.imgLen, &width, &height, nullptr, 0);
		if (img == nullptr) {
			return {};
		}
		return { img, { static_cast<int>(std::round(width)), static_cast<int>(std::round(height)) } };
	}
	std::map<int, imgData> loadGif(const fs::path& path) {
		std::map<int, imgData> frames;
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			return frames;
		}
		file.seekg(0, std::ios::end);
		std::streampos size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<char> buffer(size);
		if (!file.read(buffer.data(), size)) {
			return frames;
		}
		int* delays = nullptr;
		int width, height, frameCount, comp;
		auto data = stbi_load_gif_from_memory((stbi_uc*)buffer.data(), static_cast<int>(size), &delays, &width, &height, &frameCount, &comp, 0);
		if (!data) {
			return frames;
		}
		size_t bytes = width * comp;
		for (size_t i = 0; i < frameCount; i++) {
			frames.insert({ static_cast<int>(i), writePngToMem(width, height, comp, data + bytes * height * i, 0, delays[i]) });
		}
		stbi_image_free(data);
		return frames;
	}
	renderer::renderer() : m_swapchain(*g_pointers->m_swapchain) {
		if (FAILED(m_swapchain->GetDevice(__uuidof(ID3D11Device), (void**)m_device.GetAddressOf())))
			g_logger->send(logColor::red, "Renderer", "Failed to create SwapChain device");

		m_device->GetImmediateContext(m_context.GetAddressOf());
		ImGui::CreateContext();
		ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);
		auto& Style = ImGui::GetStyle();
		auto& Colors = Style.Colors;
		Style.Alpha = 1.f;
		Style.WindowPadding = ImVec2(2, 2);
		Style.PopupRounding = 0.f;
		Style.FramePadding = { 8.f, 4.f };
		Style.ItemSpacing = ImVec2(4, 4);
		Style.ItemInnerSpacing = ImVec2(4, 4);
		Style.TouchExtraPadding = { 0.f, 0.f };
		Style.IndentSpacing = 10.f;
		Style.ScrollbarSize = 20.f;
		Style.ScrollbarRounding = 8.f;
		Style.GrabMinSize = 90.f;
		Style.GrabRounding = 12.25f;
		Style.WindowBorderSize = 1.0f;
		Style.ChildBorderSize = 0.f;
		Style.PopupBorderSize = 0.f;
		Style.FrameBorderSize = 0.f;
		Style.TabBorderSize = 0.f;
		Style.WindowRounding = 5.f;
		Style.ChildRounding = 3.f;
		Style.FrameRounding = 4.25f;
		Style.TabRounding = 2.f;
		Style.MouseCursorScale = 1.2f;
		Style.WindowTitleAlign = { 0.5f, 0.5f };
		Style.ButtonTextAlign = { 0.5f, 0.5f };
		Style.DisplaySafeAreaPadding = { 2.f, 2.f };
		Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		Colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 0.90f, 0.19f, 1.00f);
		Colors[ImGuiCol_WindowBg] = ImColor(20, 20, 20, 255);
		Colors[ImGuiCol_ChildBg] = ImColor(77, 78, 82, 101);
		Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		Colors[ImGuiCol_Border] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_BorderShadow] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_FrameBg] = ImColor(77, 78, 82, 101);
		Colors[ImGuiCol_FrameBgActive] = ImColor(77, 78, 82, 101);
		Colors[ImGuiCol_FrameBgHovered] = ImColor(77, 78, 82, 101);
		Colors[ImGuiCol_TitleBg] = ImColor(50, 50, 50, 255);
		Colors[ImGuiCol_TitleBgActive] = ImColor(50, 50, 50, 255);
		Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_MenuBarBg] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_ScrollbarBg] = ImColor(0, 0, 0, 255);
		Colors[ImGuiCol_ScrollbarGrab] = ImColor(74, 115, 181, 255);
		Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(74, 115, 181, 255);
		Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_CheckMark] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_SliderGrab] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_SliderGrabActive] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_Button] = ImColor(50, 50, 50, 255);
		Colors[ImGuiCol_ButtonHovered] = ImColor(29, 133, 173, 255);
		Colors[ImGuiCol_ButtonActive] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_Header] = ImVec4(0.37f, 0.37f, 0.37f, 0.31f);
		Colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.38f, 0.38f, 0.37f);
		Colors[ImGuiCol_HeaderActive] = ImVec4(0.37f, 0.37f, 0.37f, 0.51f);
		Colors[ImGuiCol_Separator] = ImColor(0, 183, 254, 200);
		Colors[ImGuiCol_SeparatorHovered] = ImColor(0, 183, 254, 200);
		Colors[ImGuiCol_SeparatorActive] = ImColor(0, 183, 254, 200);
		Colors[ImGuiCol_ResizeGrip] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_ResizeGripHovered] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_ResizeGripActive] = ImColor(0, 183, 254, 255);
		Colors[ImGuiCol_Tab] = ImColor(0, 183, 250, 245);
		Colors[ImGuiCol_TabHovered] = ImColor(0, 183, 250, 245);
		Colors[ImGuiCol_TabActive] = ImColor(0, 183, 250, 255);
		Colors[ImGuiCol_TabUnfocused] = ImColor(103, 0, 250, 245);
		Colors[ImGuiCol_TabUnfocusedActive] = ImColor(103, 0, 245, 245);
		Colors[ImGuiCol_PlotLines] = ImColor(103, 0, 255, 255);
		Colors[ImGuiCol_PlotLinesHovered] = ImColor(255, 255, 255, 255);
		Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		Colors[ImGuiCol_TextSelectedBg] = ImColor(255, 255, 255, 255);
		Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		Colors[ImGuiCol_NavHighlight] = ImColor(255, 255, 255, 255);
		Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		m_fontCfg.FontDataOwnedByAtlas = false;
		auto& io = ImGui::GetIO();

		io.IniFilename = NULL;

		m_font           = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Arial.ttf)", 19.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_leftOptionFont = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Arial.ttf)", 19.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_big_font       = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Arial.ttf)", 30.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_overlayFont    = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Arial.ttf)", 17.5f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_notifyFont     = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\ArialBD.ttf)", 19.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_subtitleFont   = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\ArialBD.ttf)", 19.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_footerFont     = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\ArialI.ttf)", 18.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_infoboxFont    = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Arial.ttf)", 17.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());

		m_headerFont     = io.Fonts->AddFontFromFileTTF(R"(C:\LightningFiles\Fonts\TitleFont.ttf)", 40.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_ClicKTitleFont = io.Fonts->AddFontFromFileTTF(R"(C:\LightningFiles\Fonts\GemunuLibre-SemiBold.ttf)", 20.5f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_RD2Font        = io.Fonts->AddFontFromFileTTF(R"(C:\LightningFiles\Fonts\chinese_rocks_rg.ttf)", 25.f, &m_fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_iconFontMain   = io.Fonts->AddFontFromFileTTF(R"(C:\LightningFiles\Fonts\IconFont.ttf)", 15.f, &m_fontCfg);
		m_bigfontawesome = io.Fonts->AddFontFromFileTTF(R"(C:\LightningFiles\Fonts\Font_Awesome.otf)", 30.0f, &m_fontCfg);


		ImGui::MergeIconsWithLatestFont(25.f, false);
	}
	void rendererQueue::createInputLockFiber(std::string id) {
		g_fbrMgr.add(std::make_unique<script>([] {
			while (true) {
				PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
				script::get_current()->yield();
			}
			}), id.c_str());
	}
	void rendererQueue::removeInputLockFiber(std::string id) {
		g_fbrMgr.remove(id.c_str());
	}
	renderer::~renderer() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	void renderer::present() {
		ImGui::GetIO().MouseDrawCursor = g_dxUiManager.m_open && !g_mainScript.m_listUi;
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		g_dxFiberMgr.tick();
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	ID3D11ShaderResourceView* renderer::createTexture(fs::path filePath) {
		vec2 v2{};
		unsigned char* imgData = stbi_load(filePath.string().c_str(), &v2.x, &v2.y, nullptr, 4);
		if (imgData == nullptr) {
			return nullptr;
		}
		return createResourceView(imgData, v2);
	}
	std::map<int, frameData> renderer::createGifTexture(fs::path path) {
		auto gifData = loadGif(path);
		std::map<int, frameData> tmpArr;
		for (const auto& fr : gifData) {
			auto img = loadImgFromMem(fr.second);
			tmpArr.try_emplace(fr.first, fr.second.frameDelay, createResourceView(img.data, img.v2));
		}
		return tmpArr;
	}
	ID3D11ShaderResourceView* renderer::createResourceView(unsigned char* imgData, vec2 imgSize) {
		if (!imgData) return nullptr;
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = imgSize.x;
		desc.Height = imgSize.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		ID3D11Texture2D* pTexture = nullptr;
		D3D11_SUBRESOURCE_DATA subResource = { imgData, desc.Width * 4, 0 };
		m_device.Get()->CreateTexture2D(&desc, &subResource, &pTexture);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		ID3D11ShaderResourceView* tmpRes = nullptr;
		m_device.Get()->CreateShaderResourceView(pTexture, &srvDesc, &tmpRes);
		pTexture->Release();
		return tmpRes;
	}
	void renderer::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (ImGui::GetCurrentContext())
			ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}
}