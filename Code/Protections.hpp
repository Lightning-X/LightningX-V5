#pragma once
#include "Hooking.hpp"
#include "Common.hpp"
#include "Natives.hpp"
#include "Enums.hpp"
namespace base {
	inline std::vector<const char*> m_toggleTypes = {
	"Notify and Block",
	"None",
	"Notify",
	"Block",
	"Notify, Block and Karma"
	};
	enum navigationmenucase {
		Arrows,
		Numpad
	};
	enum eToggleTypes {
		tog_notifyAndBlock,
		tog_none,
		tog_notify,
		tog_block,
		tog_notifyBlockandKarma
	};
	enum fToggleTypes {
		tog1,
		tog2,
		tog_1and2
	};
	struct toggleHandler { bool toggle; int toggleType; };
	struct eventHandler { std::string name; eScriptEvents hash; toggleHandler& toggle; };
	struct modelHandler { std::string name; Hash hash; toggleHandler& toggle; };
	namespace protections {
		inline toggleHandler g_blockBounty{};
		inline toggleHandler g_blockCEOKick{};
		inline toggleHandler g_blockCEOBan{};
		inline toggleHandler g_blockCEOMoney{};
		inline toggleHandler g_blockSetBounty{};
		inline toggleHandler g_blockClearWanted{};
		inline toggleHandler g_blockFakeDeposit{};
		inline toggleHandler g_blockForceMission{};
		inline toggleHandler g_blockGTABanner{};
		inline toggleHandler g_blockNetBail{};
		inline toggleHandler g_blockPersonalVehDestory{};
		inline toggleHandler g_blockRemoteOffRadar{};
		inline toggleHandler g_blockRotateCam{};
		inline toggleHandler g_blockRotateCam2{};
		inline toggleHandler g_blockSendToCutscene{};
		inline toggleHandler g_blockSendToIsland{};
		inline toggleHandler g_blockSoundSpam{};
		inline toggleHandler g_blockSpectate{};
		inline toggleHandler g_blockTeleport{};
		inline toggleHandler g_blockForceTeleport{};
		inline toggleHandler g_blockForceCamera{};
		inline toggleHandler g_blockTSECommand{};
		inline toggleHandler g_blockNotifications{};
		inline toggleHandler g_blockStartActivity{};
		inline toggleHandler g_blockTriggerCeoRaid{};
		inline toggleHandler g_blockMarkPlayerAsBeast{};
		inline toggleHandler g_blockKickFromInterior{};
		inline toggleHandler g_blockInteriorControl{};
		inline toggleHandler g_blockMCTeleport{};
		inline toggleHandler g_blockStartScript{};
		inline toggleHandler g_blockTeleportToWarehouse{};
		inline toggleHandler g_blockSendToLocation{};
		inline toggleHandler g_blockGiveCollectible{};
		inline toggleHandler g_blockchangemcrole{};
		inline toggleHandler g_blockTransactionErr{};
		inline toggleHandler g_blockSms{};
		inline toggleHandler g_blockModdedEvent{};
		inline toggleHandler g_blockProceedScript{};
		inline toggleHandler g_blockTSEFreeze{};
		inline toggleHandler g_blockNullFunctionKick{};
		inline toggleHandler g_blockSendToInterior{};
		inline toggleHandler g_blockVehKick{};
		inline toggleHandler g_blockKick{};
		inline toggleHandler g_blockStoryKick2{};
		inline toggleHandler g_blockStoryKick3{};
		inline toggleHandler g_blockCrashes{};
		inline toggleHandler g_blockCrash2{};
		inline toggleHandler g_blockNetworkPTFX{};
		//NETWORK
		inline toggleHandler g_blockVoteKick{};
		inline toggleHandler g_blockNetworkFreeze{};
		inline toggleHandler g_blockRagdollRequest{};
		inline toggleHandler g_blockRemoveWeapons{};
		inline toggleHandler g_blockRemoveWeapon{};
		//missing give weapons, will add later
		inline toggleHandler g_blockGiveControl{};
		inline toggleHandler g_blockExplosion{};
		inline toggleHandler g_blockDamageKarma{};
		inline toggleHandler g_blockBlameExplosion{};
		//Report Events
		inline toggleHandler m_blockReports{};
		inline std::vector<modelHandler> m_reportHashes = {
			{ "Bad Crew Status", ("MPPLY_BAD_CREW_STATUS"_joaat), m_blockReports },
			{ "Bad Crew Motto", ("MPPLY_BAD_CREW_MOTTO"_joaat), m_blockReports },
			{ "Bad Crew Name", ("MPPLY_BAD_CREW_NAME"_joaat), m_blockReports },
			{ "Bad Crew Emblem", ("MPPLY_BAD_CREW_EMBLEM"_joaat), m_blockReports },
			{ "Annoying Me In VC", ("MPPLY_VC_ANNOYINGME"_joaat), m_blockReports },
			{ "Annoying Me In TC", ("MPPLY_TC_ANNOYINGME"_joaat), m_blockReports },
			{ "VC Hate", ("MPPLY_VC_HATE"_joaat), m_blockReports },
			{ "TC Hate", ("MPPLY_TC_HATE"_joaat), m_blockReports },
			{ "Exploits", ("MPPLY_EXPLOITS"_joaat), m_blockReports },
			{ "Game Exploits", ("MPPLY_GAME_EXPLOITS"_joaat), m_blockReports },
			{ "Offsensive Playermade Title", ("MPPLY_PLAYERMADE_TITLE"_joaat), m_blockReports },
			{ "Offsensive Playermade Description", ("MPPLY_PLAYERMADE_DESC"_joaat), m_blockReports }
		};
		//Objects
		inline toggleHandler m_blockSlodCrash{};
		//Crashes
		inline toggleHandler m_blockVehicleTaskCrash{};
		inline toggleHandler m_blockInvalidVersionCrash{};
		inline toggleHandler m_blockSyncMismatchCrash{};
		inline toggleHandler m_blockOutOfRangeSyncCrash{};
		//Network events
		inline toggleHandler m_blockExplosions{};
		inline toggleHandler m_blockFreeze{};
		inline toggleHandler m_blockRequestControl{};
		inline toggleHandler m_blockGiveWeapon{};
		inline toggleHandler m_blockRemoveWeapon{};
		inline toggleHandler m_blockRemoveAllWeapons{};
		inline toggleHandler m_blockVoteKicks{};
		inline toggleHandler m_blockAlterWantedLvl{};
		inline toggleHandler m_blockClearArea{};
	}
}