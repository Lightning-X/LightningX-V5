#pragma once
#include "GTAV-Classes-master/rage/vector.hpp"
class TimecycleKeyframeData
{
public:
	char pad_0000[32]; //0x0000
	rage::fvector4 m_azimuth_east; //0x0020
	float m_azimuth_east_intensity; //0x0030
	char pad_0034[28]; //0x0034
	rage::fvector4 m_azimuth_west; //0x0050
	float m_azimuth_west_intensity; //0x0060
	char pad_0064[28]; //0x0064
	rage::fvector4 m_azimuth_transition; //0x0080
	float m_azimuth_transition_intensity; //0x0090
	char pad_0094[4]; //0x0094
	float m_azimuth_transition_position; //0x0098
	char pad_009C[20]; //0x009C
	rage::fvector4 m_zenith; //0x00B0
	float m_zenith_intensity; //0x00C0
	char pad_00C4[28]; //0x00C4
	rage::fvector4 m_zenith_transition; //0x00E0
	float m_zenith_transition_intensity; //0x00F0
	float m_zenith_transition_position; //0x00F4
	float m_zenith_transition_east_blend; //0x00F8
	float m_zenith_transition_west_blend; //0x00FC
	float m_zenith_blend_start; //0x0100
	char pad_0104[60]; //0x0104
	rage::fvector3 m_plane; //0x0140
	float m_plane_intensity; //0x014C
	char pad_0150[52]; //0x0150
	float m_hdr; //0x0184
	float m_unk_188; //0x0188
	bool m_update_sky_attributes; //0x018C
	char pad_018D[3]; //0x018D
	uint32_t m_unk_190; //0x0190
	uint32_t m_unk_194; //0x0194
	char pad_0198[8]; //0x0198
	rage::fvector4 m_unk_1A0; //0x01A0
	char pad_01AC[16]; //0x01AC
	rage::fvector4 m_sun; //0x01C0
	char pad_01CC[32]; //0x01CC
	rage::fvector4 m_sun_disc; //0x01F0
	char pad_01FC[32]; //0x01FC
	float m_sun_disc_size; //0x0220
	float m_sun_hdr; //0x0224
	float m_sun_miephase; //0x0228
	float m_sun_miescatter; //0x022C
	float m_sun_mie_intensity_mult; //0x0230
	char pad_0234[28]; //0x0234
	rage::fvector4 m_unk_250; //0x0250
	char pad_025C[16]; //0x025C
	float m_cloud_shadow_strength; //0x0270
	float m_cloud_density_mult; //0x0274
	float m_cloud_density_bias; //0x0278
	float m_cloud_fadeout; //0x027C
	char pad_0280[32]; //0x0280
	float m_unk_2A0; //0x02A0
	float m_cloud_offset; //0x02A4
	float m_cloud_overall_color; //0x02A8
	float m_cloud_hdr; //0x02AC
	char pad_02B0[32]; //0x02B0
	float m_cloud_dither_strength; //0x02D0
	char pad_02D4[44]; //0x02D4
	float m_cloud_edge_strength; //0x0300
	char pad_0304[4]; //0x0304
	float m_cloud_overall_strength; //0x0308
	char pad_030C[16]; //0x030C
	rage::fvector4 m_unk_320; //0x031C
	rage::fvector4 m_cloud_base; //0x032C
	rage::fvector4 m_unk_340; //0x033C
	char pad_0348[16]; //0x0348
	rage::fvector4 m_cloud_1; //0x035C
	char pad_0368[20]; //0x0368
	rage::fvector4 m_cloud_mid; //0x0380
	char pad_038C[32]; //0x038C
	float m_unk_380; //0x03B0
	float m_small_cloud_detail_strength; //0x03B4
	float m_small_cloud_density_mult; //0x03B8
	float m_unk_3BC; //0x03BC
	char pad_03C0[32]; //0x03C0
	rage::fvector4 m_small_cloud; //0x03E0
	char pad_03EC[32]; //0x03EC
	float m_sun_influence_radius; //0x0410
	float m_sun_scatter_inten; //0x0414
	float m_moon_influence_radius; //0x0418
	float m_moon_scatter_inten; //0x041C
	char pad_0420[212]; //0x0420
	float m_stars_iten; //0x04F4
	char pad_04F8[60]; //0x04F8
	float m_moon_disc_size; //0x0534
	char pad_0538[24]; //0x0538
	rage::fvector4 m_moon; //0x0550
	float m_moon_intensity; //0x0560
	char pad_0564[140]; //0x0564
}; //Size: 0x05F0
static_assert(sizeof(TimecycleKeyframeData) == 0x5F0);