#pragma once
#include "Common.hpp"
#include "Translations.hpp"
namespace base
{
	inline const std::vector<std::uint32_t> weaponHashes = {
		0x05A96BA4, //weaponcombatshotgun
		0x05FC3C11, //weaponsniperrifle
		0x060EC506, //weaponfireextinguisher
		0x0781FE4A, //weaponcompactlauncher
		0x0787F0BB, //weaponsnowball
		0x083839C4, //weaponvintagepistol
		0x0A3D4D34, //weaponcombatpdw
		0x0A914799, //weaponheavysniper_mk2
		0x0C472FE2, //weaponheavysniper
		0x12E82D3D, //weaponautoshotgun
		0x13532244, //weaponmicrosmg
		0x184140A1, //weaponfertilizercan
		0x19044EE0, //weaponwrench
		0x1B06D571, //weaponpistol
		0x1BC4FDB9, //weaponpistolxm3
		0x1D073A89, //weaponpumpshotgun
		0x22D8FE39, //weaponappistol
		0x23C9F95C, //weaponball
		0x24B17070, //weaponmolotov
		0x2B5EF5EC, //weaponceramicpistol
		0x2BE6766B, //weaponsmg
		0x2C3731D9, //weaponstickybomb
		0x34A67B97, //weaponpetrolcan
		0x3656C8C1, //weaponstungun
		0x3813FC08, //weaponstone_hatchet
		0x394F415C, //weaponassaultrifle_mk2
		0x3AABBBAA, //weaponheavyshotgun
		0x42BF8A85, //weaponminigun
		0x440E4788, //weapongolfclub
		0x45CD9CF3, //weaponstungun_mp
		0x476BF155, //weaponraycarbine
		0x47757124, //weaponflaregun
		0x497FACC3, //weaponflare
		0x4DD2DC56, //weapongrenadelauncher_smoke
		0x4E875F73, //weaponhammer
		0x555AF99A, //weaponpumpshotgun_mk2
		0x57A4368C, //weapongadgetpistol
		0x5EF9FEC4, //weaponcombatpistol
		0x61012683, //weapongusenberg
		0x624FE830, //weaponcompactrifle
		0x63AB0442, //weaponhominglauncher
		0x6589186A, //weaponcandycane
		0x678B81B1, //weaponnightstick
		0x6A6C02E0, //weaponmarksmanrifle_mk2
		0x6D544C99, //weaponrailgun
		0x6E7DDDEC, //weaponprecisionrifle
		0x7846A318, //weaponsawnoffshotgun
		0x78A97CD0, //weaponsmg_mk2
		0x7F229F94, //weaponbullpuprifle
		0x7F7497E5, //weaponfirework
		0x7FD62962, //weaponcombatmg
		0x83BF0278, //weaponcarbinerifle
		0x84BD7BFD, //weaponcrowbar
		0x84D6FAFD, //weaponbullpuprifle_mk2
		0x88374054, //weaponsnspistol_mk2
		0x8BB05FD7, //weaponflashlight
		0x917F6C8C, //weaponnavyrevolver
		0x92A27487, //weapondagger
		0x93E220BD, //weapongrenade
		0x94117305, //weaponpoolcue
		0x958A4A8F, //weaponbat
		0x969C3D67, //weaponspecialcarbine_mk2
		0x97EA20B8, //weapondoubleaction
		0x99AEEB3B, //weaponpistol50
		0x99B507EA, //weaponknife
		0x9D07F764, //weaponmg
		0x9D1F17E6, //weaponmilitaryrifle
		0x9D61E50F, //weaponbullpupshotgun
		0xA0973D5E, //weaponbzgas
		0xA284510B, //weapongrenadelauncher
		0xA89CB99E, //weaponmusket
		0xAB564B93, //weaponproxmine
		0xAF113F99, //weaponadvancedrifle
		0xAF3696A1, //weaponraypistol
		0xB1CA77B1, //weaponrpg
		0xB62D1F67, //weaponrayminigun
		0xBA45E8B8, //weaponpipebomb
		0xBA536372, //weaponhazardcan
		0xBD248B55, //weaponminismg
		0xBFD21232, //weaponsnspistol
		0xBFE256D4, //weaponpistol_mk2
		0xBFEFFF6D, //weaponassaultrifle
		0xC0A3098D, //weaponspecialcarbine
		0xC1B3C3D1, //weaponrevolver
		0xC734385A, //weaponmarksmanrifle
		0xC78D71B4, //weaponheavyrifle
		0xCB96392F, //weaponrevolver_mk2
		0xCD274149, //weaponbattleaxe
		0xD1D5F52B, //weapontacticalrifle
		0xD205520E, //weaponheavypistol
		0xD8DF3C3C, //weaponknuckle
		0xDB1AA450, //weaponmachinepistol
		0xDB26713A, //weaponemplauncher
		0xDBA2E809, //weaponmetaldetector
		0xDBBD7280, //weaponcombatmg_mk2
		0xDC4DB296, //weaponmarksmanpistol
		0xDD5DF8D9, //weaponmachete
		0xDFE37640, //weaponswitchblade
		0xE284C527, //weaponassaultshotgun
		0xEF951FBB, //weapondbshotgun
		0xEFE7E2DF, //weaponassaultsmg
		0xF7F1E25E, //weaponacidpackage
		0xF9DCBF2D, //weaponhatchet
		0xF9E6AA4B, //weaponbottle
		0xFAD1F1C9, //weaponcarbinerifle_mk2
		0xFDBC8A50, //weaponsmokegrenade
		0x88C78EB7, //WEAPONBRIEFCASE
		0x1B79F17,  //WEAPONBRIEFCASE02
		0xFEA23564, //weaponrailgunxm3
	};
	inline std::vector<std::uint32_t> ParachuteCrash = {
		0x720DBB89,
		0x51188CB0,
		0x922C2A43,
		0x84898EFE,
		0x4BB13D0D,
		0xD541462D,
		0x1EC4EAFC,
		0x7FFBC1E2,
		0xAA05E30F,
	};
	inline int DesyncKickType = 0;
	inline const char* DesyncKickTypeNames[] = {
		"Complaint",
		"Instant",
	};
	inline int SuperJumpType = 0;
	inline const char* superjumpnamme[] = {
		"OFF",
		"Beast",
		"Normal",
	};
	inline int JoinType = 0;
	inline const char* JoinTypeName[] = {
		"Invite Method",
		"Ninja Method",
	};
	inline int TrailtypePos = 0;
	inline std::vector<const char*> Trailtype = {
		"Firework",//
		"Beast",//
		"Light",//
		"Firework2"
	};
	inline int InvisibleState = 0;
	inline std::vector<char const*> m_invisibleToggleType = {
		"Locally Visible",
		"Fully Invisible",
	};
	inline int KickMethodState = 0;
	inline std::vector<char const*> m_kickMethodType = {
		"DESYNC",
		"NON_HOST",
		"HOST",
		"INVALID_PU",
	};
	inline int CrashMethodState = 0;
	inline std::vector<char const*> m_CrashMethodType = {
		"Normal",// //0
		"Sync",// //1
		"Parachute",// //2
		"Car Accident",// //3
	};
	inline int ridJoinTypesPos = 0;
	inline std::vector<const char*> ridJoinTypes = {
		"Presence Method", //0
		"Ninja Method", //1
	};
	inline int RainbowSelect = 0;
	inline std::vector<const char*> RainBowVehicleNames = {
		"Fade",
		"Rapid"
	};
	inline int RpDropType = 0;
	inline std::vector <const char*> RpLoopNames = {
		"Figures",
		"Playing Cards",
	};
	struct SessionType
	{
		eSessionType id;
		const char name[22];
	};
	const SessionType sessions[] = {
		{eSessionType::JOIN_PUBLIC, "Join Public Session"},
		{eSessionType::NEW_PUBLIC, "New Public Session"},
		{eSessionType::CLOSED_CREW, "Closed Crew Session"},
		{eSessionType::CREW, "Crew Session"},
		{eSessionType::CLOSED_FRIENDS, "Closed Friend Session"},
		{eSessionType::FIND_FRIEND, "Find Friend Session"},
		{eSessionType::SOLO, "Solo Session"},
		{eSessionType::INVITE_ONLY, "Invite Only Session"},
		{eSessionType::JOIN_CREW, "Join Crew Session"},
		{eSessionType::SC_TV, "SC TV"},
		{eSessionType::LEAVE_ONLINE, "Leave GTA Online"},
	};
	inline std::vector<const char*> TriggerBotNames = {
		"NPC",
		"Players",
		"Both",
	};
}
namespace Regions {
	inline uint32_t RegionTypesPos = 0;
	inline std::vector<const char*> regions = {
		"CIS",//0
		"Africa",//1
		"East",//2
		"Europe",//3
		"China",//4
		"Australia",//5
		"West",//6
		"Japan",//7
		"Unknown",//8
	};
}