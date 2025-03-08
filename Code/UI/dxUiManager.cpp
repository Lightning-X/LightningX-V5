#include "dxUiManager.hpp"
#include "../D3DRenderer.hpp"
#include "../MainScript.hpp"
#include "../ScriptGlobal.hpp"
#include "../GTAV-Classes-master/vehicle/CVehicleModelInfo.hpp"
#include "../Enums.hpp"
#include "../Imgui_Spinner.hpp"
#include "../SpinnerService.hpp"
#include "../Features.hpp"
#include "../Fibers/fiber_pool.hpp"
#include "../GTAV-Classes-master/script/globals/GPBD_FM.hpp"
#include "../GTAV-Classes-master/script/globals/GPBD_FM_3.hpp"
#include "../Translations.hpp"
#include "../Threading.hpp"
#include <algorithm>
namespace base::listUi {
	void dxUiManager::handleDrawing() {
		//Reset the input states, needs to be done here or else it gives a flicker effect when alt tabbed/moving mouse cursor
		resetInputHandler();
		//Handle the scale
		scale();
		//Handle the width
		width();
		//Check if the menu is opened
		if (m_open && g_mainScript.m_listUi) {
			//Draw the header
			headerHandler();
			// Check if the submenu stack is not empty
			if (!m_submenuStack.empty()) {
				auto sub = m_submenuStack.top(); // Grab the first sub off the submenu stack
				sub->resetOptCount(); // Clear the options
				// Queue the handled press in a different thread
				sub->executeOptAdding(); // Handle the actual execution of the press (Addon to the clear options)
				subtitleHandler(sub); // Draw the subtitle
				const size_t numOptions = sub->getNumberOfOptions();
				const size_t maxVisibleOptions = m_maxVisOptions;
				const size_t selectedOption = sub->getSelectedOpt();
				// Check if the number of options isn't 0
				if (numOptions != 0) {
					// Set the start point to 0
					size_t startPoint = 0;
					// Check if the number of options is under the max visible options and set it to number of options, else set to number of options
					size_t endPoint = numOptions > maxVisibleOptions ? maxVisibleOptions : numOptions;
					// Check if the number of options is under the max visible options, and check if the selected option is under or equal to the max visible options
					if (numOptions > maxVisibleOptions && selectedOption >= maxVisibleOptions) {
						// Get the current selected minus the max visible option plus 1
						startPoint = selectedOption - maxVisibleOptions + 1;
						// Set the end point to the current selected option
						endPoint = selectedOption + 1;
					}
					// Create a for loop to add options until it reaches the end point
					for (size_t i = startPoint, j = 0; i < endPoint; ++i, ++j) {
						m_optionCount++;
						optionHandler(sub->getOpt(i), i == selectedOption);
					}
				}
			}
			//Draw the footer
			footerHandler();
			//Draw the description
			descriptionHandler();
		}
		else if (m_open) {
			g_guiScript.onPresentTick();
		}
		//Draw the overlay
		overlayHandler();
		//Draw the notifications
		ImGui::RenderNotifications();
		//Joiner looper
		ImguiSpinner();
	}
	void dxUiManager::ImguiSpinner() {
		if (g_spinner_service->spinner.active) {
			auto pos = ImGui::GetIO().DisplaySize;
			ImGui::SetNextWindowSize({ 380, 90 });
			ImGui::SetNextWindowPos({ pos.x / 2.5f, pos.y / 1.05f });
			rect(ImVec2(pos.x / 2.5f, pos.y / 1.05f), m_transitionRectSize, IM_COL32(10, 10, 10, 200), false); //background for the text and spinner
			if (ImGui::Begin("spinner", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
				ImGui::Spinner("##spinner_", 10.f, 4.f, g_spinner_service->spinner_colors[g_spinner_service->spinner.type]);
				ImGui::SameLine();
				ImGui::Text(g_spinner_service->spinner.text.c_str());
				if (g_spinner_service->spinner.progress_bar && g_spinner_service->spinner.progress <= 1.f) {
					ImGui::BufferingBar("##progress",
						g_spinner_service->spinner.progress,
						ImVec2(380, 7),
						ImColor(36, 119, 242, 169),//87
						g_spinner_service->spinner_colors[g_spinner_service->spinner.type]);
				}
				ImGui::End();
			}
		}
	}
	auto TopLeft = IM_COL32(182, 71, 255, 255);//
	auto BottomLeft = IM_COL32(153, 0, 255, 255);//
	auto TopRight = IM_COL32(0, 213, 255, 255);//69, 224, 255
	auto BottomRight = IM_COL32(69, 224, 255, 255);//0, 213, 255
	auto lastFrameCount = 0;
	void flipThorughFrames(int delay) {
		static timer tick(0ms);
		tick.setDelay(delay);
		if (tick.update() && lastFrameCount != ImGui::GetFrameCount()) {
			lastFrameCount = ImGui::GetFrameCount();
			if (g_renderer->m_header.size() > 1) {
				g_renderer->m_headerFrame = (g_renderer->m_headerFrame + 1) % g_renderer->m_header.size();
			}
		}
	}
	void dxUiManager::headerHandler() {
		if (m_hasHeaderLoaded) {
			flipThorughFrames(g_renderer->m_header[g_renderer->m_headerFrame].frameDelay);
			image(g_renderer->m_header[g_renderer->m_headerFrame].resView, ImVec2(m_pos.x, m_pos.y), m_headerRectSize);
			return;
		}
		if (m_headerloading) {
			auto headerTextOffset = ImGui::CalcTextSize("Loading...").x;
			auto offset = m_pos.x + (((m_width / 2.f) - headerTextOffset) - 8.f);
			rect(ImVec2(m_pos.x, m_pos.y), m_headerRectSize, m_headerRectColor);
			text("Loading...", ImVec2(offset, m_pos.y + ((m_headerRectSize.y / 2.f) - 20.05f)), m_headerTextColor, g_renderer->m_headerFont);
			return;
		}
		renderMulticolorRect(ImVec2(m_pos.x, m_pos.y), m_headerRectSize, BottomRight, BottomLeft, TopLeft, TopRight);
		auto headerTextOffset = ImGui::CalcTextSize(brandingName).x;
		auto offset = m_pos.x + (((m_width / 2.f) - headerTextOffset) - 8.f);
		text(brandingName, ImVec2(offset, m_pos.y + ((m_headerRectSize.y / 2.f) - 20.05f)), m_headerTextColor, g_renderer->m_headerFont);
	}
	void dxUiManager::subtitleHandler(abstractSub* sub) {
		auto optionCount = sub->getNumberOfOptions();
		m_currentOption = sub->getSelectedOpt() + 1;
		auto optionPosText = std::format("{}/{}", std::to_string(m_currentOption), std::to_string(optionCount));
		auto offset = m_width - ImGui::CalcTextSize(optionPosText.c_str()).x - 3.75f;
		rect(ImVec2(m_pos.x, m_pos.y + m_headerRectSize.y), m_submenuRectSize, m_subtitleRectColor);
		text(sub->getName(), ImVec2(m_pos.x + 7.f, m_pos.y + (m_headerRectSize.y + 7.f)), m_subtitleTextColor, g_renderer->m_subtitleFont);
		text(optionPosText, ImVec2(m_pos.x + offset, m_pos.y + (m_headerRectSize.y + 7.f)), m_subtitleTextColor, g_renderer->m_subtitleFont);
		renderMulticolorRect(ImVec2(m_pos.x, m_pos.y + m_headerRectSize.y + 35.f), ImVec2(m_headerRectSize.x, 2.f), BottomRight, BottomLeft, TopLeft, TopRight);
	}
	void dxUiManager::smoothScrollHandler() {
		const auto scrollPosition = m_drawBase + (m_optionRectSize.y / 2.f);
		auto opCountChecks = (m_currentOption < m_maxVisOptions) ? m_currentOption : m_maxVisOptions;
		auto scrollPos = m_pos.y + 2.f + m_headerRectSize.y + (m_optionRectSize.y * static_cast<float>(opCountChecks));
		static smoothScroll scroll(scrollPosition, m_scrollSpeed / 10.f);
		scroll.setTarget(scrollPos).update(m_scrollSpeed);
		if (m_optionCount != 0) {
			auto drawList = ImGui::GetForegroundDrawList();
			rect(ImVec2(m_pos.x, scroll.getCurrent()), m_optionRectSize, m_optionSelectedRectColor, true, drawList);
		}
	}
	void dxUiManager::optionHandler(abstractOpt* opt, bool isOpSelected) {
		auto leftAlignOffset = 7.f; //Left align offset
		auto centerTextAlignOffset = 10.f; //Center align offset (Center of Y positon of the rect)
		auto rightAlignOffset = m_width - 18.f; //Right align offset
		auto rightText = opt->getRight();
		auto numberRightAlignOffset = ImGui::CalcTextSize(rightText.c_str()).x;
		auto yPosMath = 2.f + m_headerRectSize.y + (m_optionRectSize.y * m_optionCount);
		auto textCol = isOpSelected ? m_optionSelectedTextColor : m_optionTextColor;
		//Option Background
		rect(ImVec2(m_pos.x, m_pos.y + yPosMath), m_optionRectSize, m_optionRectColor);
		//Option scroller (smooth)
		if (isOpSelected)
			smoothScrollHandler();
		//Left text
		text(opt->getLeft(), ImVec2(m_pos.x + leftAlignOffset, m_pos.y + centerTextAlignOffset + (yPosMath - 2.f)), textCol);
		switch (opt->getOptType()) {
		case eOptType::SubOpt: {
			text("e", ImVec2(m_pos.x + rightAlignOffset + 1.f, m_pos.y + centerTextAlignOffset + (yPosMath - 2.f)), textCol, g_renderer->m_iconFontMain);
		} break;
		case eOptType::BoolOpt: {
			text(rightText, ImVec2(m_pos.x + rightAlignOffset, m_pos.y + centerTextAlignOffset + (yPosMath - 2.f)), textCol, g_renderer->m_iconFontMain);
		} break;
		case eOptType::NumOpt: {
			text(rightText, ImVec2(m_pos.x + m_width - numberRightAlignOffset - 5.75f, m_pos.y + centerTextAlignOffset + (yPosMath - 2.f)), textCol);
		} break;
		case eOptType::NumBoolOpt: {
			//Get the text before the icon (smart)
			text(rightText.substr(0, rightText.find(" ")), ImVec2(m_pos.x + m_width - numberRightAlignOffset - 5.75f, m_pos.y + centerTextAlignOffset + 0.5f + (yPosMath - 2.f)), textCol);
			//Actual icon
			text(rightText.substr(rightText.find(" ") + 1), ImVec2(m_pos.x + rightAlignOffset, m_pos.y + centerTextAlignOffset + (yPosMath - 2.f)), textCol, g_renderer->m_iconFontMain);
		} break;
		case eOptType::VecOpt: {
			auto tSizeOfOptPos = ImGui::CalcTextSize(rightText.substr(rightText.find("_[") + 1).c_str()).x;
			auto tSizeOfOptNme = ImGui::CalcTextSize(rightText.substr(0, rightText.find("_[")).c_str()).x;
			//Option position
			text(rightText.substr(rightText.find("_[") + 1), ImVec2(m_pos.x + m_width - tSizeOfOptPos - 7.9f, m_pos.y + centerTextAlignOffset + 0.5f + (yPosMath - 2.f)), textCol);
			//Selected text from vectors
			text(rightText.substr(0, rightText.find("_[")), ImVec2(m_pos.x + m_width - tSizeOfOptPos - tSizeOfOptNme - 17.8f, m_pos.y + centerTextAlignOffset + 0.5f + (yPosMath - 2.f)), textCol);
		} break;
		case eOptType::VecBoolOpt: {
			auto tSizeOfOptNme = ImGui::CalcTextSize(rightText.substr(0, rightText.find("_[")).c_str()).x;
			//Selected text from vectors
			text(rightText.substr(0, rightText.find("_[")), ImVec2(m_pos.x + m_width - tSizeOfOptNme - 23.5f, m_pos.y + centerTextAlignOffset + (yPosMath - 2.f)), textCol);
			//Toggle icon
			text(rightText.substr(rightText.find("]_") + 2), ImVec2(m_pos.x + m_width - 18.f, m_pos.y + centerTextAlignOffset + (yPosMath - 2.f)), textCol, g_renderer->m_iconFontMain);
		} break;
		default: {
			//Normal right align
			text(rightText, ImVec2(m_pos.x + rightAlignOffset, m_pos.y + centerTextAlignOffset + (yPosMath - 2.f)), textCol);
		} break;
		}
	}
	void dxUiManager::footerHandler() {
		float arrowOffset = m_width / 2.f;
		float textOffset = m_width - 55.f;
		float centerArrowOffset = (m_footerRectSize.y / 2) - 10.f;
		auto footerPos = ImVec2(m_pos.x, m_pos.y + m_headerRectSize.y + m_submenuRectSize.y + (m_optionRectSize.y * m_optionCount));
		rect(footerPos, m_footerRectSize, m_footerRectColor);
		auto arrowPos = ImVec2(m_pos.x + arrowOffset, footerPos.y + centerArrowOffset + 0.5f);
		//Optimized code:
		if (m_currentOption == 1) {
			//Top arrow
			text("d", arrowPos, m_footerArrowColor, g_renderer->m_iconFontMain);
		}
		else if (m_currentOption == m_optionCount) {
			//Bottom arow
			text("c", arrowPos, m_footerArrowColor, g_renderer->m_iconFontMain);
		}
		else {
			auto arrowPos = ImVec2(m_pos.x + arrowOffset, footerPos.y + (m_footerRectSize.y / 2) - 15.f);
			//Top and bottom arrow
			text("d\nc", arrowPos, m_footerArrowColor, g_renderer->m_iconFontMain);
		}
		//Left text (str = 'Dev')
		text("V5.1.3", ImVec2(m_pos.x + 7.f, footerPos.y + 9.f), m_footerTextColor, g_renderer->m_subtitleFont);
		//Right text (str = 'v4.0.0')
		text("Public", ImVec2(m_pos.x + textOffset, footerPos.y + 9.f), m_footerTextColor, g_renderer->m_subtitleFont);
		renderMulticolorRect(footerPos, ImVec2(m_headerRectSize.x, 2.f), BottomRight, BottomLeft, TopLeft, TopRight);
	}
	void dxUiManager::descriptionHandler() {
		auto iconOffset{ 7.f };
		auto textOffset{ iconOffset + 24.f };
		auto iconOffsetFromRect{ 8.f };
		auto textOffsetFromRect{ 10.f };
		std::string descText{};
		if (!m_submenuStack.empty()) {
			if (const auto& sub = m_submenuStack.top(); sub->getNumberOfOptions() != 0) {
				if (auto opt = sub->getOpt(sub->getSelectedOpt()))
					descText = opt->getDescription();
			}
		}
		if (!descText.empty()) {
			const auto arr = getWordsSimple(descText);
			auto lines = static_cast<float>(arr.size());
			const auto yPosMath = 2.f + m_headerRectSize.y + m_submenuRectSize.y + (m_optionRectSize.y * m_optionCount) + m_footerRectSize.y;
			const auto rectPos = ImVec2(m_pos.x, m_pos.y + yPosMath);
			const auto rectSize = ImVec2(m_infoboxRectSize.x, std::max<int>(30, 5.f + round(ImGui::CalcTextSize(descText.c_str()).x / m_infoboxRectSize.x) * ImGui::CalcTextSize(descText.c_str()).y)); // "Imgui Calculate Cock size" - Playboy
			rect(rectPos, rectSize, m_infoboxRectColor);
			renderMulticolorRect(rectPos, ImVec2(m_infoboxRectSize.x, 3.f), BottomRight, BottomLeft, TopLeft, TopRight);
			if (ImGui::Begin("##descriptionText", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs)) {
				ImGui::SetWindowPos(scale(ImVec2(rectPos.x, rectPos.y + 4.f), getRes()));
				ImGui::SetWindowSize(scale(rectSize, getRes()));
				ImGui::PushFont(g_renderer->m_infoboxFont);
				ImGui::Dummy(scale(0.f, 2.f, getRes()));
				ImGui::Indent(scale(0.5f, getRes()));
				ImGui::SameLine();
				ImGui::TextWrapped(descText.c_str());
				ImGui::Dummy(scale(0.f, 5.f, getRes()));
				ImGui::PopFont();
				ImGui::End();
			}
		}
	}
	char timeBuf[80];
	void dxUiManager::overlayHandler() {
		if (features::InfoOverlay) {
			static auto textOffset = 7.f;
			auto pos = ImVec2(5.f, 5.f);
			auto timeC = std::time(nullptr);
			auto tM = std::localtime(&timeC);
			std::strftime(timeBuf, sizeof(timeBuf), "%c", tM);
			rect(pos, m_overlayRectSize, m_subtitleRectColor, false);
			renderMulticolorRect(pos, ImVec2(m_overlayRectSize.x, 3.f), BottomRight, BottomLeft, TopLeft, TopRight, false);
			auto fps = std::round(ImGui::GetIO().Framerate / 2.f);
			text(std::format("lightning-x.online | FPS {} | {}", fps, timeBuf), ImVec2(pos.x + textOffset, pos.y + 8.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			//INFO
			rect(ImVec2(pos.x, pos.y + 40.f), m_OverlayInfoRectSize, m_subtitleRectColor, false);
			renderMulticolorRect(ImVec2(pos.x, pos.y + 40.f), ImVec2(m_OverlayInfoRectSize.x, 5.f), BottomRight, BottomLeft, TopLeft, TopRight, false);
			text("Ped Pool:", ImVec2(pos.x + textOffset, pos.y + 55.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text(std::format("{}/{}", (*g_pointers->m_replayInterface)->m_ped_interface->m_cur_peds, (*g_pointers->m_replayInterface)->m_ped_interface->m_max_peds), ImVec2(pos.x + textOffset + 125.f, pos.y + 55.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text("Vehicle Pool:", ImVec2(pos.x + textOffset, pos.y + 75.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text(std::format("{}/{}", (*g_pointers->m_replayInterface)->m_vehicle_interface->m_cur_vehicles, (*g_pointers->m_replayInterface)->m_vehicle_interface->m_max_vehicles), ImVec2(pos.x + textOffset + 125.f, pos.y + 75.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text("Object Pool:", ImVec2(pos.x + textOffset, pos.y + 95.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text(std::format("{}/{}", (*g_pointers->m_replayInterface)->m_object_interface->m_cur_objects, (*g_pointers->m_replayInterface)->m_object_interface->m_max_objects), ImVec2(pos.x + textOffset + 125.f, pos.y + 95.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text("Pickup Pool:", ImVec2(pos.x + textOffset, pos.y + 115.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text(std::format("{}/{}", (*g_pointers->m_replayInterface)->m_pickup_interface->m_cur_pickups, (*g_pointers->m_replayInterface)->m_pickup_interface->m_max_pickups), ImVec2(pos.x + textOffset + 125.f, pos.y + 115.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text("GTA Online:", ImVec2(pos.x + textOffset, pos.y + 135.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text(std::format("{}", g_pointers->m_online_version), ImVec2(pos.x + textOffset + 156.f, pos.y + 135.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text("Game Build:", ImVec2(pos.x + textOffset, pos.y + 155.f), m_headerTextColor, g_renderer->m_overlayFont, false);
			text(std::format("{}", g_pointers->m_game_version), ImVec2(pos.x + textOffset + 155.f, pos.y + 155.f), m_headerTextColor, g_renderer->m_overlayFont, false);
		}
	}
	ImVec2 dxUiManager::vecAdd(ImVec2 vectorA, ImVec2 vectorB) {
		return { vectorA.x + vectorB.x, vectorA.y + vectorB.y };
	}
	void dxUiManager::rect(ImVec2 pos, ImVec2 size, ImU32 color, bool shouldHaveScaling, ImDrawList* drawList) {
		const ImRect rec(scale(pos, getRes(), shouldHaveScaling), addVecs(scale(pos, getRes(), shouldHaveScaling), ImGui::CalcItemSize(scale(size, getRes(), shouldHaveScaling), 0.f, 0.f)));
		drawList->AddRectFilled(rec.Min, rec.Max, ImGui::GetColorU32(color));
	}
	void dxUiManager::rectOutline(ImVec2 pos, ImVec2 size, ImU32 color, bool shouldHaveScaling, float thickness) {
		auto drawList = ImGui::GetBackgroundDrawList();
		const ImRect rec(scale(pos, getRes(), shouldHaveScaling), addVecs(scale(pos, getRes(), shouldHaveScaling), ImGui::CalcItemSize(scale(size, getRes(), shouldHaveScaling), 0.f, 0.f)));
		drawList->AddRect(rec.Min, rec.Max, color, 0.f, 15, thickness);
	}
	void dxUiManager::renderMulticolorRect(ImVec2 pos, ImVec2 size, ImU32 color1, ImU32 color2, ImU32 color3, ImU32 color4, bool shouldHaveScaling) {
		auto drawList = ImGui::GetBackgroundDrawList();
		const ImRect rec(scale(pos, getRes(), shouldHaveScaling), addVecs(scale(pos, getRes(), shouldHaveScaling), ImGui::CalcItemSize(scale(size, getRes(), shouldHaveScaling), 0.f, 0.f)));
		drawList->AddRectFilledMultiColor(rec.Max, rec.Min, color1, color2, color3, color4);
	}
	void dxUiManager::line(ImVec2 pos, ImVec2 size, ImU32 color, bool shouldHaveScaling, float thickness) {
		auto drawList = ImGui::GetBackgroundDrawList();
		const ImRect rec(scale(pos, getRes(), shouldHaveScaling), addVecs(scale(pos, getRes(), shouldHaveScaling), ImGui::CalcItemSize(scale(size, getRes(), shouldHaveScaling), 0.f, 0.f)));
		drawList->AddLine(rec.Min, rec.Max, color, thickness);
	}
	void dxUiManager::image(ImTextureID image, ImVec2 pos, ImVec2 size, ImU32 color, bool shouldHaveScaling) {
		auto drawList = ImGui::GetForegroundDrawList();
		const ImRect rec(scale(pos, getRes(), shouldHaveScaling), addVecs(scale(pos, getRes(), shouldHaveScaling), ImGui::CalcItemSize(scale(size, getRes(), shouldHaveScaling), 0.f, 0.f)));
		drawList->AddImage(image, rec.Min, rec.Max, ImVec2(0, 0), ImVec2(1, 1), color);
	}
	void dxUiManager::text(std::string text, ImVec2 pos, ImU32 color, ImFont* font, bool shouldHaveScaling) {
		auto drawList = ImGui::GetForegroundDrawList();
		ImGui::PushFont(font);
		drawList->AddText(scale(pos, getRes(), shouldHaveScaling), color, text.c_str());
		ImGui::PopFont();
	}
	void dxUiManager::drawTitle(const std::string& playerName) {
		const float leftSideOffset = 7.f;
		const ImVec2 pos = { m_pos.x + m_width + 15.f, m_pos.y };
		const auto color = IM_COL32(10, 10, 10, 205);
		rect(pos, m_playerinfoRectSize, color, true);
		rectOutline(pos, { m_playerinfoRectSize.x, 665.f }, m_entireMenuColor, true, 1.50f);
		text(playerName, { pos.x + leftSideOffset, pos.y + 8.f }, IM_COL32_WHITE, g_renderer->m_big_font, true);
		m_numberOfLines = 0;
	}
	void dxUiManager::drawLine(const std::string& name, const std::string& data) {
		m_numberOfLines++;
		const float leftSideOffset = 7.f;
		const ImVec2 pos = { m_pos.x + m_width + 15.f, m_pos.y + 35.f + (m_playerinfoRectSize.y * (m_numberOfLines - 1)) };
		const ImVec2 dataTextSize = ImGui::CalcTextSize(data.c_str());
		const ImVec2 nameTextSize = ImGui::CalcTextSize(name.c_str());
		rect(pos, m_playerinfoRectSize, IM_COL32(10, 10, 10, 205));
		text(name, { pos.x + leftSideOffset, pos.y + nameTextSize.y - 5.f }, IM_COL32(255, 255, 255, 255));
		text(data, { pos.x + m_playerinfoRectSize.x - dataTextSize.x - 9.5f, pos.y + dataTextSize.y - 5.f }, IM_COL32(255, 255, 255, 255));
	}
	void dxUiManager::drawSectionLine(std::string name, std::string data, std::string name2, std::string data2) {
		m_numberOfLines++;
		const float leftSideOffset = 7.f;
		const float yPosition = m_pos.y + 35.f + (m_playerinfoRectSize.y * (m_numberOfLines - 1));
		const auto pos = ImVec2(m_pos.x + m_width + 15.f, yPosition);
		const auto centerLinePos = ImVec2(pos.x + (m_playerinfoRectSize.x / 2.f) + 3.f, pos.y);
		const auto dataTextSize = ImGui::CalcTextSize(data.c_str());
		const auto data2TextSize = ImGui::CalcTextSize(data2.c_str());
		const auto textColor = IM_COL32(255, 255, 255, 255);
		const auto rectColor = IM_COL32(10, 10, 10, 205);
		rect(pos, m_playerinfoRectSize, rectColor);
		text(name, ImVec2(pos.x + leftSideOffset, pos.y + ImGui::CalcTextSize(name.c_str()).y - 7.5f), textColor);
		text(data, ImVec2(centerLinePos.x + leftSideOffset - dataTextSize.x - 15.6f, centerLinePos.y + ImGui::CalcTextSize(name.c_str()).y - 7.5f), textColor);
		rect(centerLinePos, ImVec2(1.f, m_playerinfoRectSize.y), m_entireMenuColor);
		text(name2, ImVec2(centerLinePos.x + leftSideOffset, centerLinePos.y + ImGui::CalcTextSize(name2.c_str()).y - 7.5f), textColor);
		text(data2, ImVec2(pos.x + m_playerinfoRectSize.x - data2TextSize.x - 7.f, pos.y + data2TextSize.y - 7.5f), textColor);
	}
	void dxUiManager::drawPlayerinfo(player_ptr plyr) {
		auto rlGamerInfo = plyr->get_player_info();
		if (!plyr->get_player_info()) return;
		auto pos = plyr->get_ped()->get_position();
		auto modelHash = rlGamerInfo->m_ped->m_model_info->m_hash;
		std::string finalLabel{};
		std::string PlayerVehicleGod{};
		if (auto vehicle = rlGamerInfo->m_ped->m_vehicle; rlGamerInfo->m_ped->m_vehicle && vehicle->m_model_info) {
			auto modelInfo = reinterpret_cast<CVehicleModelInfo*>(vehicle->m_model_info);
			const char* modelName = modelInfo->m_name;
			const char* manufacturerName = modelInfo->m_manufacturer;
			if (modelName && manufacturerName) {
				std::string manufacturerLabel = g_pointers->m_getGxtLabelFromTable(g_pointers->m_gxtLabels, modelName);
				std::string nameLabel = g_pointers->m_getGxtLabelFromTable(g_pointers->m_gxtLabels, manufacturerName);
				if (!manufacturerLabel.empty() && manufacturerLabel != TRANSLATE("NULL")) {
					finalLabel = manufacturerLabel + " " + nameLabel;
				}
				else {
					finalLabel = nameLabel;
				}
				PlayerVehicleGod = (vehicle->m_damage_bits & (uint32_t)eEntityProofs::GOD) ? TRANSLATE("YES") : TRANSLATE("NO");
			}
		}
		auto& stats = globals(1853988).as<GPBD_FM*>()->Entries[plyr->id()].PlayerStats;
		auto const& boss_goon = globals(1895156).as<GPBD_FM_3*>()->Entries[plyr->id()].BossGoon;
		auto ip = plyr->get_ip_address();
		auto port = plyr->get_port();
		g_dxUiManager.drawLine(TRANSLATE("SELF_GODMODE"), rlGamerInfo->m_ped->m_damage_bits & (uint32_t)eEntityProofs::GOD ? TRANSLATE("YES") : TRANSLATE("NO"));
		g_dxUiManager.drawLine(TRANSLATE("HEALTH"), std::format("{}/{} ({}%)", round(rlGamerInfo->m_ped->m_health), rlGamerInfo->m_ped->m_maxhealth, round(rlGamerInfo->m_ped->m_health / rlGamerInfo->m_ped->m_maxhealth * 100)));
		g_dxUiManager.drawLine(TRANSLATE("ARMOUR"), std::format("{}/{} ({}%)", round(rlGamerInfo->m_ped->m_armor), rlGamerInfo->m_ped->m_player_info->m_max_armor, round(rlGamerInfo->m_ped->m_armor / rlGamerInfo->m_ped->m_player_info->m_max_armor * 100)));
		g_dxUiManager.drawLine(TRANSLATE("WANTED_LVL"), std::to_string(rlGamerInfo->m_wanted_level));
		g_dxUiManager.drawLine(TRANSLATE("VEHICLE_OPTIONS"), rlGamerInfo->m_ped->m_vehicle ? finalLabel.c_str() : TRANSLATE("NOT_INSIDE_A_VEHICLE"));
		g_dxUiManager.drawLine(TRANSLATE("VEHICLE_GODMODE"), rlGamerInfo->m_ped->m_vehicle ? PlayerVehicleGod : TRANSLATE("N_A"));
		g_dxUiManager.drawLine(TRANSLATE("POSITION"), std::format("X: {}, Y: {}, Z: {}", std::round(pos->x), std::round(pos->y), std::round(pos->z)));
		g_dxUiManager.drawSectionLine(TRANSLATE("IN_INTERIOR"), g_mainScript.is_in_interior(plyr) ? TRANSLATE("YES") : TRANSLATE("NO"), TRANSLATE("IN_CUTSCENE"), g_mainScript.is_in_cutscene(plyr) ? TRANSLATE("YES") : TRANSLATE("NO"));
		g_dxUiManager.drawSectionLine(TRANSLATE("KILLS"), std::to_string(stats.KillsOnPlayers), TRANSLATE("ACCURACY"), std::to_string(stats.WeaponAccuracy));
		g_dxUiManager.drawSectionLine(TRANSLATE("DEATHS"), std::to_string(stats.DeathsByPlayers), TRANSLATE("KD_RATIO"), std::to_string(stats.KdRatio));
		g_dxUiManager.drawSectionLine(TRANSLATE("RANK"), std::to_string(stats.Rank), TRANSLATE("RP"), std::to_string(stats.RP));
		g_dxUiManager.drawSectionLine(TRANSLATE("WALLET"), std::format("${}", stats.WalletBalance), TRANSLATE("BANK"), std::format("${}", stats.Money - stats.WalletBalance));
		g_dxUiManager.drawLine(TRANSLATE("TOTAL_MONEY"), std::format("${}", stats.Money));
		g_dxUiManager.drawLine(TRANSLATE("MODEL"), std::format("{}ale", modelHash == "mp_f_freemode_01"_joaat ? TRANSLATE("PLAYER_INFO_CHECK_FEM") : TRANSLATE("PLAYER_INFO_CHECK_M")));
		g_dxUiManager.drawLine(TRANSLATE("HOST_TOKEN"), std::format("0x{:X}", rlGamerInfo->m_net_player_data.m_host_token));
		if (int languageIndex = boss_goon.Language; languageIndex >= 0 && languageIndex < 13)
			g_dxUiManager.drawLine(TRANSLATE("PLAYER_INFO_LANGUAGE"), languages[languageIndex].name);
		g_dxUiManager.drawSectionLine(TRANSLATE("RID"), std::to_string(rlGamerInfo->m_net_player_data.m_gamer_handle.m_rockstar_id), TRANSLATE("SLOT"), std::to_string(plyr->id()));
		g_dxUiManager.drawSectionLine(TRANSLATE("IP"), std::format("{}.{}.{}.{}", ip.m_field1, ip.m_field2, ip.m_field3, ip.m_field4), TRANSLATE("PORT"), std::format("{}", port));
		g_dxUiManager.drawTitle(plyr->get_name());
	}
	void dxUiManager::resetInputHandler() {
		m_openKeyPressed = false;
		m_backKeyPressed = false;
		m_enterKeyPressed = false;
		m_upKeyPressed = false;
		m_downKeyPressed = false;
		m_leftKeyPressed = false;
		m_rightKeyPressed = false;
		m_optionCount = 0;
	}
	void dxUiManager::inputCheckHandler() {
		if (!m_forceLockInput) {
			checkIfPressed(m_openKeyPressed, 2, 227, 175, VK_INSERT, m_openDelay); //RB + R-Arrow
			checkIfPressed(m_backKeyPressed, 2, 194, VK_BACK, m_enterDelay); //A
			checkIfPressed(m_enterKeyPressed, 2, 191, VK_RETURN, m_backDelay); //B
			checkIfPressed(m_upKeyPressed, 2, 172, VK_UP, m_verticalDelay); //U-Arrow
			checkIfPressed(m_downKeyPressed, 2, 173, VK_DOWN, m_verticalDelay); //D-Arrow
			checkIfPressed(m_leftKeyPressed, 2, 174, VK_LEFT, m_horizontalDelay); //L-Arrow
			checkIfPressed(m_rightKeyPressed, 2, 175, VK_RIGHT, m_horizontalDelay); //R-Arrow
		}
	}
	void dxUiManager::checkIfPressed(bool& value, int padIdx, int key, int keyboardKey, size_t delay) {
		static simpleTimer t;
		if (GetForegroundWindow() == g_pointers->m_hwnd) {
			if (GetAsyncKeyState(keyboardKey) & 1 || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(padIdx, key) && !PAD::IS_USING_KEYBOARD_AND_MOUSE(padIdx)) { value = true; }
			else if (GetAsyncKeyState(keyboardKey) & 0x8000) {
				t.start(delay * 100);
				if (t.isReady())
					value = true;
			}
			else { t.reset(); }
		}
	}
	void dxUiManager::checkIfPressed(bool& value, int padIdx, int key, int key2, int keyboardKey, size_t delay) {
		static simpleTimer t;
		if (GetForegroundWindow() == g_pointers->m_hwnd) {
			if (GetAsyncKeyState(keyboardKey) & 1 || ((PAD::IS_DISABLED_CONTROL_PRESSED(padIdx, key) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(padIdx, key)) && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(padIdx, key2) && !PAD::IS_USING_KEYBOARD_AND_MOUSE(padIdx))) { value = true; }
			else if (GetAsyncKeyState(keyboardKey) & 0x8000) {
				t.start(delay * 100);
				if (t.isReady())
					value = true;
			}
			else { t.reset(); }
		}
	}
	void dxUiManager::keypressHandler() {
		if (m_openKeyPressed) {
			if (m_enableSounds && g_mainScript.m_listUi)
				soundQueue(m_open ? "SELECT" : "BACK");
			m_open ^= true;
		}
		if (m_open && !m_submenuStack.empty()) {
			auto sub = m_submenuStack.top();
			if (m_enterKeyPressed && sub->getNumberOfOptions() != 0) {
				if (m_enableSounds)
					soundQueue("SELECT");
				//Handle enter press
				if (auto opt = sub->getOpt(sub->getSelectedOpt()))
					opt->handleAction(eOptAction::EnterPress);
			}
			if (m_backKeyPressed && sub->getNumberOfOptions() != 0) {
				if (m_enableSounds)
					soundQueue("BACK");
				//If the submenu stack is under or equal to 1, close menu | else, pop submenu (go back submenu)
				if (m_submenuStack.size() <= 1)
					m_open = false;
				else
					m_submenuStack.pop();
			}
			if (m_upKeyPressed && sub->getNumberOfOptions() != 0) {
				if (m_enableSounds)
					soundQueue("NAV_UP_DOWN");
				//Move scroller up
				sub->moveUp();
			}
			if (m_downKeyPressed && sub->getNumberOfOptions() != 0) {
				if (m_enableSounds)
					soundQueue("NAV_UP_DOWN");
				//Move scroller down
				sub->moveDown();
			}
			if (m_leftKeyPressed && sub->getNumberOfOptions() != 0) {
				if (m_enableSounds)
					soundQueue("NAV_LEFT_RIGHT");
				//Handle left press
				if (auto opt = sub->getOpt(sub->getSelectedOpt()))
					opt->handleAction(eOptAction::LeftPress);
			}
			if (m_rightKeyPressed && sub->getNumberOfOptions() != 0) {
				if (m_enableSounds)
					soundQueue("NAV_LEFT_RIGHT");
				//Handle right press
				if (auto opt = sub->getOpt(sub->getSelectedOpt()))
					opt->handleAction(eOptAction::RightPress);
			}
		}
	}
	void dxUiManager::scale() {
		g_renderer->m_leftOptionFont->Scale = m_scale;
		g_renderer->m_headerFont->Scale = m_scale;
		g_renderer->m_subtitleFont->Scale = m_scale;
		g_renderer->m_iconFontMain->Scale = m_scale;
		g_renderer->m_footerFont->Scale = m_scale;
		g_renderer->m_infoboxFont->Scale = m_scale;
	}
	void dxUiManager::width() {
		m_headerRectSize.x = m_width;
		m_submenuRectSize.x = m_width;
		m_optionRectSize.x = m_width;
		m_footerRectSize.x = m_width;
		m_infoboxRectSize.x = m_width;
	}
}