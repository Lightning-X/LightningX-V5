#pragma once
#include "../Common.hpp"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../D3DRenderer.hpp"
#include "../Natives.hpp"
#include "timer.hpp"
#include "baseSub.hpp"
#include "abstractSub.hpp"
#include "abstractOpt.hpp"
#include "smoothScroll.hpp"
#include "../Notify.hpp"
#include "../Fibers/fiber_pool.hpp"
#include "../ImVec2Operations.hpp"
#include "../Services/Player/player_service.hpp"
inline std::vector<std::string> getWords(std::string str) {
	std::vector<std::string> words;
	std::string word;
	for (int i = 0; i < str.length(); i++) {
		if (str.at(i) == ' ') {
			words.push_back(word);
			word.erase();
		}
		else {
			word += str.at(i);
			if (i == str.length() - 1)
				words.push_back(word);
		}
	}
	return words;
}
inline std::vector<std::string> getWordsSimple(const std::string& str) {
	std::vector<std::string> words;
	words.reserve(std::count(str.begin(), str.end(), ' ') + 1);
	std::string word;
	for (const auto& s : str) {
		if (s != ' ')
			word += s;
		else {
			words.emplace_back(std::move(word));
			word.clear();
		}
	}
	if (!word.empty())
		words.emplace_back(std::move(word));
	return words;
}
namespace base::listUi {
	using namespace listUiTypes;
	inline std::string lastMsg;
	class dxUiManager {
	public:
		//Handle Drawing
		void handleDrawing();
	public:
		//Draw Handlers
		// Header Handler
		void headerHandler();
		// Subtitle Handler
		void subtitleHandler(abstractSub* sub);
		// Option Handler
		void optionHandler(abstractOpt* opt, bool isOpSelected);
		// SmoothScroll Handler
		void smoothScrollHandler();
		// Footer Handler
		void footerHandler();
		// Description Handler
		void descriptionHandler();
		// Overlay Handler
		void overlayHandler();
		// Notification handler
		void ImguiSpinner();
	public:
		//Draw Helpers
		ImVec2 vecAdd(ImVec2 vectorA, ImVec2 vectorB);
		// Rect Helper
		void rect(ImVec2 pos, ImVec2 size, ImU32 color, bool shouldHaveScaling = true, ImDrawList* drawList = ImGui::GetBackgroundDrawList());
		// Rect Outline Helper
		void rectOutline(ImVec2 pos, ImVec2 size, ImU32 color, bool shouldHaveScaling = true, float thickness = 1.25f);
		// MultiColor
		void renderMulticolorRect(ImVec2 pos, ImVec2 size, ImU32 color1, ImU32 color2, ImU32 color3, ImU32 color4, bool shouldHaveScaling = true);
		// Line Helper
		void line(ImVec2 pos, ImVec2 size, ImU32 color, bool shouldHaveScaling = true, float thickness = 1.f);
		// Image Helper
		void image(ImTextureID image, ImVec2 pos, ImVec2 size, ImU32 color = IM_COL32(255, 255, 255, 255), bool shouldHaveScaling = true);
		// Text Helper
		void text(std::string text, ImVec2 pos, ImU32 color, ImFont* font = g_renderer->m_leftOptionFont, bool shouldHaveScaling = true);
		// Width Helper
		void width();
	public:
		//Input Handlers
		// Reset Input Handler
		void resetInputHandler();
		// Input Check Handler
		void inputCheckHandler();
		// Keypress Handler
		void keypressHandler();
	public:
		//Submenu Handlers
		// Sub Adding
		void addSub(sub&& submenu) {
			auto newSub = new sub(std::move(submenu));
			m_allSubmenus.emplace_back(newSub);
			if (m_submenuStack.empty())
				m_submenuStack.push(newSub);
		}
		// Sub Switching
		void switchToSub(subId subid) {
			for (auto&& const sub : m_allSubmenus) {
				if (sub->getId().cmp(subid)) {
					m_submenuStack.push(sub.get());
					m_currentOption = 0;
					return;
				}
			}
		}
		// Get Sub Data
		sub* getSubData(subId subid) {
			for (auto&& const sub : m_allSubmenus) {
				if (sub->getId().cmp(subid))
					return sub.get();
			}
			return nullptr;
		}
	public:
		//General Handlers (Called all across the ui).
		// Pos X + Y
		ImVec2 m_pos = { 210.f, 45.f };
		// Width
		float m_width = 400.f;
		// Menu Open
		bool m_open = true;

		// Force lock input
		bool m_forceLockInput = false;
		//Key Handlers
		// Keypress Handlers
		bool m_openKeyPressed = false;
		bool m_backKeyPressed = false;
		bool m_enterKeyPressed = false;
		bool m_upKeyPressed = false;
		bool m_downKeyPressed = false;
		bool m_leftKeyPressed = false;
		bool m_rightKeyPressed = false;
		// Keypress Delay Handlers
		size_t m_openDelay = 20; //Open/Close Delay
		size_t m_backDelay = 20; //Back Delay
		size_t m_enterDelay = 20; //Enter Delay
		size_t m_verticalDelay = 10; //Left/Right Delay
		size_t m_horizontalDelay = 10; //Up/Down Delay
		// Key handler
		void checkIfPressed(bool& value, int padIdx, int key, int keyboardKey, size_t delay);
		void checkIfPressed(bool& value, int padIdx, int key, int key2, int keyboardKey, size_t delay);
		// Basic key handler
		auto checkIfPressed(int key) const { return (GetForegroundWindow() == FindWindowA("grcWindow", nullptr)) && (GetAsyncKeyState(key) & 1); }
		// Vector math
		ImVec2 addVecs(ImVec2 vectorA, ImVec2 vectorB) { return { vectorA.x + vectorB.x, vectorA.y + vectorB.y }; }
		//Playerinfo Handler
		// Draw title handler
		void drawTitle(const std::string& playerName);
		// Draw line handler
		void drawLine(const std::string& name, const std::string& data);
		// Draw section line handler
		void drawSectionLine(std::string name, std::string data, std::string name2, std::string data2);
		// Draw line shit
		float m_numberOfLines{};
		// Player Info
		void drawPlayerinfo(player_ptr plyr);
	public:
		//Sound Handlers
		// Queue Sound
		void soundQueue(std::string const& soundName) const {
			g_fiber_pool->queue_job([soundName] {
				AUDIO::PLAY_SOUND_FRONTEND(-1, soundName.c_str(), "HUD_FRONTEND_DEFAULT_SOUNDSET", false);
				});
		}
		// Sound disable
		bool m_enableSounds = true;
	public:
		//Header Handlers
		// Enable header text
		bool m_enableHeaderText = true;
		// Header loaded check
		bool m_hasHeaderLoaded = false;
		// Header Loading text
		bool m_headerloading = false;
	public:
		//Option Handlers
		// Current option in op ount
		size_t m_currentOption = 0;
		// Full option count
		float m_optionCount = 0.f;
		// Max visble options
		int m_maxVisOptions = 11;
		// Smooth scroll speed
		float m_scrollSpeed = 0.1281f;
		// Menu Scale
		float m_scale = 1.f;
		//Key Handlers
		float m_drawBase{ 0.f };
	public:
		//Infobox Handlers
		// Infobox text
		std::string m_infoText;
	public:
		//Submenu Handlers
		// Submenu List
		std::vector<std::unique_ptr<sub>> m_allSubmenus;
		// Submenu Stack
		std::stack<sub*> m_submenuStack;
	public:
		//Scaling Handlers
	// Font scale handler
		void scale();
		// Screen Resolution
		auto getRes() const {
			return ImGui::GetIO().DisplaySize;
		}
		// Screen Resolution Used In Creation Of Menu
		// Used to keep scaling the same across multiple monitors and resolutions
		auto getResUsedByDeveloper() {
			return ImVec2(1920.f, 1080.f);
		}
		// Convert to floating point
		auto getFloatingPoint(float value, ImVec2 scalingRes, bool isX = true) {
			auto res = isX ? scalingRes.x : scalingRes.y;
			return float(value / res);
		}
		// Convert to floating point (ImVec2)
		auto getFloatingPoint(ImVec2 vec, ImVec2 scalingRes) {
			return ImVec2(vec.x / scalingRes.x, vec.y / scalingRes.y);
		}
		// Convert to floating point (Vector2)
		auto getFloatingPoint(Vector2 vec, ImVec2 scalingRes) {
			return Vector2(vec.x / scalingRes.x, vec.y / scalingRes.y);
		}
		// Convert to floating point (ImVec2->Vector2)
		auto getFloatingPointFromImVec2(ImVec2 vec, ImVec2 scalingRes) {
			return Vector2(vec.x / scalingRes.x, vec.y / scalingRes.y);
		}
		// Get position values as floating points
		auto getPositionValues() {
			return getFloatingPointFromImVec2(m_pos, getRes());
		}
		// Get position values as floating points with an offset
		auto getPositionValues(float offset) {
			return getFloatingPointFromImVec2(ImVec2(m_pos.x + offset, m_pos.y + offset), getRes());
		}
		// Ensure scaling
		auto scale(float x, float y, ImVec2 scalingRes, bool shouldHaveScaling = true) {
			auto scale = shouldHaveScaling ? m_scale : 1.f;
			return ImVec2((getFloatingPoint(x, scalingRes) * scalingRes.x) * scale, (getFloatingPoint(y, scalingRes, false) * scalingRes.y) * scale);
		}
		// Ensure scaling (single float)
		auto scale(float value, ImVec2 scalingRes, bool isX = true, bool shouldHaveScaling = true) {
			auto res = isX ? scalingRes.x : scalingRes.y;
			auto scale = shouldHaveScaling ? m_scale : 1.f;
			return (getFloatingPoint(value, scalingRes, isX) * res) * scale;
		}
		// Ensure scaling (ImVec2)
		auto scale(ImVec2 vec, ImVec2 scalingRes, bool shouldHaveScaling = true) {
			auto scale = shouldHaveScaling ? m_scale : 1.f;
			return ImVec2((getFloatingPoint(vec.x, scalingRes) * scalingRes.x) * scale, (getFloatingPoint(vec.y, scalingRes, false) * scalingRes.y) * scale);
		}
		ImVec2 convertCoordTypes(ImVec2 pos, bool isDC) {
			if (isDC) {
				return pos / getRes();
			}
			return pos * getRes();
		}
	public:
		//Sizes
		ImVec2 m_headerRectSize = { m_width, 100.f }; //Header Size
		ImVec2 m_submenuRectSize = { m_width, 37.f }; //Submenu Size
		ImVec2 m_optionRectSize = { m_width, 35.f }; //Option Size
		ImVec2 m_playerinfoRectSize = { 455.f, 35.f }; //Playerinfo Rect Size
		ImVec2 m_footerRectSize = { m_width, 37.f }; //Footer Size
		ImVec2 m_infoboxRectSize = { m_width, 35.f }; //Infobox Size
		ImVec2 m_notifyRectSize = { 420.f, 40.f }; //Notify Size
		ImVec2 m_notifyLineSize = { 2.f, 385.f }; //Notify line Size
		ImVec2 m_overlayRectSize = { 400.f, 35.f }; //Overlay Size
		ImVec2 m_transitionRectSize = { 380.f, 45.f }; //Transition Size
		ImVec2 m_OverlayInfoRectSize = { 200.f, 150.f }; //Overlay Info Size
	public:
		//Colors
		ImU32 m_entireMenuColor = IM_COL32(76, 146, 186, 255); //Open tooltip text color
		ImU32 m_headerRectColor = IM_COL32(76, 146, 186, 255); //Header bg color
		ImU32 m_subtitleRectColor = IM_COL32(10, 10, 10, 230); //Subtitle bg color
		ImU32 m_optionRectColor = IM_COL32(14, 14, 14, 230); //Option bg color
		ImU32 m_footerRectColor = IM_COL32(10, 10, 10, 230); //Footer bg color
		ImU32 m_infoboxRectColor = IM_COL32(10, 10, 10, 225); //Infobox bg color
		ImU32 m_infoboxTopRectColor = IM_COL32(76, 146, 186, 205); //Infobox bg color
		ImU32 m_overlayRectColor = IM_COL32(4, 4, 4, 210); //Overlay bg color
		ImU32 m_headerTextColor = IM_COL32(255, 255, 255, 255); //Header txt color
		ImU32 m_subtitleTextColor = IM_COL32(255, 255, 255, 255); //Subtitle txt color
		ImU32 m_optionTextColor = IM_COL32(255, 255, 255, 255); //Option txt color
		ImU32 m_footerTextColor = IM_COL32(255, 255, 255, 255); //Footer txt color (arrows)
		ImU32 m_footerArrowColor = IM_COL32(255, 255, 255, 255); //Footer txt color (arrows)
		ImU32 m_infoboxTextColor = IM_COL32(255, 255, 255, 255); //Infobox txt color
		ImU32 m_grayedTextColor = IM_COL32(80, 80, 80, 255); //Grayed txt color
		ImU32 m_optionSelectedRectColor = IM_COL32(255, 255, 255, 255); //Option selected bg color
		ImU32 m_optionSelectedTextColor = IM_COL32(10, 10, 10, 255); //Option selected txt color
		ImU32 m_vehicleColor = IM_COL32(0, 0, 0, 255); //Option selected txt color
	};
}
inline base::listUi::dxUiManager g_dxUiManager;