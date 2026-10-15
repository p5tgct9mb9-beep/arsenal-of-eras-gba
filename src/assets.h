#pragma once
extern const unsigned char font_bits[672];
#define HT_PANEL_FONT 96
#define HT_PANEL 192
#define HT_BAR 201
#define HT_ICON 210
extern const unsigned int hud_tiles[1720];
extern const unsigned short hud_pal[96];
extern const short sin_lut[256];
extern const unsigned char atan_lut[65];
extern const unsigned int arena_tiles_0[416];
extern const unsigned short arena_pal_0[16];
extern const unsigned short water_pal_0[16];
extern const unsigned int arena_tiles_1[416];
extern const unsigned short arena_pal_1[16];
extern const unsigned short water_pal_1[16];
extern const unsigned int arena_tiles_2[416];
extern const unsigned short arena_pal_2[16];
extern const unsigned short water_pal_2[16];
extern const unsigned int arena_tiles_3[416];
extern const unsigned short arena_pal_3[16];
extern const unsigned short water_pal_3[16];
extern const unsigned int arena_tiles_4[416];
extern const unsigned short arena_pal_4[16];
extern const unsigned short water_pal_4[16];
enum{AM_G0,AM_G1,AM_G2,AM_PATH,AM_WALL,AM_CRATE,AM_BAGS,AM_TREE,AM_CRATER,AM_BORDER,AM_SPAWN,AM_WATER,AM_BRIDGE,AM_N};
extern const unsigned int sky_tiles_0[1024];
extern const unsigned short sky_pal_0[16];
extern const unsigned int sky_tiles_1[1024];
extern const unsigned short sky_pal_1[16];
extern const unsigned int sky_tiles_2[1024];
extern const unsigned short sky_pal_2[16];
extern const unsigned int sky_tiles_3[1024];
extern const unsigned short sky_pal_3[16];
extern const unsigned int sky_tiles_4[1024];
extern const unsigned short sky_pal_4[16];
enum{CM_DEEP,CM_WATER,CM_SURF,CM_WET,CM_SAND,CM_SAND2,CM_HEDGEHOG,CM_CRATER,CM_SHINGLE,CM_WIRE,CM_WIREB,CM_GRASS,CM_GRASS2,CM_TRENCH,CM_BAGS,CM_BK_TL,CM_BK_T,CM_BK_TR,CM_BK_BL,CM_BK_EMB,CM_BK_BR,CM_RUBBLE,CM_BOAT_L,CM_BOAT_R,CM_BOAT_F,CM_BOAT_RAMP,CM_BOAT_ST,CM_DIRT,CM_CONC,CM_BLUFF,CM_N};
extern const unsigned int camp_tiles[960];
extern const unsigned short camp_pal[16];
extern const unsigned short obj_team_pal[240];
extern const unsigned short fx_pal[16];
extern const unsigned int era_spr_0[2240];
#define ES_N0 280
extern const unsigned int era_spr_1[576];
#define ES_N1 72
extern const unsigned int era_spr_2[704];
#define ES_N2 88
extern const unsigned int era_spr_3[576];
#define ES_N3 72
extern const unsigned int era_spr_4[576];
#define ES_N4 72
#define ES_SOLDIER 0
#define ES_HULL 8
#define ES_TUR 24
#define ES_PLANE 40
#define ES_SHIP 72
#define ES_SHIP0 264
extern const unsigned int fx_tiles[512];
#define FX_BULLET 0
#define FX_LASER 1
#define FX_PLASMA 2
#define FX_SHELL 3
#define FX_GRENADE 4
#define FX_FLASH 5
#define FX_SMOKE 6
#define FX_SMOKE2 7
#define FX_SPARK 8
#define FX_RADAR 9
#define FX_MISSILE 13
#define FX_ROCKET 14
#define FX_EXPL 16
#define FX_ARROW 32
#define FX_CRATE 36
#define FX_BANG 40
#define FX_MED 44
#define FX_RETICLE 48
#define FX_BRK_E 52
#define FX_BRK_C 56
#define FX_IFF 60
#define FX_NTILES 64
extern const unsigned short title_bmp[38400];
extern const unsigned short title_dim[38400];
extern const unsigned short title_song[64];
#define SONG_LEN 64
extern const unsigned short fanfare[10];
#define FANFARE_LEN 10
extern const unsigned short sq_freq[110];
extern const unsigned short wv_freq[110];
extern const unsigned char trk0[512];
extern const unsigned char trk1[512];
extern const unsigned char trk2[512];
extern const unsigned char trk3[384];
extern const unsigned char trk4[512];
extern const unsigned char trk5[512];
extern const unsigned char trk6[512];
extern const unsigned char trk7[512];
extern const unsigned char trk8[256];
extern const unsigned char trk9[96];
extern const unsigned char trk_meta[120];
extern const unsigned char*const trk_data[10];
#define NTRK 10
extern const unsigned char wave_shapes[32];
