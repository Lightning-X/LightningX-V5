#pragma once
#include "Common.hpp"
#include "Pointers.hpp"
#include "Natives.hpp"
#include "ScriptGlobal.hpp"
#include "Enums.hpp"
namespace base::features {
	//Self
	inline bool godModeBool = false;
	inline bool invisibilityBool = false;
	inline bool locallyVisbleBool = true;
	inline bool neverWantedBool = false;
	inline bool force_wanted_level = false;
	inline int wanted_level = 0;
	inline bool GracefulLanding = false;
	inline bool noRagdollBool = false;
	inline bool NoClipBool = false;
	inline bool Freecambool = false;
	inline bool superRunBool = false;
	inline bool offRadarBool = false;
	inline float fastSwimSpeed = 1.f;
	inline bool fastSwimBool = false;
	inline float fastRunSpeed = 1.f;
	inline bool fastRunBool = false;
	inline bool swimInAirBool = false;
	inline bool FastRespawnBool = false;
	inline bool enableSuperJump = false;
	inline bool healthRegenbool = false;
	inline float healthRegenSpeed = 1.0f;
	inline bool unlimitedOxygenBool = false;
	inline bool keepSelfCleanBool = false;
	inline bool mobileRadioBool = false;
	inline bool selfNoCollision = false;
	inline bool selfNoWaterCollision = false;
	inline bool TRIGGERBOT = false;
	inline bool Gravitygunbool = false;
	inline bool Rapidfirebool = false;
	inline bool Coolptfxgun = false;
	inline bool infiniteAmmoBool = false;
	inline bool Stealgunbool = false;
	inline bool Mcgunbool = false;
	inline bool Airstrikebool = false;
	inline bool RainbowWeaponsBool = false;
	inline bool RpGunbool = false;
	inline bool Cagegunbool = false;
	inline bool Teleportgungun = false;
	inline bool Deletegunbool = false;
	inline bool CartoonGunbool = false;
	inline bool VehicleGodmode = false;
	inline bool hornboost = false;
	inline bool vehiclefixLoopBool = false;
	inline bool RainbowVehicleBool = false;
	inline bool SpectateBool = false;
	inline int redcolorInt = 255;
	inline int greencolorInt = 0;
	inline int bluecolorInt = 0;
	inline 	bool Scrutchbool = false;
	inline bool StickyBool = false;
	inline bool DriftBool = false;
	inline bool MatrixPlates = false;
	inline bool HandBreakBool = false;
	inline int TriggerBotType = 0;
	inline bool Seatbeltbool = false;
	//Protections
	inline bool kick_admins = false;
	inline bool admin_check = true;
	inline bool kick_advertisers = true;
	inline bool log_chat_messages = false;
	inline bool blockKickedPlayers = true;
	inline bool desync_kick_protection = true;
	//Detections
	inline bool explosion_karma = false;
	inline 	bool damage_karma = false;
	inline bool rejoin_kicked_session = false;
	inline bool block_explotions = false;
	//detor Hook
	inline bool spoof_hide_godmode = true;
	inline bool spoof_hide_spectate = true;
	inline bool hide_from_player_list = false;
	//Native hooks
	inline 	bool unhide_players_from_player_list = false;
	inline 	bool seamlessJoinBool = false;
	inline bool dev_dlc = false;
	inline bool m_driver_performing_task;
	inline bool decloak_players = false;
	inline 	bool interior_weapon = false;
	inline 	bool force_script_host = false;
	inline bool fast_join = false;
	inline 	bool force_session_host = false;
	inline bool DisableAllActions = false;
	inline bool move_withmousebool = false;
	inline bool InfoOverlay = true;
	//VOIDS
	extern void godMode();
	extern void invisibility();
	extern void noRagdoll();
	extern void neverWanted();
	extern void offRadar();
	extern void NoClip();
	extern void self_free_cam();
	extern void fastSwim();
	extern void superRun();
	extern void fastRun();
	extern void swimInAir();
	extern void selfFastRespawn();
	extern void healthRegen();
	extern void unlimitedOxygen();
	extern void selfClean();
	extern void mobileRadio();
	extern void noCollision();
	extern void noWaterCollision();
	extern void seamlessJoin();
	extern void vehGodmode();
	extern void SeatBelt();
	extern void vehicle_horn_boost();
	extern void fixLoop();
	extern void rainbowvehicle();
	extern void Scrutch();
	extern void Stickymicky();
	extern void Driftmode();
	extern void MatrixPlate();
	extern void Handbreak();
	extern void hudTransitionState();
	extern void system_desync_kick_protection();
	extern bool force_host(std::uint32_t hash);
	extern void applyChosenSkin(const Hash model);
	extern void spectatePlayer();
	extern bool sessionJoin(eSessionType session);
	extern void forceSessionHost();
	extern void V_DisableAllActions();
	extern void doAnimation(const char* anim, const char* animid);
	extern void Triggerbot();
	extern void Gravitygun();
	extern void PTFXgun();
	extern void Stealgun();
	extern void MCgun();
	extern void Airstrikegunwep();
	extern void RGBWep();
	extern void RPGun();
	extern void Cagegun();
	extern void Teleportgun();
	extern void Deletegun();
	extern void Cartoongun();
	//Weapons
	//Tick
	extern void tick();
}