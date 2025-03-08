#include "MainScript.hpp"
#include "GuiScript.hpp"
#include "Features.hpp"
#include "ScriptGlobal.hpp"
#include "ThreadPool.hpp"
#include "HashList.hpp"
#include "vecBool.hpp"
#include "Teleport.hpp"
#include "Translations.hpp"
#include "KicksAndCrashes.hpp"
#include "Fibers/fiber_pool.hpp"
#include "Services/Api/api_service.hpp"
#include "Services/Friend/friends_service.hpp"
#include "Labels.hpp"
#include "Vehicle.hpp"
#include "GTAV-Classes-master/vehicle/CVehicleModelInfo.hpp"
#include "ColorOpt.hpp"
#include "Protections.hpp"
#include "Recovery.hpp"
#include "VehModTable.hpp"
#include "OutfitEditor.hpp"
#define INPUT_MAX 1024
namespace base {
	using namespace listUiTypes;
	bool containPictExt(const fs::path& path) {
		static const ankerl::unordered_dense::set<std::string> supportedFormats = { ".png", ".jpeg", ".bmp", ".psd", ".tga", ".gif", ".hdr", ".pic", ".ppm", ".pgm" };
		const auto ext = path.extension().string();
		return supportedFormats.count(ext) != 0;
	}
	bool containJson(const std::filesystem::path& path) {
		static const ankerl::unordered_dense::set<std::string> supportedFormats = { ".json" };
		const auto ext = path.extension().string();
		return supportedFormats.find(ext) != supportedFormats.end();
	}
	using Criterion = std::function<void(std::string)>;
	void inputText(sub* sub, const std::string buttonTxt, Criterion buttonFn) {
		static auto txt = buttonTxt + "###test_i";
		g_rendererQueue.createInputLockFiber(txt);
		g_rendererQueue.add(txt, [sub, buttonTxt, buttonFn] {
			if (sub->getName() == g_dxUiManager.m_submenuStack.top()->getName()) {
				if (ImGui::Begin(txt.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground)) {
					auto center = ImGui::GetIO().DisplaySize;
					static char textBuf[2048] = "";
					ImGui::SetWindowSize(g_dxUiManager.scale(g_dxUiManager.m_width + 10.f, 200.f, g_dxUiManager.getRes()));
					ImGui::SetWindowPos(ImVec2(center.x / 2.f - 200.f, center.y / 2.f));
					ImGui::SetWindowFontScale(g_dxUiManager.m_scale);
					g_dxUiManager.rectOutline(ImVec2(center.x / 2.f - 200.f, center.y / 2.f), ImVec2(g_dxUiManager.m_width + 10.f, 30.f), g_dxUiManager.m_headerRectColor, false, 3.f);
					ImGui::GetIO().MouseDrawCursor = true;
					g_dxUiManager.m_forceLockInput = true;
					base::features::DisableAllActions = true;
					ImGui::SetNextItemWidth(g_dxUiManager.scale(g_dxUiManager.m_width, g_dxUiManager.getRes()));
					ImGui::InputText(("##message_" + buttonTxt).c_str(), textBuf, sizeof(textBuf));
					if (ImGui::Button(txt.c_str())) {
						if (!std::string(textBuf).empty()) {
							buttonFn(textBuf);
							ZeroMemory(textBuf, sizeof(textBuf));
						}
						ImGui::GetIO().MouseDrawCursor = false;
						g_dxUiManager.m_forceLockInput = false;
						base::features::DisableAllActions = false;
						g_rendererQueue.removeInputLockFiber(txt);
						g_rendererQueue.remove(txt);
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel")) {
						ImGui::GetIO().MouseDrawCursor = false;
						g_dxUiManager.m_forceLockInput = false;
						base::features::DisableAllActions = false;
						g_rendererQueue.removeInputLockFiber(txt);
						g_rendererQueue.remove(txt);
					}
					ImGui::End();
				}
			}
			});
	}
	const std::string m_TranslationDirList = getenv("APPDATA");
	std::filesystem::path RealTranslationPath = m_TranslationDirList + "\\LX\\Translations";
	void mainScript::initUi() {
		//g_config.load(std::string());
		auto friendReg = g_pointers->m_friendRegistry;
		auto cPed = (*g_pointers->m_pedFactory)->m_local_ped;
		auto cPlayerInfo = cPed->m_player_info;
		auto const& rlGamerInfo = cPlayerInfo->m_net_player_data;
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
		g_dxUiManager.addSub(sub().setName(TRANSLATE("HOME")).setId("home.home"_id).setAction([=](sub* sub) {
			sub->addOption(subOpt().setLeft(TRANSLATE("SELF_OPTIONS")).setId("home.self"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("PROTECTION_OPTIONS")).setId("home.protections"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_OPTIONS")).setId("home.network"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("WEAPON_OPTIONS")).setId("home.weapons"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("VEHICLE_OPTIONS")).setId("home.vehicle"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("TELEPORT_OPTIONS")).setId("home.teleport"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("RECOVERY_OPTIONS")).setId("home.recovery"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("MISC_OPTIONS")).setId("home.misc"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("SETTINGS_OPTIONS")).setId("home.settings"_id));
			}));
		//Self
		g_dxUiManager.addSub(sub().setName(TRANSLATE("SELF_OPTIONS")).setId("home.self"_id).setAction([=](sub* sub) {
			sub->addOption(subOpt().setLeft(TRANSLATE("SKIN_CHANGER")).setId("home.self.skin"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("SELF_OUTFIT_EDITOR")).setId("home.self.outfit_editor"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("SELF_MOVEMENT")).setId("home.self.movement"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("SELF_HEALTH")).setId("home.self.health"_id));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_GODMODE")).setDesc(TRANSLATE("SELF_GODMODE_DESCRIPTION")).setBool(features::godModeBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_NEVER_WANTED")).setDesc(TRANSLATE("SELF_NEVER_WANTED_DESCRIPTION")).setBool(features::neverWantedBool));
			sub->addOption(vecBoolOpt<char const*, int>().setLeft(TRANSLATE("SELF_INVISIBILITY")).setDesc(TRANSLATE("SELF_INVISIBILITY_DESCRIPTION")).setArray(m_invisibleToggleType).setPos(InvisibleState).setBool(features::invisibilityBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_NO_CLIP")).setDesc(TRANSLATE("SELF_NO_CLIP_DESCRIPTION")).setBool(features::NoClipBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_NO_RAGDOLL")).setDesc(TRANSLATE("SELF_NO_RAGDOLL_DESCRIPTION")).setBool(features::noRagdollBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_NO_COLLISION")).setDesc(TRANSLATE("SELF_NO_COLLISION_DESCRIPTION")).setBool(features::selfNoCollision));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_FREECAM")).setDesc(TRANSLATE("SELF_FREECAM_DESCRIPTION")).setBool(features::Freecambool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_KEEP_PLAYER_CLEAN")).setDesc(TRANSLATE("SELF_KEEP_PLAYER_CLEAN_DESCRIPTION")).setBool(features::keepSelfCleanBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_MOBILE_RADIO")).setDesc(TRANSLATE("SELF_MOBILE_RADIO_DESCRIPTION")).setBool(features::mobileRadioBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_RESPAWN_AT_PLACE_OF_DEATH")).setDesc(TRANSLATE("SELF_RESPAWN_AT_PLACE_OF_DEATH_DESCRIPTION")).setBool(features::FastRespawnBool));
			sub->addOption(numOpt<uint32_t>().setLeft(TRANSLATE("SELF_WANTED_LVL")).setNum(gta_util::get_local_playerinfo()->m_wanted_level).setStep(1).setMinMax(0, 5).setAction([=] {
				gta_util::get_local_ped()->m_player_info->m_wanted_level = features::wanted_level;
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("SELF_SUICIDE")).setDesc(TRANSLATE("SELF_SUICIDE_DESCRIPTION")).setAction([=] { gta_util::get_local_ped()->m_health = NULL; }));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("SKIN_CHANGER")).setId("home.self.skin"_id).setAction([=](sub* sub) {
			sub->addOption(subOpt().setLeft(TRANSLATE("ANIMALS")).setId("home.self.animals"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("MULTIPLAYER")).setId("home.self.multiplayer"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("STORYMODE")).setId("home.self.storymode"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("FEMALE")).setId("home.self.female"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("ALL")).setId("home.self.all"_id));
			sub->addOption(regularOpt().setLeft(TRANSLATE("FIND_MODEL")).setAction([=] {
				inputText(sub, TRANSLATE("SEARCH"), [](std::string const& findmodel) {
					g_fiber_pool->queue_job([=] {
						features::applyChosenSkin(rage::joaat(findmodel));
						});
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("SELF_OUTFIT_EDITOR")).setId("home.self.outfit_editor"_id).setAction([=](sub* sub) {
			static components_t components;

			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_ACCS")).setId("home.self.outfit_editor.accs"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_HAIR")).setId("home.self.outfit_editor.hair"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_HEAD")).setId("home.self.outfit_editor.head"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_BEARD")).setId("home.self.outfit_editor.beard"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_UPPR")).setId("home.self.outfit_editor.upper"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_LOWR")).setId("home.self.outfit_editor.lower"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_FEET")).setId("home.self.outfit_editor.feet"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_HAND")).setId("home.self.outfit_editor.hand"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_BODY_ACCS")).setId("home.self.outfit_editor.body_accs"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_TEEF")).setId("home.self.outfit_editor.teef"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_TASK")).setId("home.self.outfit_editor.task"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_DECL")).setId("home.self.outfit_editor.decl"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_JBIB")).setId("home.self.outfit_editor.jbib"_id));

			sub->addOption(regularOpt().setLeft(TRANSLATE("OUTFIT_RANDOM_COMPONENT")).setAction([=] {
				g_fiber_pool->queue_job([=] {
					set_ped_random_component_variation(PLAYER::PLAYER_PED_ID());
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("OUTFIT_DEFAULT_COMPONENT")).setAction([=] {
				g_fiber_pool->queue_job([=] {
					PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_ACCS")).setId("home.self.outfit_editor.accs"_id).setAction([=](sub* sub) {
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_HATS")).setId("home.self.outfit_editor.hats"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_GLASSES")).setId("home.self.outfit_editor.glasses"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("OUTFIT_EARS")).setId("home.self.outfit_editor.ears"_id));
			sub->addOption(regularOpt().setLeft(TRANSLATE("OUTFIT_RANDOM_PROPS")).setAction([=] {
				g_fiber_pool->queue_job([=] {
					PED::SET_PED_RANDOM_PROPS(PLAYER::PLAYER_PED_ID());
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("OUTFIT_CLEAR_PROPS")).setAction([=] {
				g_fiber_pool->queue_job([=] {
					PED::CLEAR_ALL_PED_PROPS(PLAYER::PLAYER_PED_ID(), 1);
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_HATS")).setId("home.self.outfit_editor.hats"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(props.items[0].drawable_id).setStep(1).setMinMax(0, props.items[0].drawable_id_max).setAction([=] {
				check_bounds_drawable(&props.items[0]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					if (props.items[0].drawable_id == -1)
						PED::CLEAR_PED_PROP(PLAYER::PLAYER_PED_ID(), props.items[0].id, 1);
					else
						PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), props.items[0].id, props.items[0].drawable_id, 0, TRUE, 1);
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(props.items[0].texture_id).setStep(1).setMinMax(0, props.items[0].texture_id_max).setAction([=] {
				check_bounds_texture(&props.items[0]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), props.items[0].id, props.items[0].drawable_id, props.items[0].texture_id, TRUE, 1);
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_GLASSES")).setId("home.self.outfit_editor.glasses"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(props.items[1].drawable_id).setStep(1).setMinMax(0, props.items[1].drawable_id_max).setAction([=] {
				check_bounds_drawable(&props.items[1]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					if (props.items[1].drawable_id == -1)
						PED::CLEAR_PED_PROP(PLAYER::PLAYER_PED_ID(), props.items[1].id, 1);
					else
						PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), props.items[1].id, props.items[1].drawable_id, 0, TRUE, 1);
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(props.items[1].texture_id).setStep(1).setMinMax(0, props.items[1].texture_id_max).setAction([=] {
				check_bounds_texture(&props.items[1]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), props.items[1].id, props.items[1].drawable_id, props.items[1].texture_id, TRUE, 1);
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_EARS")).setId("home.self.outfit_editor.ears"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(props.items[2].drawable_id).setStep(1).setMinMax(0, props.items[2].drawable_id_max).setAction([=] {
				check_bounds_drawable(&props.items[2]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					if (props.items[2].drawable_id == -1)
						PED::CLEAR_PED_PROP(PLAYER::PLAYER_PED_ID(), props.items[2].id, 1);
					else
						PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), props.items[2].id, props.items[2].drawable_id, 0, TRUE, 1);
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(props.items[1].texture_id).setStep(1).setMinMax(0, props.items[1].texture_id_max).setAction([=] {
				check_bounds_texture(&props.items[1]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), props.items[2].id, props.items[2].drawable_id, props.items[2].texture_id, TRUE, 1);
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_HEAD")).setId("home.self.outfit_editor.head"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[0].drawable_id).setStep(1).setMinMax(0, components.items[0].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[0]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[0].id, components.items[0].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[0].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[0].texture_id).setStep(1).setMinMax(0, components.items[0].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[0]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[0].id, components.items[0].drawable_id, components.items[0].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[0].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_BEARD")).setId("home.self.outfit_editor.beard"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[1].drawable_id).setStep(1).setMinMax(0, components.items[1].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[1]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[1].id, components.items[1].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[1].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[1].texture_id).setStep(1).setMinMax(0, components.items[1].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[1]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[1].id, components.items[1].drawable_id, components.items[1].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[1].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_HAIR")).setId("home.self.outfit_editor.hair"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[2].drawable_id).setStep(1).setMinMax(0, components.items[2].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[2]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[2].id, components.items[2].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[2].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[2].texture_id).setStep(1).setMinMax(0, components.items[2].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[2]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[2].id, components.items[2].drawable_id, components.items[2].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[2].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_UPPR")).setId("home.self.outfit_editor.upper"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[3].drawable_id).setStep(1).setMinMax(0, components.items[3].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[3]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[3].id, components.items[3].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[3].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[3].texture_id).setStep(1).setMinMax(0, components.items[3].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[3]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[3].id, components.items[3].drawable_id, components.items[3].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[3].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_LOWR")).setId("home.self.outfit_editor.lower"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[4].drawable_id).setStep(1).setMinMax(0, components.items[4].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[4]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[4].id, components.items[4].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[4].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[4].texture_id).setStep(1).setMinMax(0, components.items[4].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[4]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[4].id, components.items[4].drawable_id, components.items[4].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[4].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_HAND")).setId("home.self.outfit_editor.hand"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[5].drawable_id).setStep(1).setMinMax(0, components.items[5].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[5]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[5].id, components.items[5].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[5].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[5].texture_id).setStep(1).setMinMax(0, components.items[5].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[5]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[5].id, components.items[5].drawable_id, components.items[5].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[5].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_FEET")).setId("home.self.outfit_editor.feet"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[6].drawable_id).setStep(1).setMinMax(0, components.items[6].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[6]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[6].id, components.items[6].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[6].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[6].texture_id).setStep(1).setMinMax(0, components.items[6].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[6]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[6].id, components.items[6].drawable_id, components.items[6].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[6].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_TEEF")).setId("home.self.outfit_editor.teef"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[7].drawable_id).setStep(1).setMinMax(0, components.items[7].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[7]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[7].id, components.items[7].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[7].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[7].texture_id).setStep(1).setMinMax(0, components.items[7].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[7]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[7].id, components.items[7].drawable_id, components.items[7].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[7].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_ACCS")).setId("home.self.outfit_editor.body_accs"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[8].drawable_id).setStep(1).setMinMax(0, components.items[8].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[8]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[8].id, components.items[8].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[8].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[8].texture_id).setStep(1).setMinMax(0, components.items[8].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[8]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[8].id, components.items[8].drawable_id, components.items[8].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[8].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_TASK")).setId("home.self.outfit_editor.task"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[9].drawable_id).setStep(1).setMinMax(0, components.items[9].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[9]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[9].id, components.items[9].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[9].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[9].texture_id).setStep(1).setMinMax(0, components.items[9].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[9]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[9].id, components.items[9].drawable_id, components.items[9].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[9].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_DECL")).setId("home.self.outfit_editor.decl"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[10].drawable_id).setStep(1).setMinMax(0, components.items[10].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[10]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[10].id, components.items[10].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[10].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[10].texture_id).setStep(1).setMinMax(0, components.items[10].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[10]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[10].id, components.items[10].drawable_id, components.items[10].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[10].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("OUTFIT_JBIB")).setId("home.self.outfit_editor.jbib"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_VAR")).setNum(components.items[11].drawable_id).setStep(1).setMinMax(0, components.items[11].drawable_id_max).setAction([=] {
				check_bounds_drawable(&components.items[11]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[11].id, components.items[11].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[11].id));
					});
				}));
			sub->addOption(numOpt<int>().setLeft(TRANSLATE("OUTFIT_TEX")).setNum(components.items[11].texture_id).setStep(1).setMinMax(0, components.items[11].texture_id_max).setAction([=] {
				check_bounds_texture(&components.items[11]); // The game does this on it's own but seems to crash if we call OOB values to fast.
				g_fiber_pool->queue_job([&] {
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[11].id, components.items[11].drawable_id, components.items[11].texture_id, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), components.items[11].id));
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("SELF_MOVEMENT")).setId("home.self.movement"_id).setAction([=](sub* sub) {
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("SELF_MOVEMENT_SUPER_JUMP")).setArray(superjumpnamme).setPos(SuperJumpType));
			sub->addOption(numOpt<float>().setLeft(TRANSLATE("SELF_MOVEMENT_RUN_SPEED")).setDesc(TRANSLATE("SELF_MOVEMENT_RUN_SPEED_DESCRIPTION")).setNum(features::fastRunSpeed).setStep(0.1f).setMinMax(1.f, 10.f));
			sub->addOption(numOpt<float>().setLeft(TRANSLATE("SELF_MOVEMENT_SWIM_SPEED")).setDesc(TRANSLATE("SELF_MOVEMENT_SWIM_SPEED_DESCRIPTION")).setNum(features::fastSwimSpeed).setStep(0.1f).setMinMax(1.f, 2.9f));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_MOVEMENT_SUPER_RUN")).setDesc(TRANSLATE("SELF_MOVEMENT_SUPER_RUN_DESCRIPTION")).setBool(features::superRunBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_MOVEMENT_SWIM_IN_AIR")).setDesc(TRANSLATE("SELF_MOVEMENT_SWIM_IN_AIR_DESCRIPTION")).setBool(features::swimInAirBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_MOVEMENT_GRACEFUL_LANDING")).setDesc(TRANSLATE("SELF_MOVEMENT_GRACEFUL_LANDING_DESCRIPTION")).setBool(features::GracefulLanding));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_MOVEMENT_WALK_UNDERWATER")).setDesc(TRANSLATE("SELF_MOVEMENT_WALK_UNDERWATER_DESCRIPTION")).setBool(features::selfNoWaterCollision));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("SELF_HEALTH")).setId("home.self.health"_id).setAction([=](sub* sub) {
			sub->addOption(regularOpt().setLeft(TRANSLATE("SELF_MAX_HEALTH")).setDesc(TRANSLATE("SELF_MAX_HEALTH_DESCRIPTION")).setAction([=] {
				gta_util::get_local_ped()->m_health = gta_util::get_local_ped()->m_maxhealth;
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("SELF_MAX_ARMOUR")).setDesc(TRANSLATE("SELF_MAX_ARMOUR_DESCRIPTION")).setAction([=] {
				gta_util::get_local_ped()->m_armor = *g_pointers->m_isSessionActive ? gta_util::get_local_playerinfo()->m_max_armor : 200.f;
				}));
			sub->addOption(numBoolOpt<float>().setLeft(TRANSLATE("SELF_HEALTH_REGENERATION")).setDesc(TRANSLATE("SELF_HEALTH_REGENERATION_DESCRIPTION")).setBool(features::healthRegenbool).setNum(features::healthRegenSpeed).setStep(1.f).setMinMax(1.f, 100.f));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SELF_UNLIMITED_OXYGEN")).setDesc(TRANSLATE("SELF_UNLIMITED_OXYGEN_DESCRIPTION")).setBool(features::unlimitedOxygenBool));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("ANIMALS")).setId("home.self.animals"_id).setAction([=](sub* sub) {
			for (auto model : Animals)
				sub->addOption(regularOpt().setLeft(model).setAction([model] { features::applyChosenSkin(rage::joaat(model)); }));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("MULTIPLAYER")).setId("home.self.multiplayer"_id).setAction([=](sub* sub) {
			for (auto model : Multiplayerskins)
				sub->addOption(regularOpt().setLeft(model).setAction([model] { features::applyChosenSkin(rage::joaat(model)); }));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("STORYMODE")).setId("home.self.storymode"_id).setAction([=](sub* sub) {
			for (auto model : Storycharacter)
				sub->addOption(regularOpt().setLeft(model).setAction([model] { features::applyChosenSkin(rage::joaat(model)); }));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("FEMAIL")).setId("home.self.female"_id).setAction([=](sub* sub) {
			for (auto model : Female)
				sub->addOption(regularOpt().setLeft(model).setAction([model] { features::applyChosenSkin(rage::joaat(model)); }));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("ALL")).setId("home.self.all"_id).setAction([=](sub* sub) {
			for (auto model : pedModels)
				sub->addOption(regularOpt().setLeft(model).setAction([model] { features::applyChosenSkin(rage::joaat(model)); }));
			}));
		//Network
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_OPTIONS")).setId("home.network"_id).setAction([=](sub* sub) {
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_PLAYERS")).setId("home.network.playerlist"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_SESSION")).setId("home.network.session"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_FRIENDS")).setId("home.network.friends"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_RID_TOOLS")).setId("home.network.rid_tools"_id));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_PLAYERS")).setId("home.network.playerlist"_id).setAction([=](sub* sub) {
			const auto playerCount = g_player_service->players().size() + 1;

			if (*g_pointers->m_isSessionActive)
			{
				if (g_player_service->get_self()->is_valid())
				{
					auto playerString = g_player_service->GetPlayerIdentifications(g_player_service->get_self());
					auto selfAction = [=] {
						g_selectedPlayer = g_player_service->get_self()->id();
						g_player_service->set_selected(g_player_service->get_by_id(g_selectedPlayer));
						g_dxUiManager.getSubData("home.network.playerlist.selected"_id)->setName(g_player_service->get_self()->get_name()).setSelectedOpt(0);
					};
					sub->addOption(subOpt().setLeft(playerString).setId("home.network.playerlist.selected"_id).setAction(selfAction));
				}

				if (playerCount > 1)
				{
					for (const auto& [_, players] : g_player_service->players())
					{
						if (players && players->is_valid())
						{
							auto playerString = g_player_service->GetPlayerIdentifications(players);
							auto playerAction = [=] {
								g_selectedPlayer = players->id();
								g_player_service->set_selected(g_player_service->get_by_id(g_selectedPlayer));
								g_dxUiManager.getSubData("home.network.playerlist.selected"_id)->setName(players->get_name()).setSelectedOpt(0);
							};
							sub->addOption(subOpt().setLeft(playerString).setId("home.network.playerlist.selected"_id).setAction(playerAction));
						}
					}
				}
			}
			else
			{
				g_selectedPlayer = 0;
				auto rlGamerInfoAction = [&rlGamerInfo] {
					g_dxUiManager.getSubData("home.network.playerlist.selected"_id)->setName(rlGamerInfo.m_name).setSelectedOpt(0);
				};
				sub->addOption(subOpt().setLeft(rlGamerInfo.m_name).setId("home.network.playerlist.selected"_id).setAction(rlGamerInfoAction));
			}
			}));

		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SELECTED_PLAYER")).setId("home.network.playerlist.selected"_id).setAction([=](sub* sub) {
			if (*g_pointers->m_isSessionActive && g_player_service->get_selected() != nullptr)
				g_dxUiManager.drawPlayerinfo(g_player_service->get_selected());
			sub->addOption(boolOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_SPECTATE")).setBool(features::SpectateBool));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS")).setId("home.network.player.teleport"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_VEHICLE_OPTIONS")).setId("home.network.player.vehicle"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_FRIENDLY_OPTIONS")).setId("home.network.player.friendly"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_GRIEFING_OPTIONS")).setId("home.network.player.griefing"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_SCRIPTS_OPTIONS")).setId("home.network.player.scripts"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_OPTIONS")).setId("home.network.player.removal"_id));
			//sub->addOption(boolOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_SYNC")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_SYNC_DESCRIPTION")).setBool(g_player_service->get_selected()->block_clone_sync));
			//sub->addOption(boolOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_CLONE_CREATE")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_CLONE_CREATE_DESCRIPTION")).setBool(g_player_service->get_selected()->block_clone_create));
			//sub->addOption(boolOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_ALL_SCRIPT_EVENT")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_BLOCK_ALL_SCRIPT_EVENT_DESCRIPTION")).setBool(g_player_service->get_selected()->block_all_script_events));
			sub->addOption(regularOpt().setLeft(TRANSLATE("FRIEND_COPY_RID")).setAction([] {
				std::string rid = std::to_string(g_player_service->get_selected()->get_net_data()->m_gamer_handle.m_rockstar_id);
				ImGui::SetClipboardText(rid.c_str());
				MainNotification(ImGuiToastType_Success, 4200, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("NETWORK_SET_CLIPBOARD_TO_U_INT"), rid);
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("FRIEND_COPY_NAME")).setAction([] {
				auto name = g_player_service->get_selected()->get_name();
				ImGui::SetClipboardText(name);
				MainNotification(ImGuiToastType_Success, 4200, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("NETWORK_SET_CLIPBOARD_TO_STR"), name);
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS")).setId("home.network.player.teleport"_id).setAction([=](sub* sub) {
			if (*g_pointers->m_isSessionActive)
				g_dxUiManager.drawPlayerinfo(g_player_service->get_selected());
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_TO_PLAYER")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_TO_PLAYER_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					TeleportToPlayer(g_player_service->get_selected());
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("TELEPORT_IN_PLAYER_VEHICLE")).setDesc(TRANSLATE("TELEPORT_IN_PLAYER_VEHICLE_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					TeleportIntoPlayersVehicle(g_player_service->get_selected());
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("TELEPORT_PLAYER_TO_ME")).setDesc(TRANSLATE("TELEPORT_PLAYER_TO_ME_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					TeleportPlayerToMe(g_player_service->get_selected());
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("TELEPORT_PLAYER_TO_WAYPOINT")).setDesc(TRANSLATE("TELEPORT_PLAYER_TO_WAYPOINT_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					TeleportPlayerToWaypoint(g_player_service->get_selected());
					});
				}));
			}));
		//g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SELECTED_PLAYER_VEHICLE_OPTIONS")).setId("home.network.player.vehicle"_id).setAction([=](sub* sub) {
		//	if (*g_pointers->m_isSessionActive)
		//		g_dxUiManager.drawPlayerinfo(g_player_service->get_selected());
		//	}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SELECTED_PLAYER_FRIENDLY_OPTIONS")).setId("home.network.player.friendly"_id).setAction([=](sub* sub) {
			if (*g_pointers->m_isSessionActive)
				g_dxUiManager.drawPlayerinfo(g_player_service->get_selected());
			sub->addOption(regularOpt().setLeft(TRANSLATE("WEAPONS_GIVE_WEAPONS")).setAction([=] {
				g_fiber_pool->queue_job([] {
					for (auto name : weaponHashes)
						WEAPON::GIVE_WEAPON_TO_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer), name, 9999, false, true);
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("GIFT_VEHICLE_TO_PLAYER")).setDesc(TRANSLATE("GIFT_VEHICLE_TO_PLAYER_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					auto vehicle = PED::GET_VEHICLE_PED_IS_USING(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer));
					if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer), FALSE)) {
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
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SELECTED_PLAYER_GRIEFING_OPTIONS")).setId("home.network.player.griefing"_id).setAction([=](sub* sub) {
			if (*g_pointers->m_isSessionActive)
				g_dxUiManager.drawPlayerinfo(g_player_service->get_selected());
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_TROLLING_CLONE")).setAction([=] {
				auto plyrMgr = gta_util::get_network_player_mgr()->m_player_list[g_selectedPlayer];
				if (plyrMgr != nullptr && plyrMgr->IsConnected()) {
					gta_util::execute_as_script(*g_pointers->m_isSessionActive ? "freemode"_joaat : "main_persistent"_joaat, [plyrMgr] {
						(*g_pointers->m_pedFactory)->ClonePed(plyrMgr->m_player_info->m_ped, true, true, true);
						});
				}
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_TROLLING_EXPLODE")).setAction([=] {
				g_fiber_pool->queue_job([] {
					auto playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer);
					auto playerCoords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
					FIRE::ADD_EXPLOSION(playerCoords, (int)eExplosionTag::EXP_TAG_BLIMP, 1000.f, true, false, 0.1f, false);
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_TROLLING_REMOVE_WEAPONS")).setAction([=] {
				g_fiber_pool->queue_job([] {
					for (auto& weapons : weaponHashes)
						WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer), weapons);
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_OPTIONS")).setId("home.network.player.removal"_id).setAction([=](sub* sub) {
			if (*g_pointers->m_isSessionActive)
				g_dxUiManager.drawPlayerinfo(g_player_service->get_selected());
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_DESYNC")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_DESYNC_DESCRIPTION")).setArray(DesyncKickTypeNames).setPos(DesyncKickType).setAction([=] {
				Toxic::DesyncKick(g_player_service->get_selected()->get_net_game_player());
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_SH")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_SH_DESCRIPTION")).setAction([=] {
				Toxic::ScriptHostKick(g_player_service->get_selected());
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("HOST_KICK")).setDesc(TRANSLATE("HOST_KICK_DESCRIPTION")).setAction([=] {
				Toxic::HostKick(g_player_service->get_selected());
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("HOST_KICK_BAN")).setDesc(TRANSLATE("HOST_KICK_BAN_DESCRIPTION")).setAction([=] {
				if (g_player_service->get_self()->is_host()) {
					Toxic::BreakUpKick(g_player_service->get_selected());
				}
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_NULL_FUNCTION")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_NULL_FUNCTION_DESCRIPTION")).setAction([=] {
				Toxic::NullFunctionKick(g_player_service->get_selected());
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_BAIL")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_KICK_BAIL_DESCRIPTION")).setAction([=] {
				Toxic::BailKick(g_player_service->get_selected());
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("END_SESSION_KICK")).setDesc(TRANSLATE("END_SESSION_KICK_DESCRIPTION")).setAction([=] {
				Toxic::EndSessionKick(g_player_service->get_selected());
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_CRASH_IPS")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_CRASH_IPS_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([=] {
					Toxic::InvalidPedCrash(g_player_service->get_selected());
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_CRASH_FB")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_REMOVAL_CRASH_IPS_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([=] {
					Toxic::BreakFragmentCrash(g_player_service->get_selected());
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_SELECTED_PLAYER_ADD_TO_BLACKLIST")).setDesc(TRANSLATE("NETWORK_SELECTED_PLAYER_ADD_TO_BLACKLIST_DESCRIPTION")).setAction([=] {
				auto Name = g_player_service->get_selected()->get_name();
				kicked_Players.emplace(Name);
				if (gta_util::get_network()->m_game_session_ptr->is_host()) {
					Toxic::HostKick(g_player_service->get_selected());
				}
				else {
					Toxic::NullFunctionKick(g_player_service->get_selected());
					Toxic::BailKick(g_player_service->get_selected());
					Toxic::ScriptHostKick(g_player_service->get_selected());
					Toxic::DesyncKick(g_player_service->get_selected()->get_net_game_player());
				}
				above_map(std::string(Name) + std::string(TRANSLATE("NETWORK_BLACKLIST_ADDED_PLAYER")));
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_FRIENDS")).setId("home.network.friends"_id).setAction([=](sub* sub) {
			if (friendReg->m_friend_count != 0) {
				for (int i = 0; i < friendReg->m_friend_count; i++) {
					auto offlineStr = " " + getFriendStateStr(friendReg->get(i)->m_friend_state, friendReg->get(i)->m_is_joinable);
					sub->addOption(subOpt().setLeft(friendReg->get(i)->m_name + offlineStr).setId("home.network.friends.selected"_id).setAction([=] {
						g_dxUiManager.getSubData("home.network.friends.selected"_id)->setName(friendReg->get(i)->m_name);
						g_selectedFriend = i;
						}));
				}
			}
			else
			{
				sub->addOption(regularOpt().setLeft(TRANSLATE("NO_FRIENDS_DETECTED")).setDesc(TRANSLATE("NO_FRIENDS_DETECTED_DESCRIPTION")).setAction([=] {}));
			}
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SELECTED_FRIEND")).setId("home.network.friends.selected"_id).setAction([=](sub* sub) {
			auto data = friendReg->get(g_selectedFriend);
			if (data->m_is_joinable) {
				sub->addOption(regularOpt().setLeft(TRANSLATE("FRIEND_JOIN")).setAction([=] {
					base::g_thread_pool->push([&] {
						g_api_service->ridJoinViaSession(data->m_rockstar_id);
						});
					}));
			}
			sub->addOption(regularOpt().setLeft(TRANSLATE("FRIEND_COPY_RID")).setAction([=] {
				std::string rid = std::to_string(data->m_rockstar_id);
				ImGui::SetClipboardText(rid.c_str());
				MainNotification(ImGuiToastType_Success, 4200, TRANSLATE("NETWORK_FRIEND_LIST"), TRANSLATE("FRIENDLIST_COPY_RID"), rid, std::string(data->m_name));
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("FRIEND_COPY_NAME")).setAction([=] {
				ImGui::SetClipboardText(data->m_name);
				MainNotification(ImGuiToastType_Success, 4200, TRANSLATE("NETWORK_FRIEND_LIST"), TRANSLATE("FRIENDLIST_COPY_NAME"), std::string(data->m_name));
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_RID_TOOLS")).setId("home.network.rid_tools"_id).setAction([=](sub* sub) {
			sub->addOption(regularOpt().setLeft(TRANSLATE("JOIN")).setAction([=] {
				inputText(sub, TRANSLATE("JOIN"), [](std::string const& stringToJoin) {
					auto rid = isNumber(stringToJoin) ? stoi(stringToJoin) : g_api_service->nameToRid(stringToJoin);
					g_api_service->ridJoinViaSession(rid);
					});
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SESSION")).setId("home.network.session"_id).setAction([=](sub* sub) {
			sub->addOption(boolOpt().setLeft(TRANSLATE("SEAMLESS_JOINING")).setDesc(TRANSLATE("SEAMLESS_JOINING_DESCRIPTION")).setBool(features::seamlessJoinBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("FAST_JOINING")).setDesc(TRANSLATE("FAST_JOINING_DESCRIPTION")).setBool(features::fast_join));
			sub->addOption(boolOpt().setLeft(TRANSLATE("FORCE_SCRIPT_HOST")).setDesc(TRANSLATE("FORCE_SCRIPT_HOST_DESCRIPTION")).setBool(features::force_script_host));
			sub->addOption(boolOpt().setLeft(TRANSLATE("FORCE_SESSION_HOST")).setDesc(TRANSLATE("FORCE_SESSION_HOST_DESCRIPTION")).setBool(features::force_session_host));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_SESSION_STARTER")).setDesc(TRANSLATE("NETWORK_SESSION_STARTER_DESCRIPTION")).setId("home.network.session_starter"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("NETWORK_BLACKLIST")).setDesc(TRANSLATE("NETWORK_BLACKLIST_DESCRIPTION")).setId("home.network.kicked_players"_id));
			sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_UNSTUCK")).setAction([] {
				if (*globals(1574996).as<eTransitionState*>() == eTransitionState::TRANSITION_STATE_WAIT_HUD_EXIT) {
					*globals(1574996).as<eTransitionState*>() = eTransitionState::TRANSITION_STATE_LOOK_TO_JOIN_ANOTHER_SESSION_FM;
					return;
				}
				NETWORK::NETWORK_BAIL(0, 0, 16);
				NETWORK::NETWORK_BAIL_TRANSITION(0, 0, 16);
				MainNotification(ImGuiToastType_Warning, 4200, TRANSLATE("NETWORK_UNSTUCK"), TRANSLATE("FORCED_TO_BAIL_FROM_THE_SESSION"));
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_BLACKLIST")).setId("home.network.kicked_players"_id).setAction([=](sub* sub) {
			if (!kicked_Players.empty()) {
				for (const auto& player : kicked_Players) {
					sub->addOption(regularOpt().setLeft(player).setDesc(TRANSLATE("NETWORK_BLACKLIST_REMOVE")).setAction([=] {
						auto it = std::find(kicked_Players.begin(), kicked_Players.end(), player);
						if (it != kicked_Players.end()) {
							kicked_Players.erase(it);
							above_map(std::string(player) + std::string(TRANSLATE("NETWORK_BLACKLIST_REMOVED_PLAYER")));
						}
						}));
				}
			}
			else {
				sub->addOption(regularOpt().setLeft(TRANSLATE("NETWORK_BLACKLIST_EMPTY")).setAction([] {}));
			}
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("NETWORK_SESSION_STARTER")).setId("home.network.session_starter"_id).setAction([=](sub* sub) {
			for (const auto& session_type : sessions) {
				sub->addOption(regularOpt().setLeft(session_type.name).setDesc(session_type.name).setAction([=] {
					features::sessionJoin(session_type.id);
					}));
			}
			}));
		//Weapons
		g_dxUiManager.addSub(sub().setName(TRANSLATE("WEAPON_OPTIONS")).setId("home.weapons"_id).setAction([=](sub* sub) {
			sub->addOption(regularOpt().setLeft(TRANSLATE("WEAPONS_GIVE_WEAPONS")).setDesc(TRANSLATE("WEAPONS_GIVE_WEAPONS_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					for (auto Weapons : weaponHashes) {
						WEAPON::GIVE_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), Weapons, 9999, 0, 1);
					}
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("WEAPONS_REMOVE_WEAPONS")).setDesc(TRANSLATE("WEAPONS_REMOVE_WEAPONS_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					for (auto WeaponsToRemove : weaponHashes) {
						WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::PLAYER_PED_ID(), WeaponsToRemove);
					}
					});
				}));
			sub->addOption(boolOpt().setLeft(TRANSLATE("WEAPONS_INTERIOR_WEAPONS")).setDesc(TRANSLATE("WEAPONS_INTERIOR_WEAPONS_DESCRIPTION")).setBool(features::interior_weapon));
			sub->addOption(boolOpt().setLeft(TRANSLATE("INFINITE_AMMO")).setBool(features::infiniteAmmoBool));
			sub->addOption(vecBoolOpt<const char*, int>().setLeft(TRANSLATE("TRIGGER_BOT")).setArray(TriggerBotNames).setPos(features::TriggerBotType).setBool(features::TRIGGERBOT));
			sub->addOption(boolOpt().setLeft(TRANSLATE("RAPID_FIRE")).setBool(features::Rapidfirebool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("RAINBOW_WEAPONS")).setBool(features::RainbowWeaponsBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("CAGE_GUN")).setBool(features::Cagegunbool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("GRAVITY_GUN")).setBool(features::Gravitygunbool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("DELETE_GUN")).setBool(features::Deletegunbool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("TELEPORT_GUN")).setBool(features::Teleportgungun));
			sub->addOption(boolOpt().setLeft(TRANSLATE("CARTOON_GUN")).setBool(features::CartoonGunbool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("PTFX_GUN")).setBool(features::Coolptfxgun));
			sub->addOption(boolOpt().setLeft(TRANSLATE("STEAL_GUN")).setBool(features::Stealgunbool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("MINECRAFT_GUN")).setBool(features::Mcgunbool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("AIRSTRIKE_GUN")).setBool(features::Airstrikebool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("RP_GUN")).setBool(features::RpGunbool));
			}));
		//Vehicle
		g_dxUiManager.addSub(sub().setName(TRANSLATE("VEHICLE_OPTIONS")).setId("home.vehicle"_id).setAction([=](sub* sub) {
			sub->addOption(subOpt().setLeft(TRANSLATE("VEHICLE_SPAWNER")).setId("home.vehicle.spawner"_id));
			sub->addOption(boolOpt().setLeft(TRANSLATE("VEHICLE_GODMODE")).setDesc(TRANSLATE("VEHICLE_GODMODE_DESCRIPTION")).setBool(features::VehicleGodmode));
			sub->addOption(boolOpt().setLeft(TRANSLATE("HORN_BOOST")).setDesc(TRANSLATE("HORN_BOOST_DESCRIPTION")).setBool(features::hornboost));
			sub->addOption(boolOpt().setLeft(TRANSLATE("AUTO_REPAIR")).setDesc(TRANSLATE("AUTO_REPAIR_DESCRIPTION")).setBool(features::vehiclefixLoopBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SEAT_BELT")).setDesc(TRANSLATE("SEAT_BELT_DESCRIPTION")).setBool(features::Seatbeltbool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("MANUAL_HANDBREAK")).setDesc(TRANSLATE("MANUAL_HANDBREAK_DESCRIPTION")).setBool(features::HandBreakBool));
			sub->addOption(vecBoolOpt<const char*, int>().setLeft(TRANSLATE("HAMILION_VEHICLE")).setDesc(TRANSLATE("HAMILION_VEHICLE_DESCRIPTION")).setArray(RainBowVehicleNames).setPos(RainbowSelect).setBool(features::RainbowVehicleBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SCORCH_VEHICLE")).setDesc(TRANSLATE("SCORCH_VEHICLE_DESCRIPTION")).setBool(features::Scrutchbool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("GROUND_VEHICLE")).setDesc(TRANSLATE("GROUND_VEHICLE_DESCRIPTION")).setBool(features::StickyBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("DRIFT_MODE")).setDesc(TRANSLATE("DRIFT_MODE_DESCRIPTION")).setBool(features::DriftBool));
			sub->addOption(boolOpt().setLeft(TRANSLATE("MATRIX_PLATE")).setDesc(TRANSLATE("MATRIX_PLATE_DESCRIPTION")).setBool(features::MatrixPlates));
			sub->addOption(regularOpt().setLeft(TRANSLATE("UPGRADE_VEHICLE")).setAction([=] {
				g_fiber_pool->queue_job([&] {
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
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("REPAIR_VEHICLE")).setAction([=] {
				g_fiber_pool->queue_job([&] {
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
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("DELETE_VEHICLE")).setAction([=] {
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
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("VEHICLE_SPAWNER")).setId("home.vehicle.spawner"_id).setAction([=](sub* sub) {
			sub->addOption(regularOpt().setLeft(TRANSLATE("VEHICLE_SPAWN_BY_NAME")).setAction([=] {
				inputText(sub, TRANSLATE("VEHICLE_SPAWN_VEHICLE"), [](std::string const& SpawnCar) {
					g_fiber_pool->queue_job([=] {
						SpawnVehicle(rage::joaat(SpawnCar));
						});
					});
				}));
			for (uint8_t i{}; i != 23; ++i) {
				sub->addOption(subOpt().setLeft(util::vehicle::g_modelClasses[i]).setId("home.vehicle.spawner.selected"_id).setAction([=] {
					util::vehicle::g_selectedVehicleClass = i;
					g_dxUiManager.getSubData("home.vehicle.spawner.selected"_id)->setName(util::vehicle::g_modelClasses[i]).setSelectedOpt(0);
					}));
			}
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("INVALID_VEHICLE_CLASS_SUBMENU")).setId("home.vehicle.spawner.selected"_id).setAction([=](sub* sub) {
			auto vehicles{ util::vehicle::g_models.get(util::vehicle::g_selectedVehicleClass) };
			for (size_t i{}; i != vehicles.size(); ++i) {
				auto& veh{ vehicles[i] };
				sub->addOption(subOpt().setLeft(veh.m_finalLabel).setId("home.vehicle.spawner.selected.selected"_id).setAction([=] {
					util::vehicle::g_selectedVehicleFromClass = i;
					g_dxUiManager.getSubData("home.vehicle.spawner.selected.selected"_id)->setName(veh.m_finalLabel).setSelectedOpt(0);
					}));
			}
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("INVALID_VEHICLE_SUBMENU")).setId("home.vehicle.spawner.selected.selected"_id).setAction([=](sub* sub) {
			auto vehicles{ util::vehicle::g_models.get(util::vehicle::g_selectedVehicleClass) };
			auto const& veh{ vehicles[util::vehicle::g_selectedVehicleFromClass] };
			sub->addOption(boolOpt().setLeft(TRANSLATE("MAX_VEHICLE_SPAWN")).setBool(MaxVehiclespawn));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SET_IN_VEHICLE_SPAWNED")).setBool(Sitinvehicle));
			sub->addOption(boolOpt().setLeft(TRANSLATE("SPAWN_WITH_EFFECT")).setBool(bSpawnWithEffect));
			sub->addOption(regularOpt().setLeft(TRANSLATE("VEHICLE_SPAWN_VEHICLE")).setAction([=] {
				g_fiber_pool->queue_job([=] {
					SpawnVehicle(veh.m_modelInfo->m_hash);
					});
				}));
			}));
		//Teleport
		g_dxUiManager.addSub(sub().setName(TRANSLATE("TELEPORT_OPTIONS")).setId("home.teleport"_id).setAction([=](sub* sub) {
			//sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("TELEPORT_ANIMATION")).setArray(TeleportAnimationNames).setPos(TeleportState));
			sub->addOption(regularOpt().setLeft(TRANSLATE("TELEPORT_TO_WAYPOINT")).setDesc(TRANSLATE("TELEPORT_TO_WAYPOINT_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					to_waypoint();
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("TELEPORT_TO_OBJECTIVE")).setDesc(TRANSLATE("TELEPORT_TO_OBJECTIVE_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					to_objective();
					});
				}));
			}));
		//Protections
		g_dxUiManager.addSub(sub().setName(TRANSLATE("PROTECTION_OPTIONS")).setId("home.protections"_id).setAction([=](sub* sub) {
			sub->addOption(subOpt().setLeft(TRANSLATE("PROTECTIONS_SCRIPT_EVENTS")).setId("home.protections.se"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("PROTECTIONS_NETWORK_EVENTS")).setId("home.protections.ne"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("PROTECTIONS_DETECTIONS")).setId("home.protections.de"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("RESPONCES")).setId("home.protections.re"_id));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("PROTECTIONS_REPORTS")).setDesc(TRANSLATE("PROTECTIONS_REPORTS_DESCRIPTION")).setArray(m_toggleTypes).setPos(protections::m_blockReports.toggleType));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("PROTECTIONS_SCRIPT_EVENTS")).setId("home.protections.se"_id).setAction([=](sub* sub) {
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("KICKS")).setArray(m_toggleTypes).setPos(protections::g_blockKick.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("CRASHES")).setArray(m_toggleTypes).setPos(protections::g_blockCrashes.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("BOUNTY")).setArray(m_toggleTypes).setPos(protections::g_blockBounty.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("CEO_KICK")).setArray(m_toggleTypes).setPos(protections::g_blockCEOKick.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("CEO_MONEY")).setArray(m_toggleTypes).setPos(protections::g_blockCEOMoney.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("CLEAR_WANTED_LVL")).setArray(m_toggleTypes).setPos(protections::g_blockClearWanted.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("FAKE_NOTIFY")).setArray(m_toggleTypes).setPos(protections::g_blockFakeDeposit.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("FORCE_TO_MISSION")).setArray(m_toggleTypes).setPos(protections::g_blockForceMission.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("GIVE_COLLECTIBLES")).setArray(m_toggleTypes).setPos(protections::g_blockGiveCollectible.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("GTA_BANNER")).setArray(m_toggleTypes).setPos(protections::g_blockGTABanner.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("MC_TELEPORT")).setArray(m_toggleTypes).setPos(protections::g_blockMCTeleport.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("PERSONAL_VEHICLE_DESTROY")).setArray(m_toggleTypes).setPos(protections::g_blockPersonalVehDestory.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("REMOTE_OFF_RADAR")).setArray(m_toggleTypes).setPos(protections::g_blockRemoteOffRadar.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("ROTATE_CAMERA")).setArray(m_toggleTypes).setPos(protections::g_blockRotateCam.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("SEND_TO_CUTSCENE")).setArray(m_toggleTypes).setPos(protections::g_blockSendToCutscene.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("SEND_TO_LOCATION")).setArray(m_toggleTypes).setPos(protections::g_blockSendToLocation.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("TSE_FREEZE")).setArray(m_toggleTypes).setPos(protections::g_blockTSEFreeze.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("SPECTATE")).setArray(m_toggleTypes).setPos(protections::g_blockSpectate.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("FORCE_TELEPORT")).setArray(m_toggleTypes).setPos(protections::g_blockForceTeleport.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("TRANSACTION_ERROR")).setArray(m_toggleTypes).setPos(protections::g_blockTransactionErr.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("VEHICLE_KICK")).setArray(m_toggleTypes).setPos(protections::g_blockVehKick.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("TELEPORT_TO_WAREHOUSE")).setArray(m_toggleTypes).setPos(protections::g_blockTeleportToWarehouse.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("START_ACTIVITY")).setArray(m_toggleTypes).setPos(protections::g_blockStartActivity.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("INTERIOR_CONTROL")).setArray(m_toggleTypes).setPos(protections::g_blockInteriorControl.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("SEND_TO_INTERIOR")).setArray(m_toggleTypes).setPos(protections::g_blockSendToInterior.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("KICK_FROM_INTERIOR")).setArray(m_toggleTypes).setPos(protections::g_blockKickFromInterior.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("NULL_FUNCTION_KICK")).setArray(m_toggleTypes).setPos(protections::g_blockNullFunctionKick.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("TRIGGER_CEO_RAID")).setArray(m_toggleTypes).setPos(protections::g_blockTriggerCeoRaid.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("START_SCRIPT_PROCEED")).setArray(m_toggleTypes).setPos(protections::g_blockProceedScript.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("SOUND_SPAM")).setArray(m_toggleTypes).setPos(protections::g_blockSoundSpam.toggleType));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("PROTECTIONS_NETWORK_EVENTS")).setId("home.protections.ne"_id).setAction([=](sub* sub) {
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("FREEZE")).setArray(m_toggleTypes).setPos(protections::g_blockNetworkFreeze.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("REQUEST_CONTROL")).setArray(m_toggleTypes).setPos(protections::m_blockRequestControl.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("RAGDOLL")).setArray(m_toggleTypes).setPos(protections::g_blockRagdollRequest.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("VOTE_KICK")).setArray(m_toggleTypes).setPos(protections::g_blockVoteKick.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("PTFX")).setArray(m_toggleTypes).setPos(protections::g_blockNetworkPTFX.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("ALTER_WANTED_LVL")).setArray(m_toggleTypes).setPos(protections::g_blockClearWanted.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("REMOVE_WEAPONS")).setArray(m_toggleTypes).setPos(protections::g_blockRemoveWeapons.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("REMOVE_WEAPON")).setArray(m_toggleTypes).setPos(protections::g_blockRemoveWeapon.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("GIVE_WEAPONS")).setArray(m_toggleTypes).setPos(protections::m_blockGiveWeapon.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("GIVE_CONTROL")).setArray(m_toggleTypes).setPos(protections::g_blockGiveControl.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("BLAME_EXPLOSION")).setArray(m_toggleTypes).setPos(protections::g_blockBlameExplosion.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("EXPLOSION")).setArray(m_toggleTypes).setPos(protections::g_blockExplosion.toggleType));
			sub->addOption(vecOpt<const char*, int>().setLeft(TRANSLATE("WEAPON_DAMAGE")).setArray(m_toggleTypes).setPos(protections::g_blockDamageKarma.toggleType));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("PROTECTIONS_DETECTIONS")).setId("home.protections.de"_id).setAction([=](sub* sub) {
			sub->addOption(boolOpt().setLeft(TRANSLATE("HIDE_GODMODE")).setBool(features::spoof_hide_godmode));
			sub->addOption(boolOpt().setLeft(TRANSLATE("HIDE_SPECTATE")).setBool(features::spoof_hide_spectate));
			sub->addOption(boolOpt().setLeft(TRANSLATE("HIDE_FROM_PLAYER_LIST")).setBool(features::hide_from_player_list));
			sub->addOption(boolOpt().setLeft(TRANSLATE("ADMIN_CHECK")).setBool(features::admin_check));
			sub->addOption(boolOpt().setLeft(TRANSLATE("KICK_ADMINS")).setBool(features::kick_admins));
			sub->addOption(boolOpt().setLeft(TRANSLATE("ADVERTISERS_KICK")).setBool(features::kick_advertisers));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("RESPONCES")).setId("home.protections.re"_id).setAction([=](sub* sub) {
			//sub->addOption(boolOpt().setLeft(TRANSLATE("LOG_CHAT_MESSAGES")).setBool(features::log_chat_messages));
			sub->addOption(boolOpt().setLeft(TRANSLATE("REJOIN_FROM_KICKED_SESSION")).setBool(features::rejoin_kicked_session));
			}));
		//Recovery
		g_dxUiManager.addSub(sub().setName(TRANSLATE("RECOVERY_OPTIONS")).setId("home.recovery"_id).setAction([=](sub* sub) {
			sub->addOption(regularOpt().setLeft(TRANSLATE("RECOVERY_APPLY_RANK")).setAction([=] {
				inputText(sub, TRANSLATE("RECOVERY_APPLY_RANK"), [](std::string const& rankStr) {
					int rank = stoi(rankStr);
					g_fiber_pool->queue_job([=] {
						setRank(rank);
						});
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("RECOVERY_CLEAR_REPORTS")).setAction([=] {
				g_fiber_pool->queue_job([] {
					ClearAllReports();
					});
				}));
			}));
		//Misc
		g_dxUiManager.addSub(sub().setName(TRANSLATE("MISC_OPTIONS")).setId("home.misc"_id).setAction([=](sub* sub) {
			sub->addOption(regularOpt().setLeft(TRANSLATE("MISC_SKIP_CUTSCENE")).setDesc(TRANSLATE("MISC_SKIP_CUTSCENE_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([] {
					if (CUTSCENE::IS_CUTSCENE_ACTIVE() || CUTSCENE::IS_CUTSCENE_PLAYING()) {
						CUTSCENE::STOP_CUTSCENE(TRUE);
						CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
						CUTSCENE::REMOVE_CUTSCENE();
					}
					});
				}));
			}));
		//Settings
		g_dxUiManager.addSub(sub().setName(TRANSLATE("SETTINGS_OPTIONS")).setId("home.settings"_id).setAction([=](sub* sub) {
			sub->addOption(subOpt().setLeft(TRANSLATE("SETTINGS_HEADER_OPTIONS")).setId("home.settings.header"_id));
			//sub->addOption(subOpt().setLeft(TRANSLATE("SETTINGS_INPUT_OPTIONS")).setId("home.settings.input"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("SETTINGS_TRASLATIONS")).setId("home.settings.translations"_id));
			sub->addOption(subOpt().setLeft(TRANSLATE("GAME")).setId("home.settings.game"_id));
			sub->addOption(boolOpt().setLeft("Move Menu").setBool(features::move_withmousebool));
			sub->addOption(numOpt<float>().setLeft(TRANSLATE("XPOS")).setDesc(TRANSLATE("XPOS_DESCRIPTION")).setMinMax(0, g_dxUiManager.getRes().x).setStep(5.f).setNum(g_dxUiManager.m_pos.x));
			sub->addOption(numOpt<float>().setLeft(TRANSLATE("YPOS")).setDesc(TRANSLATE("YPOS_DESCRIPTION")).setMinMax(0, g_dxUiManager.getRes().y).setStep(5.f).setNum(g_dxUiManager.m_pos.y));
			sub->addOption(numOpt<float>().setLeft(TRANSLATE("MENU_WIDTH")).setDesc(TRANSLATE("MENU_WIDTH_DESCRIPTION")).setMinMax(350.f, 520.f).setStep(5.f).setNum(g_dxUiManager.m_width));
			sub->addOption(numOpt<float>().setLeft(TRANSLATE("MENU_SCALE")).setDesc(TRANSLATE("MENU_SCALE_DESCRIPTION")).setMinMax(0.5f, 5.f).setStep(0.1f).setNum(g_dxUiManager.m_scale));
			sub->addOption(numOpt<float>().setLeft(TRANSLATE("MENU_SCROLL_SPEED")).setDesc(TRANSLATE("MENU_SCROLL_SPEED_DESCRIPTION")).setMinMax(0.f, 1.f).setStep(0.001f).setNum(g_dxUiManager.m_scrollSpeed));
			sub->addOption(boolOpt().setLeft(TRANSLATE("INFO_OVERLAY")).setDesc(TRANSLATE("INFO_OVERLAY_DESCRIPTION")).setBool(features::InfoOverlay));
			sub->addOption(boolOpt().setLeft(TRANSLATE("CHANGE_UI_MODE")).setDesc(TRANSLATE("CHANGE_UI_MODE_DESCRIPTION")).setBool(g_mainScript.m_listUi));
			sub->addOption(regularOpt().setLeft(TRANSLATE("MENU_UNLOAD")).setDesc(TRANSLATE("MENU_UNLOAD_DESCRIPTION")).setAction([=] {
				g_running = false;
				}));
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("SETTINGS_HEADER_OPTIONS")).setId("home.settings.header"_id).setAction([=](sub* sub) {
			const fs::path headerPath = "C:/LightningFiles/Headers"; // Use forward slashes for better portability
			if (fs::exists(headerPath)) {
				for (const auto& dirEntry : fs::directory_iterator(headerPath)) {
					if (dirEntry.is_regular_file() && containPictExt(dirEntry.path())) { // Check file extension directly
						sub->addOption(regularOpt().setLeft(TRANSLATE("LOAD_HEADER") + dirEntry.path().filename().string()).setAction([path = dirEntry.path()] {
							g_thread_pool->push([path] {
								g_dxUiManager.m_hasHeaderLoaded = false;
								g_dxUiManager.m_headerloading = true;
								g_renderer->m_header.clear();
								g_renderer->m_headerFrame = 0;
								if (path.extension() == ".gif") {
									g_renderer->m_header = g_renderer->createGifTexture(path);
								}
								else {
									g_renderer->m_header.try_emplace(0, 0, g_renderer->createTexture(path));
								}
								g_dxUiManager.m_headerloading = false;
								g_dxUiManager.m_hasHeaderLoaded = true;
								});
							}));
					}
				}
			}
			else
			{
				sub->addOption(regularOpt().setLeft(TRANSLATE("NO_HEADER_FOUND")).setAction([=]() {
					}));
			}
			}));
		/*g_dxUiManager.addSub(sub().setName(TRANSLATE("SETTINGS_INPUT_OPTIONS")).setId("home.settings.input"_id).setAction([=](sub* sub) {
			sub->addOption(numOpt<size_t>().setLeft("Open Delay").setMinMax(0, 50).setStep(5).setNum(g_dxUiManager.m_openDelay));
			sub->addOption(numOpt<size_t>().setLeft("Back Delay").setMinMax(0, 50).setStep(5).setNum(g_dxUiManager.m_backDelay));
			sub->addOption(numOpt<size_t>().setLeft("Enter Delay").setMinMax(0, 50).setStep(5).setNum(g_dxUiManager.m_enterDelay));
			sub->addOption(numOpt<size_t>().setLeft("Vertical Delay").setMinMax(0, 50).setStep(5).setNum(g_dxUiManager.m_verticalDelay));
			sub->addOption(numOpt<size_t>().setLeft("Horizontal Delay").setMinMax(0, 50).setStep(5).setNum(g_dxUiManager.m_horizontalDelay));
			}));*/
		g_dxUiManager.addSub(sub().setName(TRANSLATE("SETTINGS_TRASLATIONS")).setId("home.settings.translations"_id).setAction([=](sub* sub) {

			const fs::path headerPath = RealTranslationPath; // Use forward slashes for better portability
			if (fs::exists(headerPath)) {
				for (const auto& dirEntry : fs::directory_iterator(headerPath)) {
					if (dirEntry.is_regular_file() && containJson(dirEntry.path())) {
						std::string filenameWithoutExtension = dirEntry.path().stem().string();
						sub->addOption(regularOpt().setLeft(TRANSLATE("LOAD_HEADER") + filenameWithoutExtension).setAction([=]() {
							g_thread_pool->push([filenameWithoutExtension] {
								g_TranslationManager->LoadTranslations(filenameWithoutExtension);
								});
							}));
					}
				}
			}
			else
			{
				sub->addOption(regularOpt().setLeft(TRANSLATE("NO_TRANSLATIONS_FOUND")).setAction([=]() {	
				}));
			}
			}));
		g_dxUiManager.addSub(sub().setName(TRANSLATE("GAME")).setId("home.settings.game"_id).setAction([=](sub* sub) {
			sub->addOption(regularOpt().setLeft(TRANSLATE("FORCE_QUIT")).setDesc(TRANSLATE("FORCE_QUIT_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([&] {
					if (*g_pointers->m_isSessionActive) {
						NETWORK::SHUTDOWN_AND_LOAD_MOST_RECENT_SAVE();
					}
					else {
						MainNotification(ImGuiToastType_Error, 5000, TRANSLATE("ERROR"), TRANSLATE("YOU_MUST_BE_ONLINE_TO_USE_THIS_OPTION"));
					}
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("RESTART_GAME")).setDesc(TRANSLATE("RESTART_GAME_DESCRIPTION")).setAction([=] {
				g_fiber_pool->queue_job([&] {
					MISC::RESTART_GAME();
					});
				}));
			sub->addOption(regularOpt().setLeft(TRANSLATE("EXIT_GAME")).setDesc(TRANSLATE("EXIT_GAME_DESCRIPTION")).setAction([=] {
				TerminateProcess(GetCurrentProcess(), 0);
				}));
			}));
	}
}