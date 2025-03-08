#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <functional>
#include "opt.hpp"
#include "Colors.hpp"
#include "ui/dxUiManager.hpp"
#include "D3DRenderer.hpp"

namespace base::listUiTypes {
	inline void inputColor(const std::string& name, ImU32* color, const std::function<void()>& onChange = nullptr) {
		static char txt[32];
		sprintf_s(txt, "##%s_colInput", name.c_str());

		g_rendererQueue.add(txt, [color, onChange]() {
			ImGui::SetNextWindowSize(ImVec2(g_dxUiManager.scale(g_dxUiManager.m_width + 10.0f, 500.0f, g_dxUiManager.getRes())));
			ImGui::SetNextWindowPos(ImVec2(g_dxUiManager.scale(g_dxUiManager.m_pos.x + g_dxUiManager.m_width + 5.0f, g_dxUiManager.m_pos.y - 5.0f, g_dxUiManager.getRes())));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.0f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			if (ImGui::Begin(txt, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground)) {
				ImGui::PopStyleVar(2);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
				ImGui::PushItemWidth(-1.0f);

				static float col[4] = {
					((*color >> IM_COL32_R_SHIFT) & 0xFF) * (1.0f / 255.0f),
					((*color >> IM_COL32_G_SHIFT) & 0xFF) * (1.0f / 255.0f),
					((*color >> IM_COL32_B_SHIFT) & 0xFF) * (1.0f / 255.0f),
					((*color >> IM_COL32_A_SHIFT) & 0xFF) * (1.0f / 255.0f) };

				if (ImGui::ColorPicker4("##picker", col, ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayRGB)) {
					*color = IM_COL32(int(col[0] * 255.0f), int(col[1] * 255.0f), int(col[2] * 255.0f), int(col[3] * 255.0f));
					if (onChange) {
						onChange();
					}
				}

				ImGui::PopItemWidth();
				ImGui::PopStyleVar(4);

				if (g_dxUiManager.checkIfPressed(VK_SHIFT) && g_dxUiManager.checkIfPressed(VK_RETURN)) {
					g_dxUiManager.m_forceLockInput = false;
					ImGui::GetIO().MouseDrawCursor = false;
					features::DisableAllActions = false;
					g_rendererQueue.remove(txt);
				}
				else if (g_dxUiManager.checkIfPressed(VK_ESCAPE)) {
					g_dxUiManager.m_forceLockInput = false;
					ImGui::GetIO().MouseDrawCursor = false;
					features::DisableAllActions = false;
					g_rendererQueue.remove(txt);
				}
				else {
					g_dxUiManager.m_forceLockInput = true;
					ImGui::GetIO().MouseDrawCursor = true;
					features::DisableAllActions = true;
				}
				ImGui::End();
			}
			else {
				g_rendererQueue.remove(txt);
			}
			});
	}
	class colorOpt : public opt<colorOpt> {
	public:
		colorOpt& setColor(ImU32* color, std::function<void()> customAction = nullptr) {
			m_customAction = customAction;
			m_callActionOnChange = m_customAction ? true : false;
			m_color = color;
			*m_color = getColor();
			return *this;
		}
		ImU32 getColor() const {
			return *m_color;
		}
		RGBA getRGBAColor() const {
			auto col = ImColor(getColor()).Value;
			return RGBA(int(col.x * 255.f), int(col.y * 255.f), int(col.z * 255.f), int(col.w * 255.f));
		}
		void handleAction(eOptAction action) {
			switch (action) {
			case eOptAction::EnterPress: {
				if (m_callActionOnChange) {
					inputColor(getLeft(), m_color, m_customAction);
				}
				else {
					inputColor(getLeft(), m_color);
				}
				opt::handleAction(action);
			} break;
			}
		}
		eOptType getOptType() {
			return eOptType::ColorOpt;
		}
	private:
		bool m_callActionOnChange{};
		std::function<void()> m_customAction{};
		ImU32* m_color{};
	};
}