#pragma once
extern const unsigned char font_bits[672];
#define HT_PANEL_FONT 96
#define HT_PANEL 192
#define HT_BAR 201
#define HT_ICON 210
#define HT_HEART 215
extern const unsigned int hud_tiles[1744];
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
extern const unsigned char trk10[512];
extern const unsigned char trk11[512];
extern const unsigned char trk12[512];
extern const unsigned char trk13[512];
extern const unsigned char trk14[512];
extern const unsigned char trk15[512];
extern const unsigned char trk16[512];
extern const unsigned char trk17[512];
extern const unsigned char trk18[512];
extern const unsigned char trk19[512];
extern const unsigned char trk20[512];
extern const unsigned char trk21[512];
extern const unsigned char trk22[512];
extern const unsigned char trk23[512];
extern const unsigned char trk24[384];
extern const unsigned char trk25[512];
extern const unsigned char trk26[512];
extern const unsigned char trk27[128];
extern const unsigned char trk28[128];
extern const unsigned char trk_meta[348];
extern const unsigned char*const trk_data[29];
#define NTRK 29
extern const unsigned char wave_shapes[32];
extern const unsigned int tank_spr[26880];
extern const unsigned int plane_spr[18176];
extern const unsigned int ship_spr[2944];
extern const unsigned short world_pal[64];
extern const unsigned int world_tiles[1600];
#define WT_N 25
extern const unsigned char world_map[131072];
extern const unsigned short world_ov[28800];
extern const unsigned int world_obj[1424];
#define WO_X15 0
#define WO_B52 64
#define WO_ICON 80
#define WO_RING 156
#define WO_ARROW 172
#define WO_FLAME 176
#define WO_STAR 177
extern const unsigned short world_objpal[112];
extern const unsigned int world_bg1t[1120];
extern const unsigned short world_cloudmap[1024];
extern const unsigned short world_vigmap[1024];
extern const unsigned short world_bg1pal[16];
extern const unsigned short pf_pal[720];
extern const unsigned int pf_tiles_0[1088];
extern const unsigned int pf_tiles_1[1088];
extern const unsigned int pf_tiles_2[1088];
extern const unsigned int pf_tiles_3[1088];
extern const unsigned int pf_tiles_4[1088];
extern const unsigned int pf_tiles_5[1088];
extern const unsigned int pf_tiles_6[1088];
extern const unsigned int pf_tiles_7[1088];
extern const unsigned int pf_tiles_8[1088];
extern const unsigned int pf_tiles_9[1088];
extern const unsigned int pf_tiles_10[1088];
extern const unsigned int pf_tiles_11[1088];
extern const unsigned int pf_tiles_12[1088];
extern const unsigned int pf_tiles_13[1088];
extern const unsigned int pf_tiles_14[1088];
extern const unsigned int pf_bdt_0[1992];
extern const unsigned short pf_bdm_0[1024];
#define PF_BDN_0 249
extern const unsigned int pf_bdt_1[1960];
extern const unsigned short pf_bdm_1[1024];
#define PF_BDN_1 245
extern const unsigned int pf_bdt_2[1520];
extern const unsigned short pf_bdm_2[1024];
#define PF_BDN_2 190
extern const unsigned int pf_bdt_3[2120];
extern const unsigned short pf_bdm_3[1024];
#define PF_BDN_3 265
extern const unsigned int pf_bdt_4[1968];
extern const unsigned short pf_bdm_4[1024];
#define PF_BDN_4 246
extern const unsigned int pf_bdt_5[2208];
extern const unsigned short pf_bdm_5[1024];
#define PF_BDN_5 276
extern const unsigned int pf_bdt_6[1736];
extern const unsigned short pf_bdm_6[1024];
#define PF_BDN_6 217
extern const unsigned int pf_bdt_7[2248];
extern const unsigned short pf_bdm_7[1024];
#define PF_BDN_7 281
extern const unsigned int pf_bdt_8[1952];
extern const unsigned short pf_bdm_8[1024];
#define PF_BDN_8 244
extern const unsigned int pf_bdt_9[2176];
extern const unsigned short pf_bdm_9[1024];
#define PF_BDN_9 272
extern const unsigned int pf_bdt_10[1960];
extern const unsigned short pf_bdm_10[1024];
#define PF_BDN_10 245
extern const unsigned int pf_bdt_11[2128];
extern const unsigned short pf_bdm_11[1024];
#define PF_BDN_11 266
extern const unsigned int pf_bdt_12[2064];
extern const unsigned short pf_bdm_12[1024];
#define PF_BDN_12 258
extern const unsigned int pf_bdt_13[2328];
extern const unsigned short pf_bdm_13[1024];
#define PF_BDN_13 291
extern const unsigned int pf_bdt_14[672];
extern const unsigned short pf_bdm_14[1024];
#define PF_BDN_14 84
extern const unsigned short pf_unit_pal[400];
extern const unsigned int pf_unit_spr[5600];
extern const unsigned int pf_eset_0[576];
extern const unsigned int pf_eset_1[576];
extern const unsigned int pf_eset_2[576];
extern const unsigned int pf_eset_3[576];
extern const unsigned int pf_eset_4[576];
extern const unsigned int pf_eset_5[576];
extern const unsigned int pf_eset_6[576];
extern const unsigned int pf_eset_7[576];
extern const unsigned int pf_eset_8[576];
extern const unsigned int pf_eset_9[576];
extern const unsigned int pf_eset_10[576];
extern const unsigned int pf_eset_11[576];
extern const unsigned int pf_eset_12[576];
extern const unsigned short pf_en_pal[320];
extern const unsigned int pf_bk_tank[512];
extern const unsigned int pf_bk_heli[512];
extern const unsigned int pf_bk_loco[512];
extern const unsigned int pf_bk_car[512];
extern const unsigned int pf_bk_boat[512];
extern const unsigned int pf_bk_walker[512];
extern const unsigned int pf_bk_fort[512];
extern const unsigned int pf_bk_mi8[512];
extern const unsigned int pf_bk_cmdr[256];
extern const unsigned int pf_bk_t55[512];
extern const unsigned int pf_bk_hind[512];
extern const unsigned int pf_bk_aagun[512];
extern const unsigned int pf_bk_tiger[512];
extern const unsigned int pf_bk_bunker[512];
extern const unsigned int pf_bk_zero[512];
extern const unsigned int pf_bk_me262[512];
extern const unsigned int pf_bk_brood[512];
extern const unsigned int pf_bk_mother[512];
extern const unsigned int pf_bk_armtruck[512];
extern const unsigned int pf_bk_technical[512];
extern const unsigned int pf_bk_kpatank[512];
extern const unsigned int pf_bk_t90[512];
extern const unsigned int pf_bk_type99[512];
extern const unsigned int pf_bk_z10[512];
extern const unsigned short pf_boss_pal[288];
extern const unsigned int pf_misc[648];
extern const unsigned short pf_misc_pal[16];
extern const unsigned short pf_wmap_0[38400];
extern const unsigned short pf_wmap_1[38400];
extern const unsigned short pf_wmap_2[38400];
extern const unsigned short pf_wmap_3[38400];
extern const unsigned short pf_wmap_4[38400];
extern const unsigned short pf_wmap_5[38400];
extern const unsigned short pf_wmap_6[38400];
extern const unsigned short pf_wmap_7[38400];
extern const unsigned short pf_wmap_8[38400];
extern const unsigned short pf_wmap_9[38400];
extern const unsigned short pf_globe[38400];
extern const unsigned char pf_lv_0_0[208];
extern const unsigned short pf_le_0_0[78];
extern const unsigned char pf_lv_0_1[230];
extern const unsigned short pf_le_0_1[30];
extern const unsigned char pf_lv_0_2[282];
extern const unsigned short pf_le_0_2[117];
extern const unsigned char pf_lv_0_3[202];
extern const unsigned short pf_le_0_3[51];
extern const unsigned char pf_lv_0_4[150];
extern const unsigned short pf_le_0_4[54];
extern const unsigned char pf_lv_0_5[346];
extern const unsigned short pf_le_0_5[171];
extern const unsigned char pf_lv_0_6[270];
extern const unsigned short pf_le_0_6[99];
extern const unsigned char pf_lv_0_7[384];
extern const unsigned short pf_le_0_7[111];
extern const unsigned char pf_lv_0_8[240];
extern const unsigned short pf_le_0_8[90];
extern const unsigned char pf_lv_1_0[258];
extern const unsigned short pf_le_1_0[147];
extern const unsigned char pf_lv_1_1[196];
extern const unsigned short pf_le_1_1[78];
extern const unsigned char pf_lv_1_2[256];
extern const unsigned short pf_le_1_2[120];
extern const unsigned char pf_lv_1_3[154];
extern const unsigned short pf_le_1_3[69];
extern const unsigned char pf_lv_1_4[292];
extern const unsigned short pf_le_1_4[108];
extern const unsigned char pf_lv_1_5[324];
extern const unsigned short pf_le_1_5[156];
extern const unsigned char pf_lv_1_6[294];
extern const unsigned short pf_le_1_6[165];
extern const unsigned char pf_lv_1_7[472];
extern const unsigned short pf_le_1_7[129];
extern const unsigned char pf_lv_1_8[216];
extern const unsigned short pf_le_1_8[102];
extern const unsigned char pf_lv_2_0[264];
extern const unsigned short pf_le_2_0[96];
extern const unsigned char pf_lv_2_1[208];
extern const unsigned short pf_le_2_1[120];
extern const unsigned char pf_lv_2_2[302];
extern const unsigned short pf_le_2_2[123];
extern const unsigned char pf_lv_2_3[122];
extern const unsigned short pf_le_2_3[57];
extern const unsigned char pf_lv_2_4[272];
extern const unsigned short pf_le_2_4[129];
extern const unsigned char pf_lv_2_5[224];
extern const unsigned short pf_le_2_5[72];
extern const unsigned char pf_lv_2_6[448];
extern const unsigned short pf_le_2_6[126];
extern const unsigned char pf_lv_2_7[472];
extern const unsigned short pf_le_2_7[126];
extern const unsigned char pf_lv_2_8[204];
extern const unsigned short pf_le_2_8[102];
extern const unsigned char pf_lv_3_0[226];
extern const unsigned short pf_le_3_0[99];
extern const unsigned char pf_lv_3_1[320];
extern const unsigned short pf_le_3_1[129];
extern const unsigned char pf_lv_3_2[348];
extern const unsigned short pf_le_3_2[126];
extern const unsigned char pf_lv_3_3[248];
extern const unsigned short pf_le_3_3[93];
extern const unsigned char pf_lv_3_4[480];
extern const unsigned short pf_le_3_4[99];
extern const unsigned char pf_lv_3_5[312];
extern const unsigned short pf_le_3_5[126];
extern const unsigned char pf_lv_3_6[560];
extern const unsigned short pf_le_3_6[144];
extern const unsigned char pf_lv_3_7[444];
extern const unsigned short pf_le_3_7[132];
extern const unsigned char pf_lv_3_8[266];
extern const unsigned short pf_le_3_8[48];
extern const unsigned char pf_lv_4_0[182];
extern const unsigned short pf_le_4_0[105];
extern const unsigned char pf_lv_4_1[208];
extern const unsigned short pf_le_4_1[123];
extern const unsigned char pf_lv_4_2[204];
extern const unsigned short pf_le_4_2[144];
extern const unsigned char pf_lv_4_3[198];
extern const unsigned short pf_le_4_3[57];
extern const unsigned char pf_lv_4_4[178];
extern const unsigned short pf_le_4_4[153];
extern const unsigned char pf_lv_4_5[396];
extern const unsigned short pf_le_4_5[177];
extern const unsigned char pf_lv_4_6[318];
extern const unsigned short pf_le_4_6[183];
extern const unsigned char pf_lv_4_7[444];
extern const unsigned short pf_le_4_7[138];
extern const unsigned char pf_lv_4_8[246];
extern const unsigned short pf_le_4_8[147];
extern const unsigned char pf_lv_5_0[282];
extern const unsigned short pf_le_5_0[108];
extern const unsigned char pf_lv_5_1[84];
extern const unsigned short pf_le_5_1[57];
extern const unsigned char pf_lv_5_2[312];
extern const unsigned short pf_le_5_2[141];
extern const unsigned char pf_lv_5_3[246];
extern const unsigned short pf_le_5_3[159];
extern const unsigned char pf_lv_5_4[212];
extern const unsigned short pf_le_5_4[72];
extern const unsigned char pf_lv_5_5[388];
extern const unsigned short pf_le_5_5[177];
extern const unsigned char pf_lv_5_6[178];
extern const unsigned short pf_le_5_6[69];
extern const unsigned char pf_lv_5_7[522];
extern const unsigned short pf_le_5_7[171];
extern const unsigned char pf_lv_5_8[240];
extern const unsigned short pf_le_5_8[111];
extern const unsigned char pf_lv_6_0[222];
extern const unsigned short pf_le_6_0[72];
extern const unsigned char pf_lv_6_1[306];
extern const unsigned short pf_le_6_1[120];
extern const unsigned char pf_lv_6_2[148];
extern const unsigned short pf_le_6_2[78];
extern const unsigned char pf_lv_6_3[358];
extern const unsigned short pf_le_6_3[183];
extern const unsigned char pf_lv_6_4[224];
extern const unsigned short pf_le_6_4[90];
extern const unsigned char pf_lv_6_5[356];
extern const unsigned short pf_le_6_5[132];
extern const unsigned char pf_lv_6_6[158];
extern const unsigned short pf_le_6_6[48];
extern const unsigned char pf_lv_6_7[432];
extern const unsigned short pf_le_6_7[120];
extern const unsigned char pf_lv_6_8[278];
extern const unsigned short pf_le_6_8[123];
extern const unsigned char pf_lv_7_0[302];
extern const unsigned short pf_le_7_0[117];
extern const unsigned char pf_lv_7_1[200];
extern const unsigned short pf_le_7_1[69];
extern const unsigned char pf_lv_7_2[240];
extern const unsigned short pf_le_7_2[165];
extern const unsigned char pf_lv_7_3[174];
extern const unsigned short pf_le_7_3[87];
extern const unsigned char pf_lv_7_4[156];
extern const unsigned short pf_le_7_4[45];
extern const unsigned char pf_lv_7_5[132];
extern const unsigned short pf_le_7_5[78];
extern const unsigned char pf_lv_7_6[268];
extern const unsigned short pf_le_7_6[105];
extern const unsigned char pf_lv_7_7[396];
extern const unsigned short pf_le_7_7[123];
extern const unsigned char pf_lv_7_8[260];
extern const unsigned short pf_le_7_8[135];
extern const unsigned char pf_lv_8_0[290];
extern const unsigned short pf_le_8_0[132];
extern const unsigned char pf_lv_8_1[318];
extern const unsigned short pf_le_8_1[150];
extern const unsigned char pf_lv_8_2[132];
extern const unsigned short pf_le_8_2[66];
extern const unsigned char pf_lv_8_3[170];
extern const unsigned short pf_le_8_3[66];
extern const unsigned char pf_lv_8_4[280];
extern const unsigned short pf_le_8_4[129];
extern const unsigned char pf_lv_8_5[180];
extern const unsigned short pf_le_8_5[57];
extern const unsigned char pf_lv_8_6[314];
extern const unsigned short pf_le_8_6[84];
extern const unsigned char pf_lv_8_7[208];
extern const unsigned short pf_le_8_7[42];
extern const unsigned char pf_lv_8_8[512];
extern const unsigned short pf_le_8_8[144];
extern const unsigned char pf_lv_8_9[248];
extern const unsigned short pf_le_8_9[51];
extern const unsigned char pf_lv_9_0[248];
extern const unsigned short pf_le_9_0[63];
extern const unsigned char pf_lv_9_1[298];
extern const unsigned short pf_le_9_1[180];
extern const unsigned char pf_lv_9_2[264];
extern const unsigned short pf_le_9_2[153];
extern const unsigned char pf_lv_9_3[320];
extern const unsigned short pf_le_9_3[174];
extern const unsigned char pf_lv_9_4[242];
extern const unsigned short pf_le_9_4[105];
extern const unsigned char pf_lv_9_5[280];
extern const unsigned short pf_le_9_5[129];
extern const unsigned char pf_lv_9_6[422];
extern const unsigned short pf_le_9_6[189];
extern const unsigned char pf_lv_9_7[244];
extern const unsigned short pf_le_9_7[63];
extern const unsigned int kc_spr[160];
extern const unsigned short kc_pal[16];
extern const unsigned short kc_portrait[4608];
