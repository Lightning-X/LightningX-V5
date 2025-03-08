#include "ScriptGlobal.hpp"
#include "MainScript.hpp"
#include "GtaUtill.hpp"
#include "Teleport.hpp"
#include "Features.hpp"
#include "HashList.hpp"
#include "../Entity.hpp"
#include "../Enums.hpp"
#include "HTTPRequest.hpp"
#include "ThreadPool.hpp"
#include "Fibers/script.hpp"
#include "Fibers/fiber_pool.hpp"
#include "fontAwesome.hpp"
#include "Common.hpp"
#include "Recovery.hpp"
#include "KicksAndCrashes.hpp"
#include "Services/Api/api_service.hpp"
#include "Services/Friend/friends_service.hpp"
#include "OutfitEditor.hpp"
#include "VehModTable.hpp"
#include "Vehicle.hpp"
#include "Labels.hpp"
namespace base {
	struct Color {
		int r, g, b, a;
		Color() : r(0), g(0), b(0), a(0) {}
		Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}
	};
#define convertFloatTableToColorTable(flt) Color(flt[0] * floatToIntTable, flt[1] * floatToIntTable, flt[2] * floatToIntTable, 255)
#define floatToIntTable 255.f
#define convertFloatTable(flt, idx) flt[idx] * floatToIntTable
	float primaryCol[3];
	float secondaryCol[3];
	float tireSmokeCol[3];
	float nenonlightsCol[3];
	bool selectable(const std::string_view text, bool selected)
	{
		return ImGui::Selectable(text.data(), selected);
	}
	bool selectable(const std::string_view text, bool selected, ImGuiSelectableFlags flag)
	{
		return ImGui::Selectable(text.data(), selected, flag);
	}
	void selectable(const std::string_view text, bool selected, std::function<void()> cb)
	{
		if (selectable(text.data(), selected))
			g_fiber_pool->queue_job(std::move(cb));
	}
	void selectable(const std::string_view text, bool selected, ImGuiSelectableFlags flag, std::function<void()> cb)
	{
		if (selectable(text.data(), selected, flag))
		{
			g_fiber_pool->queue_job(std::move(cb));
		}
	}
	bool button(const std::string_view text) {
		return ImGui::Button(text.data());
	}
	void button(const std::string_view text, std::function<void()> cb) {
		if (button(text)) {
			g_fiber_pool->queue_job(cb);
		}
	}
	ImVec2 vecAdd(ImVec2 vectorA, ImVec2 vectorB) {
		return { vectorA.x + vectorB.x, vectorA.y + vectorB.y };
	}
	void renderText(std::string text, ImVec2 pos, ImU32 color, ImFont* font = g_renderer->m_font) {
		auto drawList = ImGui::GetCurrentWindow()->DrawList;
		ImGui::PushFont(font);
		drawList->AddText(pos, color, text.c_str());
		ImGui::PopFont();
	}
	void renderMulticolorRect(ImVec2 pos, ImVec2 size, ImU32 color1, ImU32 color2, ImU32 color3, ImU32 color4) {
		auto drawList = ImGui::GetCurrentWindow()->DrawList;
		const ImRect rec(pos, vecAdd(pos, ImGui::CalcItemSize(size, 0.f, 0.f)));
		drawList->AddRectFilledMultiColor(rec.Max, rec.Min, color1, color2, color3, color4);
	}
	void sectionBox(std::string id, ImVec2 size, ImFont* font, std::function<void()> func) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4());
		if (ImGui::BeginListBox(("##" + id).c_str(), size)) {
			ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(77, 78, 82, 101));
			ImGui::PushFont(font);
			func();
			ImGui::PopFont();
			ImGui::PopStyleColor();
			ImGui::EndListBox();
		}
		ImGui::PopStyleColor();
	}
	auto TopLeft = IM_COL32(0, 183, 255, 255);//
	auto BottomLeft = IM_COL32(0, 156, 235, 255);//
	auto TopRight = IM_COL32(0, 156, 235, 255);//69, 224, 255
	auto BottomRight = IM_COL32(0, 105, 158, 255);//0, 213, 255
	//TODO: Dynamicly change to fit all the text
	void child(std::string name, float length, std::function<void()> func) {
		const ImVec2 boxSize(325.f, length + 30.f);
		sectionBox(name + "_", boxSize, g_renderer->m_ClicKTitleFont, [length, name, func] {
			auto style = (*GImGui).Style;
			const float textHeight = ImGui::CalcTextSize(name.c_str(), nullptr, true).y;
			const ImVec2 frameSize(290.f, ImMax(length, textHeight) + ImGui::GetTextLineHeightWithSpacing() * 7.25f + style.FramePadding.y * 2.0f);
			const ImVec2 framePos = ImGui::GetCurrentWindow()->DC.CursorPos;
			auto frameBb = ImRect(framePos, vecAdd(framePos, frameSize));
			auto textPos = ImVec2(frameBb.Max.x + style.ItemInnerSpacing.x, frameBb.Min.y + style.FramePadding.y);
			renderText(name, ImVec2(textPos.x - 288.f, textPos.y - 8.f), IM_COL32(0, 183, 254, 255));
			ImGui::Dummy(ImVec2(0.f, 15.f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
			if (ImGui::BeginListBox(("##" + name).c_str(), ImVec2(290.f, length))) {
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.25f);
				ImGui::Dummy(ImVec2(0.f, 10.f));
				func();
				renderMulticolorRect(
					ImVec2(textPos.x - 290.f, textPos.y + 18.f),
					ImVec2(285.f, 10.f),
					TopLeft,
					BottomLeft,
					TopRight,
					BottomRight
				);
				ImGui::PopStyleVar();
				ImGui::EndListBox();
			}
			ImGui::PopStyleVar();
			});
	}
	decltype(auto) combineChilds(float offset = -1.f) {
		return ImGui::SameLine(0.f, offset);
	}
	void checkbox(std::string name, bool& toggle, std::function<void()> func = [] {}) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
		if (ImGui::Checkbox(name.c_str(), &toggle))
			func();
		ImGui::PopStyleVar();
	}
	void sliderInt(std::string name, int& val, int min, int max, float width = 170.f, std::function<void()> func = [] {}) {
		auto style = (*GImGui).Style;
		auto size = ImFloor(
			ImGui::CalcItemSize(ImVec2(290.f, 10.f),
				ImGui::CalcItemWidth(),
				ImGui::GetTextLineHeightWithSpacing() * 7.25f + style.FramePadding.y * 2.0f)
		);
		auto frameBb = ImRect(ImGui::GetCurrentWindow()->DC.CursorPos, vecAdd(ImGui::GetCurrentWindow()->DC.CursorPos, ImVec2(size.x, ImMax(size.y, ImGui::CalcTextSize(name.c_str(), nullptr, true).y))));
		auto textPos = ImVec2(frameBb.Max.x + style.ItemInnerSpacing.x, frameBb.Min.y + style.FramePadding.y);
		renderText(name, ImVec2(textPos.x - 288.f, textPos.y - 3.f), IM_COL32(255, 255, 255, 255), g_renderer->m_font);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
		ImGui::PushItemWidth(width);
		if (ImGui::SliderInt((name).c_str(), &val, min, max))
			func();
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
	}
	void sliderFloat(std::string name, float& val, float min, float max, float width = 150.f, std::function<void()> func = [] {}) {
		auto style = (*GImGui).Style;
		auto size = ImFloor(
			ImGui::CalcItemSize(ImVec2(290.f, 10.f),
				ImGui::CalcItemWidth(),
				ImGui::GetTextLineHeightWithSpacing() * 7.25f + style.FramePadding.y * 2.0f)
		);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
		ImGui::PushItemWidth(width);
		if (ImGui::SliderFloat((name).c_str(), &val, min, max))
			func();
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
	}
	int tabId = 0;
	void selfTab() {
		child(TRANSLATE("SELF_OPTIONS"), 210.f, [=] {
			checkbox(TRANSLATE("SELF_GODMODE"), features::godModeBool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("SELF_NEVER_WANTED"), features::neverWantedBool);
			checkbox(TRANSLATE("SELF_INVISIBILITY"), features::invisibilityBool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("SELF_LOCALLY_VISIBLE"), features::locallyVisbleBool);
			checkbox(TRANSLATE("SELF_NO_RAGDOLL"), features::noRagdollBool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("SELF_NO_CLIP"), features::NoClipBool);
			checkbox(TRANSLATE("SELF_FREECAM"), features::Freecambool);
			checkbox(TRANSLATE("SELF_RESPAWN_AT_PLACE_OF_DEATH"), features::FastRespawnBool);
			});
		combineChilds();
		child(TRANSLATE("SELF_MOVEMENT"), 210.f, [=] {
			ImGui::Combo(TRANSLATE("SELF_MOVEMENT_SUPER_JUMP"), &SuperJumpType, superjumpnamme, IM_ARRAYSIZE(superjumpnamme));
			sliderFloat(TRANSLATE("SELF_MOVEMENT_RUN_SPEED"), features::fastRunSpeed, 1.f, 10.f);
			sliderFloat(TRANSLATE("SELF_MOVEMENT_SWIM_SPEED"), features::fastSwimSpeed, 1.f, 2.9f);
			checkbox(TRANSLATE("SELF_MOVEMENT_SUPER_RUN"), features::superRunBool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("SELF_MOVEMENT_SWIM_IN_AIR"), features::swimInAirBool);
			checkbox(TRANSLATE("SELF_MOVEMENT_GRACEFUL_LANDING"), features::GracefulLanding);
			checkbox(TRANSLATE("SELF_MOVEMENT_WALK_UNDERWATER"), features::selfNoWaterCollision);
			});
		child(TRANSLATE("SKIN_CHANGER"), 210.f, [=] {
			if (ImGui::TreeNode(TRANSLATE("ANIMALS"))) {
				for (auto model : Animals) {
					if (ImGui::Selectable(model)) {
						features::applyChosenSkin(rage::joaat(model));
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(TRANSLATE("MULTIPLAYER"))) {
				for (auto model : Multiplayerskins) {
					if (ImGui::Selectable(model)) {
						features::applyChosenSkin(rage::joaat(model));
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(TRANSLATE("STORYMODE"))) {
				for (auto model : Storycharacter) {
					if (ImGui::Selectable(model)) {
						features::applyChosenSkin(rage::joaat(model));
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(TRANSLATE("FEMALE"))) {
				for (auto model : Female) {
					if (ImGui::Selectable(model)) {
						features::applyChosenSkin(rage::joaat(model));
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(TRANSLATE("ALL"))) {
				for (auto model : pedModels) {
					if (ImGui::Selectable(model)) {
						features::applyChosenSkin(rage::joaat(model));
					}
				}
				ImGui::TreePop();
			}
			static char inputTextForModel[20];
			ImGui::InputText(TRANSLATE("SEARCH"), inputTextForModel, sizeof(inputTextForModel));
			button(TRANSLATE("FIND_MODEL"), [] {
				g_fiber_pool->queue_job([=] {
					features::applyChosenSkin(rage::joaat(inputTextForModel));
					});
				});
			});
		ImGui::Separator();
		//combineChilds();
		//child(TRANSLATE("SELF_OUTFIT_EDITOR"), 210.f, [=] {
		static components_t components;
		static props_t props;
		g_fiber_pool->queue_job([] {
			for (auto& item : components.items) {
				item.drawable_id = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), item.id);
				item.drawable_id_max = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), item.id) - 1;
				item.texture_id = PED::GET_PED_TEXTURE_VARIATION(PLAYER::PLAYER_PED_ID(), item.id);
				item.texture_id_max = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(PLAYER::PLAYER_PED_ID(), item.id, item.drawable_id) - 1;
			}
			for (auto& item : props.items) {
				item.drawable_id = PED::GET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), item.id, 1);
				item.drawable_id_max = PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), item.id) - 1;
				item.texture_id = PED::GET_PED_PROP_TEXTURE_INDEX(PLAYER::PLAYER_PED_ID(), item.id);
				item.texture_id_max = PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(PLAYER::PLAYER_PED_ID(), item.id, item.drawable_id) - 1;
			}
			});
		button(TRANSLATE("OUTFIT_RANDOM_COMPONENT"), [] {
			set_ped_random_component_variation(PLAYER::PLAYER_PED_ID());
			});
		ImGui::SameLine();

		button(TRANSLATE("OUTFIT_DEFAULT_COMPONENT"), [] {
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
			});
		ImGui::SameLine();

		button(TRANSLATE("OUTFIT_RANDOM_PROPS"), [] {
			PED::SET_PED_RANDOM_PROPS(PLAYER::PLAYER_PED_ID());
			});
		ImGui::SameLine();

		button(TRANSLATE("OUTFIT_CLEAR_PROPS"), [] {
			PED::CLEAR_ALL_PED_PROPS(PLAYER::PLAYER_PED_ID(), 1);
			});
		ImGui::BeginGroup();
		for (auto& item : components.items) {
			ImGui::SetNextItemWidth(120);
			if (ImGui::InputInt(std::format("{} [0,{}]##1", item.label, item.drawable_id_max).c_str(), &item.drawable_id)) {
				check_bounds_drawable(&item); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([item] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), item.id, item.drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), item.id));
					});
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		for (auto& item : components.items) {
			ImGui::SetNextItemWidth(120);
			if (ImGui::InputInt(std::format("{} {} [0,{}]##2", item.label, TRANSLATE("OUTFIT_TEX"), item.texture_id_max).c_str(), &item.texture_id)) {
				check_bounds_texture(&item); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([item] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), item.id, item.drawable_id, item.texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), item.id));
					});
			}
		}
		ImGui::EndGroup();
		//ImGui::SameLine();
		ImGui::BeginGroup();
		for (auto& item : props.items) {
			ImGui::SetNextItemWidth(120);
			if (ImGui::InputInt(std::format("{} [0,{}]##3", item.label, item.drawable_id_max).c_str(), &item.drawable_id)) {
				check_bounds_drawable(&item); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([item] {
					if (item.drawable_id == -1)
						PED::CLEAR_PED_PROP(PLAYER::PLAYER_PED_ID(), item.id, 1);
					else
						PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), item.id, item.drawable_id, 0, TRUE, 1);
					});
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		for (auto& item : props.items) {
			ImGui::SetNextItemWidth(60);
			if (ImGui::InputInt(std::format("{} {} [0,{}]##4", item.label, TRANSLATE("OUTFIT_TEX"), item.texture_id_max).c_str(), &item.texture_id)) {
				check_bounds_texture(&item); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([item] {
					PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), item.id, item.drawable_id, item.texture_id, TRUE, 1);
					});
			}
		}
		ImGui::EndGroup();
		//});
	}
	void protectionsTab() {
		child(TRANSLATE("PROTECTIONS_SCRIPT_EVENTS"), 210.f, [=] {
			ImGui::Combo(TRANSLATE("KICKS"), &protections::g_blockKick.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("CRASHES"), &protections::g_blockCrashes.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("BOUNTY"), &protections::g_blockBounty.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("CEO_KICK"), &protections::g_blockCEOKick.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("CEO_MONEY"), &protections::g_blockCEOMoney.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("CLEAR_WANTED_LVL"), &protections::g_blockClearWanted.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("FAKE_NOTIFY"), &protections::g_blockFakeDeposit.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("FORCE_TO_MISSION"), &protections::g_blockForceMission.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("GIVE_COLLECTIBLES"), &protections::g_blockGiveCollectible.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("GTA_BANNER"), &protections::g_blockGTABanner.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("MC_TELEPORT"), &protections::g_blockMCTeleport.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("PERSONAL_VEHICLE_DESTROY"), &protections::g_blockPersonalVehDestory.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("REMOTE_OFF_RADAR"), &protections::g_blockRemoteOffRadar.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("ROTATE_CAMERA"), &protections::g_blockRotateCam.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("SEND_TO_CUTSCENE"), &protections::g_blockSendToCutscene.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("SEND_TO_LOCATION"), &protections::g_blockSendToLocation.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("TSE_FREEZE"), &protections::g_blockTSEFreeze.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("SPECTATE"), &protections::g_blockSpectate.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("FORCE_TELEPORT"), &protections::g_blockForceTeleport.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("TRANSACTION_ERROR"), &protections::g_blockTransactionErr.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("VEHICLE_KICK"), &protections::g_blockVehKick.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("TELEPORT_TO_WAREHOUSE"), &protections::g_blockTeleportToWarehouse.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("START_ACTIVITY"), &protections::g_blockStartActivity.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("INTERIOR_CONTROL"), &protections::g_blockInteriorControl.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("SEND_TO_INTERIOR"), &protections::g_blockSendToInterior.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("KICK_FROM_INTERIOR"), &protections::g_blockKickFromInterior.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("NULL_FUNCTION_KICK"), &protections::g_blockNullFunctionKick.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("TRIGGER_CEO_RAID"), &protections::g_blockTriggerCeoRaid.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("START_SCRIPT_PROCEED"), &protections::g_blockProceedScript.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("SOUND_SPAM"), &protections::g_blockSoundSpam.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("PROTECTIONS_REPORTS_DESCRIPTION"), &protections::m_blockReports.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			});
		combineChilds();
		child(TRANSLATE("PROTECTIONS_NETWORK_EVENTS"), 210.f, [=] {
			ImGui::Combo(TRANSLATE("FREEZE"), &protections::g_blockNetworkFreeze.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("REQUEST_CONTROL"), &protections::m_blockRequestControl.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("RAGDOLL"), &protections::g_blockRagdollRequest.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("VOTE_KICK"), &protections::g_blockVoteKick.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("PTFX"), &protections::g_blockNetworkPTFX.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("ALTER_WANTED_LVL"), &protections::g_blockClearWanted.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("REMOVE_WEAPONS"), &protections::g_blockRemoveWeapons.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("REMOVE_WEAPON"), &protections::g_blockRemoveWeapon.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("GIVE_WEAPONS"), &protections::m_blockGiveWeapon.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("GIVE_CONTROL"), &protections::g_blockGiveControl.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("BLAME_EXPLOSION"), &protections::g_blockBlameExplosion.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("EXPLOSION"), &protections::g_blockExplosion.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			ImGui::Combo(TRANSLATE("WEAPON_DAMAGE"), &protections::g_blockDamageKarma.toggleType, m_toggleTypes.data(), m_toggleTypes.size());
			});
		child(TRANSLATE("PROTECTIONS_DETECTIONS"), 200.f, [=] {
			checkbox(TRANSLATE("HIDE_GODMODE"), features::spoof_hide_godmode);
			checkbox(TRANSLATE("HIDE_SPECTATE"), features::spoof_hide_spectate);
			checkbox(TRANSLATE("HIDE_FROM_PLAYER_LIST"), features::hide_from_player_list);
			checkbox(TRANSLATE("ADMIN_CHECK"), features::admin_check);
			checkbox(TRANSLATE("KICK_ADMINS"), features::kick_admins);
			checkbox(TRANSLATE("ADVERTISERS_KICK"), features::kick_advertisers);
			});
		combineChilds();
		child(TRANSLATE("RESPONCES"), 200.f, [=] {
			//checkbox(TRANSLATE("LOG_CHAT_MESSAGES"), features::log_chat_messages);
			checkbox(TRANSLATE("REJOIN_FROM_KICKED_SESSION"), features::rejoin_kicked_session);
			});
	}
	void playersTab() {
		auto npm = *g_pointers->m_netPlayerMgr;
		sectionBox("playerBox", ImVec2(200, -1), g_renderer->m_font, [=] {
			const auto player_count = g_player_service->players().size() + 1;
			auto mySelf = g_player_service->get_self();
			bool isSessionActive = *g_pointers->m_isSessionActive;
			if (isSessionActive && mySelf->is_valid()) {
				ImGui::PushID("PlayerSelection");
				if (auto playerString = g_player_service->GetPlayerIdentifications(mySelf); selectable(playerString, g_selectedPlayer == mySelf->id())) {
					g_selectedPlayer = mySelf->id();
					g_player_service->set_selected(g_player_service->get_by_id(g_selectedPlayer));
				}
				if (player_count > 1) {
					ImGui::Separator();
					for (const auto& [_, player] : g_player_service->players()) {
						auto playerString = g_player_service->GetPlayerIdentifications(player);
						if (player->is_valid() && selectable(playerString, g_selectedPlayer == player->id())) {
							g_selectedPlayer = player->id();
							g_player_service->set_selected(g_player_service->get_by_id(g_selectedPlayer));
						}
					}
				}
				ImGui::PopID();
			}
			});
		ImGui::SameLine();
		sectionBox("playerOptionBox", ImVec2(450, -1), g_renderer->m_font, [=] {
			auto SelectedPlayerFromMgr = npm->m_player_list[g_selectedPlayer];
			if (SelectedPlayerFromMgr && SelectedPlayerFromMgr->IsConnected()) {
				checkbox(TRANSLATE("NETWORK_SELECTED_PLAYER_SPECTATE"), features::SpectateBool);
				child(TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), 200.f, [] {
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_TO_PLAYER"), [] {
						TeleportToPlayer(g_player_service->get_selected());
						});
					button(TRANSLATE("TELEPORT_IN_PLAYER_VEHICLE"), [] {
						TeleportIntoPlayersVehicle(g_player_service->get_selected());
						});
					button(TRANSLATE("TELEPORT_PLAYER_TO_ME"), [] {
						TeleportPlayerToMe(g_player_service->get_selected());
						});
					button(TRANSLATE("TELEPORT_PLAYER_TO_WAYPOINT"), [] {
						TeleportPlayerToWaypoint(g_player_service->get_selected());
						});
					});
				child(TRANSLATE("NETWORK_SELECTED_PLAYER_FRIENDLY_OPTIONS"), 200.f, [] {
					button(TRANSLATE("WEAPONS_GIVE_WEAPONS"), [] {
						g_fiber_pool->queue_job([] {
							for (auto name : weaponHashes)
								WEAPON::GIVE_WEAPON_TO_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer), name, 9999, false, true);
							});
						});
					button(TRANSLATE("GIFT_VEHICLE_TO_PLAYER"), [] {
						g_fiber_pool->queue_job([] {
							auto vehicle = PED::GET_VEHICLE_PED_IS_USING(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer));
							if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer), FALSE)) {
								if (DECORATOR::DECOR_EXIST_ON(vehicle, "Player_Vehicle")) {
									MainNotification(ImGuiToastType_Error, 7000, TRANSLATE("ERROR"), TRANSLATE("PLAYER_ALREADY_OWNS_VEHICLE"));
									return;
								}
								if (take_control_of(vehicle))
								{
									ENTITY::SET_ENTITY_AS_MISSION_ENTITY(vehicle, TRUE, TRUE);
									DECORATOR::DECOR_REGISTER("PV_Slot", 3);
									DECORATOR::DECOR_REGISTER("Player_Vehicle", 3);
									DECORATOR::DECOR_SET_BOOL(vehicle, "IgnoredByQuickSave", FALSE);
									DECORATOR::DECOR_SET_INT(vehicle, "Player_Vehicle", NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(g_selectedPlayer));
									VEHICLE::SET_VEHICLE_IS_STOLEN(vehicle, FALSE);
									MainNotification(ImGuiToastType_Success, 7000, TRANSLATE("SUCCESS"), TRANSLATE("PLAYER_HAS_BEEN_GIFTED_VEHICLE"));
								}
								else
								{
									MainNotification(ImGuiToastType_Error, 7000, TRANSLATE("ERROR"), TRANSLATE("FAILED_TO_TAKE_CONTROL_OF_VEHICLE"));
								}
							}
							else
							{
								MainNotification(ImGuiToastType_Error, 7000, TRANSLATE("ERROR"), TRANSLATE("PLAYER_NOT_IN_ANY_VEHICLE"));
							}
							});
						});
					});
				child(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_OPTIONS"), 200.f, [] {
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_DESYNC"), [] {
						Toxic::DesyncKick(g_player_service->get_selected()->get_net_game_player());
						});
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_SH"), [] {
						Toxic::ScriptHostKick(g_player_service->get_selected());
						});
					button(TRANSLATE("HOST_KICK"), [] {
						Toxic::HostKick(g_player_service->get_selected());
						});
					button(TRANSLATE("HOST_KICK_BAN"), [] {
						if (g_player_service->get_self()->is_host()) {
							Toxic::BreakUpKick(g_player_service->get_selected());
						}
						});
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_NULL_FUNCTION"), [] {
						Toxic::NullFunctionKick(g_player_service->get_selected());
						});
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_BAIL"), [] {
						Toxic::BailKick(g_player_service->get_selected());
						});
					button(TRANSLATE("END_SESSION_KICK"), [] {
						Toxic::EndSessionKick(g_player_service->get_selected());
						});
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_CRASH_IPS"), [] {
						g_fiber_pool->queue_job([] {
							Toxic::InvalidPedCrash(g_player_service->get_selected());
							});
						});
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_CRASH_FB"), [] {
						g_fiber_pool->queue_job([] {
							Toxic::BreakFragmentCrash(g_player_service->get_selected());
							});
						});
					});
				child(TRANSLATE("NETWORK_SELECTED_PLAYER_GRIEFING_OPTIONS"), 120.f, [] {
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_TROLLING_CLONE"), [=] {
						auto plyrMgr = gta_util::get_network_player_mgr()->m_player_list[g_selectedPlayer];
						if (plyrMgr != nullptr && plyrMgr->IsConnected()) {
							gta_util::execute_as_script(*g_pointers->m_isSessionActive ? "freemode"_joaat : "main_persistent"_joaat, [plyrMgr] {
								(*g_pointers->m_pedFactory)->ClonePed(plyrMgr->m_player_info->m_ped, true, true, true);
								});
						}
						});
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_TROLLING_EXPLODE"), [] {
						g_fiber_pool->queue_job([] {
							auto playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer);
							auto playerCoords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
							FIRE::ADD_EXPLOSION(playerCoords, (int)eExplosionTag::EXP_TAG_BLIMP, 1000.f, true, false, 0.1f, false);
							});
						});
					button(TRANSLATE("NETWORK_SELECTED_PLAYER_TROLLING_REMOVE_WEAPONS"), [] {
						g_fiber_pool->queue_job([] {
							for (auto& weapons : weaponHashes)
								WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer), weapons);
							});
						});
					});
				//checkbox(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_SYNC"), g_player_service->get_selected()->block_clone_sync);
				//checkbox(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_CLONE_CREATE"), g_player_service->get_selected()->block_clone_create);
				//checkbox(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_ALL_SCRIPT_EVENT"), g_player_service->get_selected()->block_all_script_events);
				button(TRANSLATE("FRIEND_COPY_RID"), [SelectedPlayerFromMgr] {
					std::string rid = std::to_string(SelectedPlayerFromMgr->GetGamerInfo()->m_gamer_handle.m_rockstar_id);
					ImGui::SetClipboardText(rid.c_str());
					MainNotification(ImGuiToastType_Success, 4200, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("NETWORK_SET_CLIPBOARD_TO_U_INT"), rid);
					});
				button(TRANSLATE("FRIEND_COPY_NAME"), [SelectedPlayerFromMgr] {
					auto name = SelectedPlayerFromMgr->GetName();
					ImGui::SetClipboardText(name);
					MainNotification(ImGuiToastType_Success, 4200, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("NETWORK_SET_CLIPBOARD_TO_STR"), name);
					});
			}
			});
	}
	void networkTab() {
		auto friend_Regestry = g_pointers->m_friendRegistry;
		auto data = friend_Regestry->get(g_selectedFriend);
		const auto FriendCount = friend_Regestry->m_friend_count;
		child(TRANSLATE("NETWORK_FRIENDS"), 240.f, [=] {
			if (FriendCount == 0) {
				return ImGui::Text(TRANSLATE("NO_FRIENDS_DETECTED"));
			}
			for (int i = 0; i != FriendCount; i++) {
				if (selectable(friend_Regestry->get(i)->m_name, g_selectedFriend == i)) {
					g_selectedFriend = i;
				}
			}
			});
		combineChilds();
		if (FriendCount != 0) {
			child(TRANSLATE("FRIEND_INFORMATION"), 240.f, [data] {
				ImGui::Text(TRANSLATE("FRIEND_NAME"), data->m_name);
				ImGui::Text(TRANSLATE("FRIEND_RID"), data->m_rockstar_id);
				ImGui::Text(TRANSLATE("FRIEND_STATE"), getFriendStateStr(data->m_friend_state, data->m_is_joinable));
				ImGui::Separator();
				button(TRANSLATE("FRIEND_COPY_RID"), [data] {
					std::string rid = std::to_string(data->m_rockstar_id);
					ImGui::SetClipboardText(rid.c_str());
					});
				button(TRANSLATE("FRIEND_COPY_NAME"), [data] {
					auto name = data->m_name;
					ImGui::SetClipboardText(name);
					});
				});
		}
		child(TRANSLATE("NETWORK_SESSION_STARTER"), 240.f, [] {
			for (const auto& session_type : sessions) {
				selectable(session_type.name, false, [&session_type] {
					base::features::sessionJoin(session_type.id);
					});
			}
			});
		combineChilds();
		child(TRANSLATE("NETWORK_TRANSITION"), 240.f, [=] {
			checkbox(TRANSLATE("NETWORK_SEAMLESS_JOIN"), features::seamlessJoinBool);
			checkbox(TRANSLATE("FAST_JOINING"), features::fast_join);
			checkbox(TRANSLATE("FORCE_SCRIPT_HOST"), features::force_script_host);
			checkbox(TRANSLATE("FORCE_SESSION_HOST"), features::force_session_host);
			button(TRANSLATE("NETWORK_UNSTUCK"), [] {
				if (*globals(1574996).as<eTransitionState*>() == eTransitionState::TRANSITION_STATE_WAIT_HUD_EXIT) {
					*globals(1574996).as<eTransitionState*>() = eTransitionState::TRANSITION_STATE_LOOK_TO_JOIN_ANOTHER_SESSION_FM;
					return;
				}
				NETWORK::NETWORK_BAIL(0, 0, 16);
				NETWORK::NETWORK_BAIL_TRANSITION(0, 0, 16);
				MainNotification(ImGuiToastType_Warning, 4200, TRANSLATE("NETWORK_UNSTUCK"), TRANSLATE("FORCED_TO_BAIL_FROM_THE_SESSION"));
				});
			});
		combineChilds();
		child(TRANSLATE("NETWORK_RID_JOINER"), 120.f, [=] {
			static char InputTextForRid[20];
			ImGui::InputText(TRANSLATE("INPUT_RID_OR_NAME"), InputTextForRid, sizeof(InputTextForRid));
			if (ImGui::Button(TRANSLATE("JOIN"))) {
				std::string nameOrRid(InputTextForRid);
				int rid = isNumber(nameOrRid) ? std::stoi(nameOrRid) : g_api_service->nameToRid(nameOrRid);
				base::g_thread_pool->push([rid] {
					g_api_service->ridJoinViaSession(rid);
					});
			}
			});
	}
	void weaponsTab() {
		child(TRANSLATE("WEAPON_OPTIONS"), 240.f, [=] {
			checkbox(TRANSLATE("WEAPONS_INTERIOR_WEAPONS"), features::interior_weapon);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("INFINITE_AMMO"), features::infiniteAmmoBool);
			checkbox(TRANSLATE("TRIGGER_BOT"), features::TRIGGERBOT);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("RAPID_FIRE"), features::Rapidfirebool);
			checkbox(TRANSLATE("RAINBOW_WEAPONS"), features::RainbowWeaponsBool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("CAGE_GUN"), features::Cagegunbool);
			checkbox(TRANSLATE("GRAVITY_GUN"), features::Gravitygunbool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("DELETE_GUN"), features::Deletegunbool);
			checkbox(TRANSLATE("TELEPORT_GUN"), features::Teleportgungun);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("CARTOON_GUN"), features::CartoonGunbool);
			checkbox(TRANSLATE("PTFX_GUN"), features::Coolptfxgun);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("STEAL_GUN"), features::Stealgunbool);
			checkbox(TRANSLATE("MINECRAFT_GUN"), features::Mcgunbool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("AIRSTRIKE_GUN"), features::Airstrikebool);
			checkbox(TRANSLATE("RP_GUN"), features::RpGunbool);
			});
		combineChilds();
		child(TRANSLATE("GENERAL"), 240.f, [=] {
			button(TRANSLATE("WEAPONS_GIVE_WEAPONS"), [] {
				g_fiber_pool->queue_job([] {
					for (auto Weapons : weaponHashes) {
						WEAPON::GIVE_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), Weapons, 9999, 0, 1);
					}
					});
				});
			button(TRANSLATE("WEAPONS_REMOVE_WEAPONS"), [] {
				g_fiber_pool->queue_job([] {
					for (auto WeaponsToRemove : weaponHashes) {
						WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::PLAYER_PED_ID(), WeaponsToRemove);
					}
					});
				});
			});
	}
	void vehicleTab() {
		child(TRANSLATE("VEHICLE_OPTIONS"), 240.f, [=] {
			checkbox(TRANSLATE("VEHICLE_GODMODE"), features::VehicleGodmode);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("HORN_BOOST"), features::hornboost);
			checkbox(TRANSLATE("SEAT_BELT"), features::Seatbeltbool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("AUTO_REPAIR"), features::vehiclefixLoopBool);
			checkbox(TRANSLATE("MANUAL_HANDBREAK"), features::HandBreakBool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("SCORCH_VEHICLE"), features::Scrutchbool);
			checkbox(TRANSLATE("GROUND_VEHICLE"), features::StickyBool);
			ImGui::SameLine(150.0f, -1.0f);
			checkbox(TRANSLATE("DRIFT_MODE"), features::DriftBool);
			checkbox(TRANSLATE("MATRIX_PLATE"), features::MatrixPlates);
			button(TRANSLATE("UPGRADE_VEHICLE"), [] {
				g_fiber_pool->queue_job([] {
					if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0)) {
						auto vehicletorepair = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
						max_vehicle(vehicletorepair);
					}
					else
					{
						MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("VEHICLE_OPTIONS"), TRANSLATE("YOU_ARE_NOT_IN_VEHICLE"));
						return;
					}
					});
				});
			button(TRANSLATE("REPAIR_VEHICLE"), [] {
				g_fiber_pool->queue_job([] {
					if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0)) {
						auto vehicletorepair = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
						repair(vehicletorepair);
					}
					else
					{
						MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("VEHICLE_OPTIONS"), TRANSLATE("YOU_ARE_NOT_IN_VEHICLE"));
						return;
					}
					});
				});
			button(TRANSLATE("DELETE_VEHICLE"), [] {
				g_fiber_pool->queue_job([&] {
					if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0)) {
						TASK::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::PLAYER_PED_ID());
						auto veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), TRUE);
						if (take_control_of(veh)) {
							delete_entity(veh);
						}
						else
						{
							MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("VEHICLE_OPTIONS"), TRANSLATE("FAILED_TO_TAKE_CONTROL_OF_VEHICLE"));
							return;
						}
					}
					else {
						MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("VEHICLE_OPTIONS"), TRANSLATE("YOU_ARE_NOT_IN_VEHICLE"));
					}
					});
				});
			});
		combineChilds();
		child(TRANSLATE("VEHICLE_COLOR"), 240.f, [=] {
			ImGui::Combo("Rainbow Vehicle", &RainbowSelect, RainBowVehicleNames.data(), RainBowVehicleNames.size());
			ImGui::ColorEdit4("Primary", primaryCol, ImGuiColorEditFlags_NoAlpha);
			ImGui::SameLine();
			button("Apply", [&] {
				auto selfPed = PLAYER::PLAYER_PED_ID;
				auto vehiclePedIsIn = PED::GET_VEHICLE_PED_IS_IN(selfPed(), FALSE);
				auto primCol = convertFloatTableToColorTable(primaryCol);
				if (PED::IS_PED_IN_ANY_VEHICLE(selfPed(), FALSE)) {
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehiclePedIsIn, primCol.r, primCol.g, primCol.b);
				}
				});
			ImGui::ColorEdit4("Secondary", secondaryCol, ImGuiColorEditFlags_NoAlpha);
			ImGui::SameLine();
			button("Apply", [&] {
				auto selfPed = PLAYER::PLAYER_PED_ID;
				auto vehiclePedIsIn = PED::GET_VEHICLE_PED_IS_IN(selfPed(), FALSE);
				auto secdCol = convertFloatTableToColorTable(secondaryCol);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehiclePedIsIn, secdCol.r, secdCol.g, secdCol.b);
				});
			ImGui::ColorEdit4("Tire Smoke", tireSmokeCol, ImGuiColorEditFlags_NoAlpha);
			ImGui::SameLine();
			button("Apply", [&] {
				auto selfPed = PLAYER::PLAYER_PED_ID;
				auto vehiclePedIsIn = PED::GET_VEHICLE_PED_IS_IN(selfPed(), FALSE);
				auto tiresmkCol = convertFloatTableToColorTable(tireSmokeCol);
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehiclePedIsIn, tiresmkCol.r, tiresmkCol.g, tiresmkCol.b);
				});
			ImGui::ColorEdit4("Neons", nenonlightsCol, ImGuiColorEditFlags_NoAlpha);
			ImGui::SameLine();
			button("Apply", [&] {
				auto selfPed = PLAYER::PLAYER_PED_ID;
				auto vehiclePedIsIn = PED::GET_VEHICLE_PED_IS_IN(selfPed(), FALSE);
				auto neonlCol = convertFloatTableToColorTable(nenonlightsCol);
				VEHICLE::SET_VEHICLE_NEON_COLOUR(vehiclePedIsIn, neonlCol.r, neonlCol.g, neonlCol.b);
				});
			});
		child(TRANSLATE("VEHICLE_SPAWNER"), 240.f, [=] {
			static char inputTextForModel[20];
			ImGui::InputText(TRANSLATE("VEHICLE_SPAWN_BY_NAME"), inputTextForModel, sizeof(inputTextForModel));
			button(TRANSLATE("VEHICLE_SPAWN_VEHICLE"), [] {
				g_fiber_pool->queue_job([=] {
					SpawnVehicle(rage::joaat(inputTextForModel));
					});
				});
			if (ImGui::TreeNode(TRANSLATE("VEHICLE_SPAWN_OPTIONS"))) {
				checkbox(TRANSLATE("MAX_VEHICLE_SPAWN"), MaxVehiclespawn);
				checkbox(TRANSLATE("SET_IN_VEHICLE_SPAWNED"), Sitinvehicle);
				checkbox(TRANSLATE("SPAWN_WITH_EFFECT"), bSpawnWithEffect);
				ImGui::TreePop();
			}
			for (uint8_t i{}; i != 23; ++i) {
				if (ImGui::TreeNode(util::vehicle::g_modelClasses[i].c_str())) {
					util::vehicle::g_selectedVehicleClass = i;
					auto vehicles{ util::vehicle::g_models.get(util::vehicle::g_selectedVehicleClass) };
					for (size_t i{}; i != vehicles.size(); ++i) {
						auto& veh{ vehicles[i] };
						button(veh.m_finalLabel, [=] {
							g_fiber_pool->queue_job([=] {
								SpawnVehicle(veh.m_modelInfo->m_hash);
								});
							});
					}
					ImGui::TreePop();
				}
			}
			});
	}
	void teleportTab() {
		child(TRANSLATE("TELEPORT_OPTIONS"), 200.f, [=] {
			//ImGui::Combo(TRANSLATE("TELEPORT_ANIMATION"), &TeleportState, TeleportAnimationNames, IM_ARRAYSIZE(TeleportAnimationNames));
			button(TRANSLATE("TELEPORT_TO_WAYPOINT"), [] {
				g_fiber_pool->queue_job([] {
					to_waypoint();
					});
				});
			button(TRANSLATE("TELEPORT_TO_OBJECTIVE"), [] {
				g_fiber_pool->queue_job([] {
					to_objective();
					});
				});
			});
	}
	void recoveryTab() {
		child(TRANSLATE("RECOVERY_OPTIONS"), 210.f, [=] {
			static int InputRank = 0; // Initialize the variable with a default value
			ImGui::InputInt(TRANSLATE("RECOVERY_INPUT_RANK"), &InputRank);
			button(TRANSLATE("RECOVERY_APPLY_RANK"), [] {
				setRank(InputRank);
				});
			button(TRANSLATE("RECOVERY_CLEAR_REPORTS"), [] {
				ClearAllReports();
				});
			});
	}
	void settingsTab() {
		child(TRANSLATE("SETTINGS_OPTIONS"), 125.f, [] {
			button(TRANSLATE("MENU_UNLOAD"), [] {
				g_running = false;
				});
			ImGui::SameLine();
			button(TRANSLATE("EXIT_GAME"), [] {
				TerminateProcess(GetCurrentProcess(), 0);
				});
			checkbox(TRANSLATE("CHANGE_UI_MODE"), g_mainScript.m_listUi);
			checkbox(TRANSLATE("INFO_OVERLAY"), features::InfoOverlay);
			});
	}
	void tab(const char* icon, int id) {
		auto selected = (tabId == id);
		auto const& Colors = ImGui::GetStyle().Colors;
		auto colorIdx = selected ? ImGuiCol_ButtonActive : ImGuiCol_Button;
		ImGui::PushStyleColor(ImGuiCol_Button, Colors[colorIdx]);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
		if (ImGui::Button(icon, ImVec2(50.f, 50.f))) {
			tabId = id;
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}
	void guiScript::onPresentTick() { //X  //Y
		ImGui::SetNextWindowSize(ImVec2(720, 540), ImGuiCond_Once);
		ImGui::PushFont(g_renderer->m_RD2Font);
		if (ImGui::Begin(brandingName, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
			sectionBox("tabBox", ImVec2(65, -1), g_renderer->m_bigfontawesome, [] {
				tab(ICON_FA_USER, 0); //Self
				tab(ICON_FA_SHIELD_ALT, 1); //Protections
				tab(ICON_FA_USERS, 2); //Players
				tab(ICON_FA_GLOBE, 3); //Network
				tab(ICON_FA_BOMB, 4); //Weapons
				tab(ICON_FA_CAR, 5); //Vehicle
				tab(ICON_FA_LOCATION_DOT, 6); //Teleport
				tab(ICON_FA_HAND_HOLDING_DOLLAR, 7); //Recovery
				tab(ICON_FA_GEAR, 8); //Settings
				});
			ImGui::SameLine(68.f, -1.0f);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();
			sectionBox("optionBox", ImVec2(-1, -1), g_renderer->m_ClicKTitleFont, [=] {
				switch (tabId) {
				case 0: selfTab(); break; //Self
				case 1: protectionsTab(); break; //Protections
				case 2: playersTab(); break; //Players
				case 3: networkTab(); break; //Network
				case 4: weaponsTab(); break; //Weapons
				case 5: vehicleTab(); break; //Vehicle
				case 6: teleportTab(); break; //Teleport
				case 7: recoveryTab(); break; //Recovery
				case 8: settingsTab(); break; //Settings
				}
				});
			ImGui::End();
		}
		ImGui::PopFont();
	}
	void guiScript::tick() {
		base::util::vehicle::cacheModelTable();
		g_mainScript.initUi();
		while (true) {
			if (g_dxUiManager.m_open && !g_mainScript.m_listUi) {
				for (uint8_t i = 0; i <= 6; i++)
					PAD::DISABLE_CONTROL_ACTION(2, i, true);
				PAD::DISABLE_CONTROL_ACTION(2, 106, true);
				PAD::DISABLE_CONTROL_ACTION(2, 329, true);
				PAD::DISABLE_CONTROL_ACTION(2, 330, true);
				PAD::DISABLE_CONTROL_ACTION(2, 14, true);
				PAD::DISABLE_CONTROL_ACTION(2, 15, true);
				PAD::DISABLE_CONTROL_ACTION(2, 16, true);
				PAD::DISABLE_CONTROL_ACTION(2, 17, true);
				PAD::DISABLE_CONTROL_ACTION(2, 24, true);
				PAD::DISABLE_CONTROL_ACTION(2, 69, true);
				PAD::DISABLE_CONTROL_ACTION(2, 70, true);
				PAD::DISABLE_CONTROL_ACTION(2, 84, true);
				PAD::DISABLE_CONTROL_ACTION(2, 85, true);
				PAD::DISABLE_CONTROL_ACTION(2, 99, true);
				PAD::DISABLE_CONTROL_ACTION(2, 92, true);
				PAD::DISABLE_CONTROL_ACTION(2, 100, true);
				PAD::DISABLE_CONTROL_ACTION(2, 114, true);
				PAD::DISABLE_CONTROL_ACTION(2, 115, true);
				PAD::DISABLE_CONTROL_ACTION(2, 121, true);
				PAD::DISABLE_CONTROL_ACTION(2, 142, true);
				PAD::DISABLE_CONTROL_ACTION(2, 241, true);
				PAD::DISABLE_CONTROL_ACTION(2, 261, true);
				PAD::DISABLE_CONTROL_ACTION(2, 257, true);
				PAD::DISABLE_CONTROL_ACTION(2, 262, true);
				PAD::DISABLE_CONTROL_ACTION(2, 331, true);
			}
			else if (g_dxUiManager.m_open) {
				PAD::DISABLE_CONTROL_ACTION(0, 27, true);//phone
			}
			//Check for input
			g_dxUiManager.inputCheckHandler();
			//Handle the checked input
			g_dxUiManager.keypressHandler();
			script::get_current()->yield();
		}
	}
}