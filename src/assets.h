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
extern const unsigned int arena_tiles_0[352];
extern const unsigned short arena_pal_0[16];
extern const unsigned int arena_tiles_1[352];
extern const unsigned short arena_pal_1[16];
extern const unsigned int arena_tiles_2[352];
extern const unsigned short arena_pal_2[16];
extern const unsigned int arena_tiles_3[352];
extern const unsigned short arena_pal_3[16];
extern const unsigned int arena_tiles_4[352];
extern const unsigned short arena_pal_4[16];
enum{AM_G0,AM_G1,AM_G2,AM_PATH,AM_WALL,AM_CRATE,AM_BAGS,AM_TREE,AM_CRATER,AM_BORDER,AM_SPAWN,AM_N};
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
extern const unsigned int era_spr_0[576];
extern const unsigned int era_spr_1[576];
extern const unsigned int era_spr_2[576];
extern const unsigned int era_spr_3[576];
extern const unsigned int era_spr_4[576];
#define ES_SOLDIER 0
#define ES_HULL 8
#define ES_TUR 24
#define ES_PLANE 40
#define ES_PLANE2 56
#define ES_NTILES 72
extern const unsigned int fx_tiles[384];
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
#define FX_NTILES 48
extern const unsigned short title_bmp[38400];
extern const unsigned short title_dim[38400];
extern const unsigned short title_song[64];
#define SONG_LEN 64
extern const unsigned short fanfare[10];
#define FANFARE_LEN 10
