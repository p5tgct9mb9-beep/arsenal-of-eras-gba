// ======================================================================== v1.3 EOD - BOMB DISPOSAL
// Top-down render-safe ops: detector sweep, bomb suit, Talon-style robot (X-ray + disruptor), manual wire / fuze puzzles,
// blow-in-place, timers, decoys, tripwire booby traps. WW2 (mines, UXB) and Modern (IED, VBIED). 13 scenarios + endless.
#define SCR_EODSEL 63
#define SCR_EODB 64
#define SCR_EOD 65
#define SCR_EODP 66
#define EX_OFF 0xA80
static int ex_isauto(void){u32 v=*(volatile u32*)0x0203FFD0;return v==0x4F545541u||v==0x42545541u;}
static int ex_isbip(void){return *(volatile u32*)0x0203FFD0==0x42545541u;}
static int seg_d2(int px,int py,int x0,int y0,int x1,int y1);
static inline int iminv(int a,int b){return a<b?a:b;}static inline int imaxv(int a,int b){return a>b?a:b;}
enum{DK_MINE,DK_ATM,DK_UXB,DK_IED,DK_VB,DK_CHG,DK_BAG,DK_SMINE,DK_N};
static const char*const DKN[DK_N]={"AP MINE","AT MINE","UXB","IED","VBIED","DEMO CHARGE","SUSPECT BAG","S-MINE"};
static const u8 DKR[DK_N]={20,30,72,40,92,40,36,26}; // blast radius px
enum{EF_NOBIP=1,EF_ROBOT=2,EF_ZUS=4,EF_TIMED=8,EF_AT=16,EF_LANE=32,EF_PLATE=64};
typedef struct{const char*n,*loc,*b1,*b2;u8 era,th,nmine,natm,nuxb,nied,nvb,nchg,nbag,nsm,ndec,ntrip,fl;u16 tsec;}EScn;
// th: 0 field 1 street 2 docks 3 desert road 4 market 5 compound
static const EScn ESC[]={
 {"MINEFIELD GAP","NORMANDY, JUNE 1944","CLEAR THE TAPED LANE FOR THE ARMOR.","TELLERS ARE HEAVY-ONLY: THE SUIT CAN SET THEM OFF.",0,0,6,2,0,0,0,0,0,0,2,0,EF_LANE,180},
 {"LONDON UXB","STEPNEY, SEPTEMBER 1940","250KG BOMB, CLOCKWORK FUZE TICKING.","HOUSES ALL ROUND: NO BLOW-IN-PLACE.",0,1,0,0,1,0,0,0,0,0,1,0,EF_NOBIP|EF_TIMED,150},
 {"S-MINE ORCHARD","ITALY, OCTOBER 1943","BOUNCING BETTYS ON TRIPWIRES.","SWEEP SLOW. WATCH THE WIRES.",0,0,2,0,0,0,0,0,0,3,2,3,0,240},
 {"DELAYED ACTION","LIVERPOOL DOCKS, MAY 1941","TWO UXBS. ONE MAY HAVE A ZUS-40.","ANTI-WITHDRAWAL: TWO SLIPS AND IT FIRES.",0,2,0,0,2,0,0,0,0,0,1,0,EF_ZUS|EF_TIMED,240},
 {"BRIDGE CHARGES","ARNHEM, SEPTEMBER 1944","DEMOLITION CHARGES ON THE BRIDGE.","TIMED. CUT THE FIRING CIRCUIT.",0,1,2,0,0,0,0,3,0,0,1,2,EF_TIMED,210},
 {"BUTTERFLY FIELD","GRIMSBY, JUNE 1943","SD-2 BUTTERFLY BOMBLETS EVERYWHERE.","SCRAP METAL WILL FOOL YOUR DETECTOR.",0,0,7,0,0,0,0,0,0,0,4,0,0,240},
 {"ROUTE CLEARANCE","HELMAND, 2009","PRESSURE-PLATE IEDS ON MSR CORNWALL.","SEND THE ROBOT FIRST. TRUST THE DETECTOR.",2,3,0,0,0,3,0,0,0,0,3,0,EF_ROBOT|EF_PLATE,300},
 {"MARKET DAY","BAGHDAD, 2007","IED HIDDEN IN A BUSY MARKET.","CIVILIANS: NO BLOW-IN-PLACE.",2,4,0,0,0,1,0,0,1,0,3,1,EF_ROBOT|EF_NOBIP|EF_TIMED,180},
 {"CHECKPOINT VBIED","RAMADI, 2006","CAR BOMB STOPPED AT THE CHECKPOINT.","DRIVER BAILED. TIMER RUNNING. SECONDARY LIKELY.",2,5,0,0,0,1,1,0,0,0,2,1,EF_ROBOT|EF_TIMED|EF_AT,200},
 {"DAISY CHAIN","KANDAHAR, 2010","LINKED IEDS ALONG THE WADI ROAD.","ANTI-TAMPER WIRES ON EVERY DEVICE.",2,3,0,0,0,4,0,0,0,0,3,2,EF_ROBOT|EF_AT|EF_PLATE,300},
 {"COMPOUND CLEARANCE","SANGIN, 2011","HOUSE-BORNE IEDS IN A COMPOUND.","DOORWAYS ARE WIRED.",2,5,0,0,0,2,0,0,0,0,2,4,EF_ROBOT|EF_AT,300},
 {"AIRPORT BAG","LONDON, 2019","UNATTENDED BAG IN THE TERMINAL.","COULD BE NOTHING. COULD BE EVERYTHING.",2,4,0,0,0,0,0,0,1,0,2,0,EF_ROBOT|EF_NOBIP|EF_TIMED|EF_AT,120},
 {"FINAL PUSH","MOSUL, 2017","EVERYTHING THEY LEFT BEHIND.","MINES, IEDS AND A CAR BOMB. GOOD LUCK.",2,5,2,0,0,2,1,0,0,0,3,2,EF_ROBOT|EF_TIMED|EF_AT|EF_PLATE,300},
};
#define EX_NS ((int)(sizeof ESC/sizeof ESC[0]))
typedef struct{s16 x,y,tm;u8 k,st,hid,dec,id,at,big;}EDv; // st: 0 armed 1 charged 2 safe 3 destroyed 4 functioned
typedef struct{s16 x0,y0,x1,y1;u8 dv,rev,cut;}ETw;
static EDv ED[20] EWRAM;static int nED;static ETw ETW[8] EWRAM;static int nTW;
static u8 egrid[18][30] EWRAM;static int exTh,exEra,exFl,exT,exTsec,exScore,exHP,exSuit,exCtl,exRob,exMsgT,exHud,exFail,exDone,exBip,exSafeN,exDecN,exBipN,exLost;
static s32 exX,exY,exRX,exRY;static int exDir,exRDir,exSweep,exBeep;static const char*exMsg;static char exMsgB[44] EWRAM;
static int exFxX,exFxY,exFxR,exFxT;static int exFace;
static void ex_msg(const char*s){int k=0;while(s[k]&&k<42){exMsgB[k]=s[k];k++;}exMsgB[k]=0;exMsg=exMsgB;exMsgT=180;exHud=1;}
// ---------------------------------------------------------------- map
static const u16 EXG[6][4]={{RGB(84,120,60),RGB(96,134,66),RGB(74,108,54),RGB(120,100,70)}, // field
 {RGB(120,116,108),RGB(132,128,118),RGB(108,104,98),RGB(90,84,78)},  // street
 {RGB(140,140,136),RGB(150,150,146),RGB(128,128,124),RGB(100,100,98)},// docks
 {RGB(196,170,120),RGB(206,180,130),RGB(184,158,110),RGB(160,136,96)},// desert road
 {RGB(186,164,128),RGB(196,174,138),RGB(172,150,116),RGB(150,130,100)},// market
 {RGB(170,140,100),RGB(180,150,108),RGB(158,128,92),RGB(136,110,80)}};// compound
static u32 exHash(int x,int y){u32 h=(u32)x*0x9E3779B1u^(u32)y*0x85EBCA77u;h^=h>>15;h*=0x2C1B3C6Du;h^=h>>13;return h;}
static void ex_tile(int cx,int cy){int x=cx*8,y=16+cy*8,t=egrid[cy][cx];const u16*G=EXG[exTh];
 for(int j=0;j<8;j++)for(int i=0;i<8;i+=2){u32 h=exHash(x+i,y+j);lf(x+i,y+j,2,1,G[h&1?(h>>8&1):(h>>4&1)*2]);}
 switch(t){
 case 1:lf(x,y,8,8,exTh==3?RGB(176,152,108):exTh==0?RGB(132,110,76):RGB(96,96,100));if(exTh!=0&&exTh!=3&&((cx+cy)&1))lf(x+3,y+3,2,2,RGB(150,150,150));break; // road
 case 2:{u16 w=exTh==1?RGB(150,90,70):exTh==2?RGB(110,100,90):exTh==4?RGB(214,196,160):RGB(186,150,104);lf(x,y,8,8,w);lf(x,y+7,8,1,sn_scale(w,170));
  if(exTh==1&&(cy&1)==0)lf(x,y+3,8,1,sn_scale(w,200));int up=cy>0&&egrid[cy-1][cx]!=2;if(up)lf(x,y,8,2,sn_scale(w,300));break;}
 case 3:lf(x,y,8,8,RGB(40,80,130));if(exHash(cx,cy)&1)lf(x+1,y+2+(cx&3),4,1,RGB(90,140,190));break;
 case 4:lf(x+1,y+1,6,6,exTh==3||exTh==5?RGB(120,120,70):RGB(46,90,40));lf(x+2,y+2,3,2,exTh==3||exTh==5?RGB(150,150,90):RGB(66,120,54));break; // bush
 case 5:lf(x+1,y+4,3,3,RGB(110,104,96));lf(x+4,y+2,3,2,RGB(140,130,120));break;
 case 6:lf(x,y+3,8,1,RGB(250,250,250));if(cx&1)lf(x,y+3,4,1,RGB(220,40,40));break; // lane tape
 case 7:lf(x,y,8,8,RGB(170,170,166));lf(x,y,8,1,RGB(210,210,206));lf(x,y+7,8,1,RGB(110,110,108));break; // concrete barrier
 case 8:{u16 c=(cx*7+cy*3)%3==0?RGB(200,60,50):(cx*7+cy*3)%3==1?RGB(60,110,190):RGB(220,180,60);lf(x,y,8,8,RGB(110,80,50));lf(x,y,8,4,c);break;} // market stall
 }}
static int ex_solid(int px,int py){int cx=px>>3,cy=(py-16)>>3;if(px<0||px>=240||py<16||py>=160)return 1;int t=egrid[cy][cx];return t==2||t==3||t==7||t==8;}
static void ex_genmap(void){memset(egrid,0,sizeof egrid);int th=exTh;
 for(int y=0;y<18;y++)for(int x=0;x<30;x++){u32 h=exHash(x*13+th,y*7);if((h&31)==0)egrid[y][x]=th==0?4:5;}
 if(th==0){for(int y=0;y<18;y++)for(int x=0;x<30;x+=7+(int)(exHash(y,x)&3))if(exHash(x,y)%5==0)egrid[y][x]=4;if(exFl&EF_LANE){for(int x=3;x<30;x++){egrid[6][x]=6;egrid[11][x]=6;}}}
 if(th==1){for(int x=0;x<30;x++){egrid[8][x]=1;egrid[9][x]=1;}for(int b=0;b<5;b++){int x0=2+b*6;for(int x=x0;x<x0+4;x++)for(int y=1;y<5;y++)egrid[y][x]=2;for(int x=x0;x<x0+4;x++)for(int y=13;y<17;y++)egrid[y][x]=2;}}
 if(th==2){for(int x=0;x<30;x++){egrid[0][x]=3;egrid[1][x]=3;egrid[17][x]=3;}for(int x=8;x<12;x++)for(int y=4;y<7;y++)egrid[y][x]=2;for(int x=18;x<24;x++)for(int y=11;y<14;y++)egrid[y][x]=2;}
 if(th==3){for(int x=0;x<30;x++){int c=8+(int)(SIN(x*9)*2>>8);egrid[c][x]=1;egrid[c+1][x]=1;}for(int x=20;x<30;x++)for(int y=1;y<4;y++)if(x<25)egrid[y][x]=2;}
 if(th==4){for(int c=0;c<5;c++)for(int x=4+c*5;x<7+c*5;x++){egrid[3][x]=8;egrid[14][x]=8;}for(int x=0;x<30;x++){egrid[0][x]=2;egrid[17][x]=2;}for(int y=6;y<12;y++)if(y!=8&&y!=9)egrid[y][14]=8;}
 if(th==5){for(int x=12;x<28;x++){egrid[2][x]=2;egrid[15][x]=2;}for(int y=2;y<16;y++){egrid[y][27]=2;if(y<7||y>10)egrid[y][12]=2;}for(int y=2;y<9;y++)egrid[y][20]=2;for(int x=1;x<9;x++)if(x!=4&&x!=5){egrid[5][x]=7;egrid[12][x]=7;}for(int x=0;x<12;x++){egrid[8][x]=1;egrid[9][x]=1;}}
 for(int y=6;y<12;y++)for(int x=0;x<3;x++)if(egrid[y][x]!=1)egrid[y][x]=0; // ICP clear
}
// ---------------------------------------------------------------- art
static void ex_dev_art(const EDv*d){int x=d->x,y=d->y;
 if(d->st==3||d->st==4){lf(x-5,y-3,10,6,RGB(50,44,40));lf(x-3,y-4,6,8,RGB(36,32,30));return;}
 if(d->dec&&d->id){lf(x-3,y-2,6,4,RGB(120,120,120));lf(x-2,y-1,4,1,RGB(170,170,170));return;} // identified scrap
 if(d->hid){return;}
 if(!d->id&&d->dec){lf(x-3,y-2,7,5,RGB(110,90,70));lf(x-2,y-3,5,1,RGB(90,74,60));lf(x+3,y-6,1,6,RGB(240,240,240));lf(x+4,y-6,3,2,RGB(230,60,40));return;} // suspicious + flag
 switch(d->k){
 case DK_MINE:case DK_SMINE:lf(x-3,y-2,6,5,RGB(70,80,60));lf(x-1,y-1,2,2,RGB(170,170,150));if(d->k==DK_SMINE)lf(x-1,y-3,2,1,RGB(150,140,110));break;
 case DK_ATM:lf(x-4,y-3,8,7,RGB(76,84,64));lf(x-3,y-4,6,1,RGB(76,84,64));lf(x-1,y-1,3,3,RGB(140,140,120));break;
 case DK_UXB:lf(x-8,y-3,13,6,RGB(70,72,74));lf(x-9,y-2,1,4,RGB(70,72,74));lf(x+5,y-4,3,8,RGB(90,92,94));lf(x-7,y-3,11,1,RGB(120,122,124));lf(x-3,y-1,2,2,RGB(200,170,60));break;
 case DK_IED:lf(x-4,y-2,8,5,RGB(126,98,62));lf(x-2,y-3,4,1,RGB(150,120,80));lf(x+4,y,4,1,RGB(200,40,40));lf(x-1,y-1,2,2,RGB(230,220,190));break;
 case DK_VB:lf(x-11,y-5,22,10,RGB(190,190,200));lf(x-6,y-5,11,10,RGB(60,80,110));lf(x-10,y-6,4,1,RGB(30,30,30));lf(x+6,y-6,4,1,RGB(30,30,30));lf(x-10,y+5,4,1,RGB(30,30,30));lf(x+6,y+5,4,1,RGB(30,30,30));break;
 case DK_CHG:lf(x-4,y-3,8,6,RGB(190,160,90));lf(x-4,y-1,8,1,RGB(90,60,30));lf(x+4,y-1,3,1,RGB(200,40,40));break;
 case DK_BAG:lf(x-5,y-3,10,6,RGB(30,30,36));lf(x-2,y-4,4,1,RGB(60,60,70));lf(x-4,y-1,8,1,RGB(70,70,80));break;}
 if(d->st==1){lf(x-1,y-6,3,3,C_YEL);} // BIP charge placed
 if(d->st==2){lf(x+4,y-7,4,1,C_GREEN);lf(x+4,y-6,1,2,C_GREEN);lf(x+7,y-8,1,3,C_GREEN);} // render-safe tick
 if(d->hid==0&&d->st==0&&!d->id)lf(x+5,y-8,1,6,RGB(240,240,240)),lf(x+6,y-8,3,2,RGB(230,60,40));}
static void ex_line(int x0,int y0,int x1,int y1,u16 c){int dx=iabs(x1-x0),dy=iabs(y1-y0),sx=x0<x1?1:-1,sy=y0<y1?1:-1,e=dx-dy;for(int n=0;n<600;n++){lf(x0,y0,1,1,c);if(x0==x1&&y0==y1)break;int e2=2*e;if(e2>-dy){e-=dy;x0+=sx;}if(e2<dx){e+=dx;y0+=sy;}}}
// rebake a rect of the static layer (tiles + static items) into dl_bg and VRAM
static void ex_rebake(int x0,int y0,int x1,int y1){if(x0<0)x0=0;if(y0<16)y0=16;if(x1>239)x1=239;if(y1>159)y1=159;if(x1<x0||y1<y0)return;u16*sv=lfb;lfb=dl_bg;
 for(int cy=(y0-16)>>3;cy<=(y1-16)>>3;cy++)for(int cx=x0>>3;cx<=x1>>3;cx++)ex_tile(cx,cy);
 for(int i=0;i<nTW;i++)if(ETW[i].rev&&!ETW[i].cut)ex_line(ETW[i].x0,ETW[i].y0,ETW[i].x1,ETW[i].y1,RGB(230,230,200));
 for(int i=0;i<nED;i++)ex_dev_art(&ED[i]);
 lfb=sv;int ax=(x0>>3)<<3,ay=16+(((y0-16)>>3)<<3),bx=((x1>>3)<<3)+8,by=16+((((y1-16)>>3))<<3)+8;if(bx>240)bx=240;if(by>160)by=160;
 for(int y=ay;y<by;y++)dma16c(dl_bg+y*240+ax,(u16*)0x06000000+y*240+ax,bx-ax);}
static void ex_bake_all(void){u16*sv=lfb;lfb=dl_bg;for(int cy=0;cy<18;cy++)for(int cx=0;cx<30;cx++)ex_tile(cx,cy);
 for(int i=0;i<nTW;i++)if(ETW[i].rev&&!ETW[i].cut)ex_line(ETW[i].x0,ETW[i].y0,ETW[i].x1,ETW[i].y1,RGB(230,230,200));for(int i=0;i<nED;i++)ex_dev_art(&ED[i]);lfb=sv;}
static void ex_dev_rebake(const EDv*d){int r=d->k==DK_VB?13:10;ex_rebake(d->x-r,d->y-r,d->x+r,d->y+r);}
// dynamic sprites into VRAM with dirty-rect restore
typedef struct{s16 x,y,w,h;}ExR;static ExR exDirty[8] EWRAM;static int nExD;
static void ex_restore(void){for(int i=0;i<nExD;i++){ExR*r=&exDirty[i];int x=r->x,y=r->y,w=r->w,h=r->h;if(x<0){w+=x;x=0;}if(y<16){h-=16-y;y=16;}if(x+w>240)w=240-x;if(y+h>160)h=160-y;if(w<=0||h<=0)continue;
  for(int j=0;j<h;j++)dma16c(dl_bg+(y+j)*240+x,(u16*)0x06000000+(y+j)*240+x,w);}nExD=0;}
static void ex_dirty(int x,int y,int w,int h){if(nExD<8){exDirty[nExD].x=x;exDirty[nExD].y=y;exDirty[nExD].w=w;exDirty[nExD].h=h;nExD++;}}
static void ex_tech(int x,int y,int an){ex_dirty(x-6,y-10,12,13);if(exSuit){lf(x-4,y-8,8,9,RGB(70,92,60));lf(x-3,y-10,6,3,RGB(60,80,52));lf(x-2,y-9,4,2,RGB(40,60,80));lf(x-4,y-3,8,1,RGB(54,72,46));
  lf(x-3+(an?1:0),y+1,2,2,RGB(40,50,36));lf(x+1-(an?1:0),y+1,2,2,RGB(40,50,36));}
 else{lf(x-2,y-7,5,7,RGB(110,120,80));lf(x-2,y-9,4,2,RGB(214,170,130));lf(x-2,y-10,4,1,RGB(70,80,50));lf(x-2+(an?1:0),y,2,2,RGB(60,50,40));lf(x+1-(an?1:0),y,2,2,RGB(60,50,40));}
 if(exSweep){int dx=exFace==0?7:exFace==1?-7:0,dy=exFace==2?-7:exFace==3?5:0;if(exFace<2)dy=2;lf(x+dx-2,y+dy-1,4,2,RGB(40,40,40));lf(x+dx-2,y+dy-1,4,1,(exBeep>0)?C_YEL:RGB(90,90,90));ex_dirty(x+dx-3,y+dy-2,6,4);}}
static void ex_robot(int x,int y){ex_dirty(x-7,y-7,14,12);if(!exRob){lf(x-5,y-3,10,6,RGB(40,40,40));return;}lf(x-6,y-4,12,2,RGB(40,40,40));lf(x-6,y+2,12,2,RGB(40,40,40));lf(x-5,y-2,10,4,RGB(90,110,70));
 int ax=exRDir==0?6:exRDir==1?-6:0,ay=exRDir==2?-6:exRDir==3?6:0;lf(x+ax/2-1,y+ay/2-1,2,2,RGB(150,160,140));lf(x+ax-1,y+ay-1,2,2,RGB(200,200,200));lf(x-1,y-6,2,2,RGB(30,30,30));lf(x,y-6,1,1,C_RED);}
// ---------------------------------------------------------------- HUD
static void ex_hud(void){u16*sv=lfb;lfb=(u16*)0x06000000;lf(0,0,240,16,RGB(14,16,22));lf(0,15,240,1,RGB(70,70,40));
 int s=exT/60;if(s<0)s=0;const char*ts=cat3(itoa(s/60),s%60<10?":0":":",itoa(s%60));ltx(2,1,(exFl&EF_TIMED)?"FUZE":"TIME",C_GRAY);ltx(32,1,ts,(exFl&EF_TIMED)&&s<30?C_RED:C_WHITE);
 ltx(66,1,exCtl?"ROBOT":"TECH",exCtl?C_BLUE:C_YEL);for(int i=0;i<(exSuit?3:1);i++)lf(100+i*7,2,5,5,i<exHP?RGB(220,40,40):RGB(60,60,60));
 int left=0;for(int i=0;i<nED;i++)if(!ED[i].dec&&ED[i].st<2)left++;ltx(124,1,cat("LEFT ",itoa(left)),left?C_WHITE:C_GREEN);ltx(176,1,cat("PTS ",itoa(exScore)),C_YEL);
 if(exMsgT>0)ltx(2,8,exMsg,C_YEL);else ltx(2,8,exCtl?"A:ACT B:BIP SEL:TECH":(exSweep?"SWEEPING... A:HOLD":"A:SWEEP/ACT B:BIP SEL:ROBOT"),C_GRAY);
 if(exSweep&&!exCtl&&exMsgT==0){lf(180,9,52,5,RGB(40,40,40));lf(180,9,exBeep>0?52*exBeep/40:0,5,C_YEL);}
 lfb=sv;exHud=0;}
// ---------------------------------------------------------------- explosions
static void ex_hurt(int x,int y,int R){s32 dx=(exX>>8)-x,dy=(exY>>8)-y;int d2=dx*dx+dy*dy;
 if(d2<R*R){int dmg=exSuit?(d2<(R*R)/3?2:1):3;exHP-=dmg;if(exHP<0)exHP=0;sfx_hurt();if(exHP<=0){ex_msg("TECH KILLED IN THE BLAST");exFail=1;}else ex_msg(exSuit?"BLAST! THE SUIT TOOK IT":"BLAST!");}
 if(exRob){dx=(exRX>>8)-x;dy=(exRY>>8)-y;if(dx*dx+dy*dy<R*R){exRob=0;exLost=1;if(exCtl)exCtl=0;if(exHP>0)ex_msg("ROBOT DESTROYED");}}exHud=1;}
static void ex_boom(EDv*d,int bip){int R=DKR[d->k];d->st=bip?3:4;d->hid=0;exFxX=d->x;exFxY=d->y;exFxR=R;exFxT=36;sfx_boom();shake(8);
 if(!bip){exScore-=400;if((exFl&EF_NOBIP)||d->tm>0||d->k==DK_VB){if(!exFail)ex_msg("DEVICE FUNCTIONED - MISSION FAILED");exFail=1;}}
 ex_hurt(d->x,d->y,R);
 for(int i=0;i<nED;i++){EDv*o=&ED[i];if(o==d||o->st>=2||o->dec)continue;int dx=o->x-d->x,dy=o->y-d->y;if(dx*dx+dy*dy<(R*R)/2){o->st=3;o->hid=0;ex_dev_rebake(o);}} // sympathetic
 ex_rebake(d->x-R,d->y-R,d->x+R,d->y+R);}
// ---------------------------------------------------------------- generation
static int ex_place(EDv*d,int road){for(int k=0;k<200;k++){int x=40+rr(190),y=24+rr(130);
  if(road){int cx=x>>3,cy=(y-16)>>3;if(egrid[cy][cx]!=1&&k<150)continue;}
  if(exFl&EF_LANE&&!road){if(y<16+7*8+4||y>16+11*8-4){if(k<160)continue;}}
  if(ex_solid(x,y)||ex_solid(x-6,y)||ex_solid(x+6,y)||ex_solid(x,y-6)||ex_solid(x,y+5))continue;int ok=1;for(int i=0;i<nED;i++){int dx=ED[i].x-x,dy=ED[i].y-y;if(dx*dx+dy*dy<20*20){ok=0;break;}}if(!ok)continue;d->x=x;d->y=y;return 1;}return 0;}
static EDv*ex_add(int k,int hid,int dec){if(nED>=20)return 0;EDv*d=&ED[nED];memset(d,0,sizeof*d);d->k=k;d->hid=hid;d->dec=dec;
 int road=(k==DK_IED&&(exFl&EF_PLATE))||k==DK_VB;if(!ex_place(d,road))return 0;nED++;d->at=(exFl&EF_AT)&&rr(3)!=0;d->big=(exFl&EF_ZUS)&&k==DK_UXB&&(nED==1||rr(2));if(exFl&EF_TIMED)d->tm=(k==DK_UXB||k==DK_VB||k==DK_CHG||k==DK_BAG||k==DK_IED);return d;}
static void ex_setup(const EScn*S,int seed){rs=0xE0D0+seed*7919;exTh=S->th;exEra=S->era;exFl=S->fl;exTsec=S->tsec;ex_genmap();nED=0;nTW=0;
 for(int i=0;i<S->nuxb;i++)ex_add(DK_UXB,0,0);for(int i=0;i<S->nvb;i++)ex_add(DK_VB,0,0);for(int i=0;i<S->nchg;i++)ex_add(DK_CHG,0,0);for(int i=0;i<S->nbag;i++){EDv*d=ex_add(DK_BAG,0,0);if(d&&S->nbag==1&&S->nied==0&&rr(3)==0){d->dec=1;}}
 for(int i=0;i<S->nied;i++)ex_add(DK_IED,(exFl&EF_PLATE)?1:(rr(2)),0);for(int i=0;i<S->nmine;i++)ex_add(DK_MINE,1,0);for(int i=0;i<S->natm;i++)ex_add(DK_ATM,1,0);for(int i=0;i<S->nsm;i++)ex_add(DK_SMINE,1,0);
 for(int i=0;i<S->ndec;i++)ex_add(rr(2)?DK_MINE:DK_IED,rr(2),1);
 // tripwires: anchored at an S-mine or a device, stretched across likely paths
 int anchors[20],na=0;for(int i=0;i<nED;i++)if(!ED[i].dec&&(ED[i].k==DK_SMINE||ED[i].k==DK_IED||ED[i].k==DK_CHG||ED[i].k==DK_BAG))anchors[na++]=i;
 for(int t=0;t<S->ntrip&&na&&nTW<8;t++){int a=anchors[t%na];EDv*d=&ED[a];ETw*w=&ETW[nTW];int L=18+rr(16);int vert=rr(2),sg=rr(2)?1:-1;w->x0=d->x+(vert?3:sg*5);w->y0=d->y+(vert?sg*5:3);w->x1=d->x+(vert?3:sg*L);w->y1=d->y+(vert?sg*L:3);if(w->x1<w->x0){int t=w->x0;w->x0=w->x1;w->x1=t;}if(w->y1<w->y0){int t=w->y0;w->y0=w->y1;w->y1=t;}
  if(w->y0<18)w->y0=18;if(w->y1>158)w->y1=158;if(w->x0<30)w->x0=30;if(w->x1>238)w->x1=238;w->dv=a;w->rev=0;w->cut=0;nTW++;}
 exT=(exFl&EF_TIMED)?exTsec*60:0;exScore=0;exSuit=1;exHP=3;exCtl=0;exRob=(exFl&EF_ROBOT)?1:0;exMsgT=0;exFail=exDone=0;exBipN=exSafeN=exDecN=exLost=0;exFxT=0;
 exX=12<<8;exY=(16+8*8)<<8;exRX=12<<8;exRY=(16+10*8+4)<<8;exFace=0;exRDir=0;nExD=0;}
// ---------------------------------------------------------------- puzzles (full screen in fb)
static int ex_ptick(void){if(exFl&EF_TIMED){exT--;if(exT<=0)return 1;}return 0;}
static void ex_ptimer(void){if(!(exFl&EF_TIMED))return;int s=exT/60;m3_fill(180,4,56,10,RGB(10,10,10));m3_text(184,5,cat3(itoa(s/60),s%60<10?":0":":",itoa(s%60)),s<20?C_RED:C_GREEN,1);}
static const u16 WCOL[7]={RGB(220,40,40),RGB(60,110,230),RGB(240,210,40),RGB(50,180,70),RGB(240,240,240),RGB(70,70,76),RGB(240,130,30)};
static const char*const WCN[7]={"RED","BLUE","YELLOW","GREEN","WHITE","BLACK","ORANGE"};
// wire trace: cut the wire that runs BATTERY -> DETONATOR. AT wires collapse the circuit.
static int ex_wires(EDv*d,int diff){int n=3+diff;if(n>6)n=6;int col[6],dst[6],ly[6][2];int perm[6];for(int i=0;i<n;i++)perm[i]=i;for(int i=n-1;i>0;i--){int j=rr(i+1);int t=perm[i];perm[i]=perm[j];perm[j]=t;}
 for(int i=0;i<n;i++){dst[i]=perm[i];col[i]=diff>=2&&i>0&&rr(3)==0?col[rr(i)]:rr(7);ly[i][0]=40+rr(100);ly[i][1]=40+rr(100);}
 int det=rr(n);int at=-1;if(d->at&&n>3){do at=rr(n);while(at==det);}
 static const char*const RL[5]={"LED","TMR","N/C","SW","N/C"};int sel=0,redraw=1,cut[6]={0};
 for(;;){BEAT->screen=SCR_EODP;BEAT->mode=1;BEAT->x=sel;BEAT->extra=det|(at<0?7:at)<<4|n<<8;
  if(redraw){m3_fill(0,0,240,160,RGB(24,40,32));for(int x=0;x<240;x+=8)m3_fill(x,0,1,160,RGB(30,50,40));for(int y=0;y<160;y+=8)m3_fill(0,y,240,1,RGB(30,50,40));
   m3_fill(0,0,240,16,RGB(10,12,16));m3_text(4,5,cat("RENDER SAFE: ",DKN[d->k]),C_YEL,1);ex_ptimer();
   m3_fill(4,26,30,126,RGB(60,60,70));m3_text(6,20,"BATT",C_GRAY,1);m3_fill(206,26,30,126,RGB(60,60,70));
   for(int i=0;i<n;i++){int y0=40+i*18,y1=40+dst[i]*18;u16 c=WCOL[col[i]];
    int px[5]={34,80,130,170,206},py[5]={y0,ly[i][0],ly[i][1],y1,y1};
    for(int s=0;s<4;s++){if(cut[i]&&s==1){continue;}u16*sv=lfb;lfb=0;ex_line(px[s],py[s],px[s+1],py[s+1],c);ex_line(px[s],py[s]+1,px[s+1],py[s+1]+1,sn_scale(c,160));lfb=sv;}
    m3_fill(28,y0-2,8,6,RGB(200,180,60));m3_fill(204,y1-2,8,6,RGB(200,180,60));
    if(i==sel){m3_text(2,y0-3,">",C_YEL,1);}}
   for(int r=0;r<n;r++){int who=-1;for(int i=0;i<n;i++)if(dst[i]==r)who=i;const char*lb=who==det?"DET":who==at?"AT":RL[r%5];m3_text(212,40+r*18-3,lb,who==det?C_RED:C_WHITE,1);}
   m3_fill(0,148,240,12,RGB(10,12,16));m3_text(4,150,cat3("WIRE ",WCN[col[sel]],"   A:CUT  UP/DN  B:BACK"),C_GRAY,1);m3_present();redraw=0;}
  else{vsync();m3_fill(180,4,56,10,RGB(10,10,10));ex_ptimer();dma32(fb,(void*)0x06000000,240*16/2);}
  frame++;BEAT->frame=frame;poll();if(ex_ptick())return 0;
  if(ex_isauto()&&(frame&15)==0){pressed=sel==det?K_A:K_DOWN;}
  if(pressed&K_UP){sel=(sel+n-1)%n;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%n;redraw=1;sfx_menu();}
  if(pressed&K_B){pressed=0;return 2;}
  if(pressed&K_A&&!cut[sel]){cut[sel]=1;redraw=1;sfx_shot(0);
   if(sel==at){return 0;}if(sel==det){for(int k=0;k<40;k++){next_frame();}return 1;}
   if(exFl&EF_TIMED){exT-=600;if(exT<60)exT=60;}ex_msg("NOTHING HAPPENED. -10S");}}}
// fuze discharge (UXB) or lift (mine): stop the needle in the green
static int ex_crab(EDv*d,int need){int ok=0,miss=0,pos=0,dir=3,zone=40+rr(130),zw=need>1?(d->big?16:22):12,redraw=1;
 for(;;){BEAT->screen=SCR_EODP;BEAT->mode=2;BEAT->x=pos;BEAT->extra=zone|zw<<8;
  pos+=dir;if(pos<0){pos=0;dir=-dir;}if(pos>200){pos=200;dir=-dir;}
  m3_fill(0,0,240,160,RGB(30,30,34));m3_fill(0,0,240,16,RGB(10,12,16));m3_text(4,5,d->k==DK_UXB?"FUZE DISCHARGE (CRABTREE)":"LIFT THE MINE",C_YEL,1);ex_ptimer();
  if(d->k==DK_UXB){m3_fill(50,40,140,50,RGB(80,82,84));m3_fill(40,48,10,34,RGB(100,100,100));m3_fill(190,44,24,42,RGB(70,70,74));m3_fill(108,56,24,18,RGB(200,170,60));m3_fill(116,62,8,6,RGB(60,40,20));
   if(d->big)m3_text(64,94,"ZUS-40 FITTED: 2 SLIPS = FIRE",C_RED,1);}
  else{m3_fill(90,44,60,40,RGB(76,84,64));m3_fill(112,56,16,16,RGB(150,150,130));}
  m3_fill(20,112,200,12,RGB(60,20,20));m3_fill(20+zone,112,zw,12,RGB(40,160,60));m3_fill(20+pos,108,2,20,C_WHITE);
  m3_text(20,132,cat3("TURNS ",itoa(ok),cat("/",itoa(need))),C_WHITE,1);m3_text(120,132,cat("SLIPS ",itoa(miss)),miss?C_RED:C_WHITE,1);m3_text(20,146,"A: TURN WHEN NEEDLE IS GREEN",C_GRAY,1);
  m3_present();frame++;BEAT->frame=frame;poll();if(ex_ptick())return 0;(void)redraw;
  if(ex_isauto()){pressed=(pos>=zone+2&&pos<=zone+zw-3)?K_A:0;}
  if(pressed&K_A){if(pos>=zone-1&&pos<=zone+zw){ok++;sfx_ok();zone=20+rr(160);dir=dir>0?dir+1:dir-1;if(ok>=need){for(int k=0;k<30;k++)next_frame();return 1;}}
   else{miss++;sfx_hurt();if(d->k!=DK_UXB)return 0;if(d->big&&miss>=2)return 0;if(miss>=4)return 0;}}
  if(pressed&K_B){pressed=0;return 2;}}}
// robot X-ray + disruptor: hit the power source (battery) or the PCB
static int ex_xray(EDv*d){int bx=50+rr(130),by=76+rr(40),px=bx+(rr(2)?30:-30),py=by+rr(20)-10,dx=bx+(rr(2)?50:-50),dy=by-20+rr(30);if(px<40)px=40;if(px>196)px=196;if(dx<40)dx=40;if(dx>196)dx=196;if(dy<70)dy=70;if(dy>124)dy=124;
 int cx=120,cy=84,shots=2,mode=0;
 for(;;){BEAT->screen=SCR_EODP;BEAT->mode=3;BEAT->x=cx;BEAT->y=cy;BEAT->extra=bx|by<<8;BEAT->kills=shots;
  m3_fill(0,0,240,160,mode?RGB(10,20,50):RGB(20,20,20));m3_fill(0,0,240,16,RGB(10,12,16));m3_text(4,5,mode?"X-RAY - AIM DISRUPTOR":"ROBOT CAMERA - R: X-RAY",C_YEL,1);ex_ptimer();
  m3_fill(30,30,180,104,mode?RGB(40,70,120):RGB(60,56,50));
  if(mode){m3_fill(50,36,140,28,RGB(90,130,190));m3_text(56,44,"MAIN CHARGE",RGB(150,190,240),1);
   u16*sv=lfb;lfb=0;ex_line(bx+8,by+5,px,py,RGB(220,240,255));ex_line(px,py,dx,dy,RGB(220,240,255));lfb=sv;
   m3_fill(bx,by,16,10,RGB(230,240,255));m3_fill(bx+16,by+3,2,4,RGB(230,240,255));m3_text(bx-2,by+12,"BATT",RGB(200,220,255),1);
   m3_fill(px-6,py-4,12,8,RGB(160,200,160));m3_fill(dx-2,dy-5,4,10,RGB(255,255,255));}
  else{m3_fill(70,60,100,50,d->k==DK_VB?RGB(160,160,170):d->k==DK_BAG?RGB(30,30,36):RGB(126,98,62));m3_text(70,116,DKN[d->k],C_WHITE,1);}
  m3_fill(cx-8,cy,6,1,C_RED);m3_fill(cx+3,cy,6,1,C_RED);m3_fill(cx,cy-8,1,6,C_RED);m3_fill(cx,cy+3,1,6,C_RED);
  m3_text(4,140,cat3("SHOTS ",itoa(shots),"   A:FIRE  R:X-RAY  B:BACK"),C_GRAY,1);
  m3_present();frame++;BEAT->frame=frame;poll();if(ex_ptick())return 0;
  if(ex_isauto()){keys=0;pressed=0;if(!mode)pressed=K_R;else{int ax=bx+8,ay=by+5;if(cx<ax-1)keys|=K_RIGHT;if(cx>ax+1)keys|=K_LEFT;if(cy<ay-1)keys|=K_DOWN;if(cy>ay+1)keys|=K_UP;if(!keys&&(frame&7)==0)pressed=K_A;}}
  if(keys&K_LEFT)cx-=2;if(keys&K_RIGHT)cx+=2;if(keys&K_UP)cy-=2;if(keys&K_DOWN)cy+=2;if(cx<30)cx=30;if(cx>210)cx=210;if(cy<30)cy=30;if(cy>134)cy=134;
  if(pressed&K_R){mode^=1;sfx_menu();}
  if(pressed&K_A){shots--;sfx_shot(1);
   if(cx>=bx-2&&cx<=bx+18&&cy>=by-2&&cy<=by+12){for(int k=0;k<30;k++)next_frame();return 1;}
   if(cx>=px-7&&cx<=px+7&&cy>=py-5&&cy<=py+5){for(int k=0;k<30;k++)next_frame();return 1;}
   if(cx>=dx-4&&cx<=dx+4&&cy>=dy-6&&cy<=dy+6)return 0;
   if(shots<=0)return 3;}
  if(pressed&K_B){pressed=0;return 2;}}}
// ---------------------------------------------------------------- action menu
static int ex_menu(const char*t,const char*const*o,int n){int sel=0;u16*V=(u16*)0x06000000;u16*sv=lfb;
 for(;;){lfb=V;lf(40,50,160,16+n*12,RGB(16,20,40));lf(40,50,160,1,C_YEL);lf(40,65+n*12,160,1,C_YEL);ltx(46,53,t,C_YEL);for(int i=0;i<n;i++)ltx(52,66+i*12,o[i],i==sel?C_YEL:C_WHITE);ltx(44,66+sel*12,">",C_YEL);lfb=sv;
  next_frame();BEAT->screen=SCR_EOD;BEAT->mode=9;BEAT->x=sel;if(ex_isauto()&&(frame&15)==0){if(ex_isbip()&&n>=3&&sel!=n-2)pressed=K_DOWN;else pressed=K_A;}if(pressed&K_UP){sel=(sel+n-1)%n;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%n;sfx_menu();}
  if(pressed&K_A){pressed=0;for(int y=50;y<67+n*12;y++)dma16c(dl_bg+y*240+40,V+y*240+40,160);return sel;}if(pressed&K_B){pressed=0;for(int y=50;y<67+n*12;y++)dma16c(dl_bg+y*240+40,V+y*240+40,160);return -1;}}}
static void ex_full_restore(void){vsync();dma32(dl_bg,(void*)0x06000000,240*160/2);REG_DISPCNT=3|(1<<10);exHud=1;nExD=0;}
static int ex_diff(void){return (exEra==0?0:1)+(SET.bot>=2)+(exFl&EF_AT?1:0);}
static void ex_safe(EDv*d,const char*how){d->st=2;exSafeN++;exScore+=1000+(d->k==DK_VB?500:d->k==DK_UXB?400:0);ex_msg(cat3(DKN[d->k]," RENDERED SAFE",how));sfx_ok();ex_dev_rebake(d);}
static void ex_act(EDv*d){int rob=exCtl;const char*o[3];int n=0,code[3];
 if(rob){if(d->k==DK_IED||d->k==DK_VB||d->k==DK_CHG||d->k==DK_BAG){o[n]="X-RAY + DISRUPTOR";code[n++]=2;}}
 else{if(d->k==DK_UXB){o[n]="DISCHARGE FUZE";code[n++]=3;}else if(d->k==DK_MINE||d->k==DK_ATM||d->k==DK_SMINE){o[n]="LIFT MINE (BY HAND)";code[n++]=4;}else{o[n]="CUT WIRES (MANUAL)";code[n++]=1;}}
 if(!(exFl&EF_NOBIP)){o[n]="PLACE BIP CHARGE";code[n++]=5;}o[n]="CANCEL";code[n++]=0;
 int s=ex_menu(cat("DEVICE: ",DKN[d->k]),o,n);if(s<0)return;int c=code[s];int r=2;
 if(c==5){d->st=1;exBip=1;ex_msg("CHARGE SET - CLEAR THE AREA, B TO FIRE");ex_dev_rebake(d);sfx_menu();return;}
 if(c==0)return;
 if(c==1)r=ex_wires(d,ex_diff());else if(c==2)r=ex_xray(d);else if(c==3)r=ex_crab(d,d->big?5:3);else if(c==4)r=ex_crab(d,1);
 ex_full_restore();
 if(r==1)ex_safe(d,c==2?" (ROBOT)":"");else if(r==0){if(exFl&EF_TIMED&&exT<=0){ex_msg("TIME RAN OUT");}ex_boom(d,0);}else if(r==3){ex_msg("DISRUPTOR MISSED - OUT OF WATER");}}

// test-only autopilot (harness pokes 'AUTO' at 0x0203FFD0): walks/sweeps to each device, cuts wires, solves puzzles
static int exAT=-1,exAstk;
static int ex_rbt_ok(const EDv*d){return d->k==DK_IED||d->k==DK_VB||d->k==DK_CHG||d->k==DK_BAG;}

static u8 exBD[18][30] EWRAM;
static int ex_cfree(int cx,int cy){if(cx<0||cx>=30||cy<0||cy>=18)return 0;int px=cx*8+4,py=16+cy*8+4;return !ex_solid(px,py)&&!ex_solid(px-3,py)&&!ex_solid(px+3,py)&&!ex_solid(px,py-3)&&!ex_solid(px,py+3);}
static void ex_bfs(int gcx,int gcy,int tgt){static u8 q[540*2] EWRAM;memset(exBD,255,sizeof exBD);int h=0,t=0;
 u8 blk[18][30];for(int y=0;y<18;y++)for(int x=0;x<30;x++)blk[y][x]=!ex_cfree(x,y);
 for(int i=0;i<nED;i++){EDv*d=&ED[i];if(i==tgt||d->hid||d->st>=2||d->dec)continue;int cx=d->x>>3,cy=(d->y-16)>>3;if(cy>=0&&cy<18)blk[cy][cx]=1;}
 if(gcx<0)gcx=0;if(gcx>29)gcx=29;if(gcy<0)gcy=0;if(gcy>17)gcy=17;exBD[gcy][gcx]=0;q[t++]=gcx;q[t++]=gcy;
 while(h<t){int x=q[h++],y=q[h++];int dd=exBD[y][x];static const s8 DX[4]={1,-1,0,0},DY[4]={0,0,1,-1};for(int k=0;k<4;k++){int nx=x+DX[k],ny=y+DY[k];if(nx<0||nx>=30||ny<0||ny>=18||blk[ny][nx]||exBD[ny][nx]!=255)continue;exBD[ny][nx]=dd+1;q[t++]=nx;q[t++]=ny;}}}
static void ex_auto(void){if(!ex_isauto())return;int tx=exX>>8,ty=exY>>8;
 // pick target: revealed uncut tripwire > nearest unresolved device > ICP
 int gx=12,gy=88,act=0,wire=-1,useR=0;
 for(int i=0;i<nTW;i++){ETw*w=&ETW[i];if(w->rev&&!w->cut){wire=i;break;}}
 if(wire>=0){ETw*w=&ETW[wire];int mx=(w->x0+w->x1)/2,my=(w->y0+w->y1)/2;int vert=w->x0==w->x1;if(vert){gx=mx+(tx<mx?-6:6);gy=my;}else{gx=mx;gy=my+(ty<my?-6:6);}act=2;}
 else{int bi=-1,bd=1<<30;for(int i=0;i<nED;i++){EDv*d=&ED[i];if(d->st>=2)continue;if(d->dec&&(d->id||d->hid))continue;int dx=d->x-tx,dy=d->y-ty;int dd=dx*dx+dy*dy;if(dd<bd){bd=dd;bi=i;}}
  if(bi>=0){EDv*d=&ED[bi];static const s8 OX[8]={-10,10,0,0,-8,8,-8,8},OY[8]={0,0,-9,9,-8,-8,8,8};gx=d->x-10;gy=d->y;for(int o=0;o<8;o++){int ax=d->x+OX[o],ay=d->y+OY[o];if(ex_cfree(ax>>3,(ay-16)>>3)&&!ex_solid(ax-3,ay)&&!ex_solid(ax+3,ay)&&!ex_solid(ax,ay-3)&&!ex_solid(ax,ay+3)){gx=ax;gy=ay;break;}}act=1;exAT=bi;useR=(exFl&EF_ROBOT)&&exRob&&ex_rbt_ok(d)&&!d->dec;}}
 if(exBip){useR=0;gx=12;gy=88;act=0;int far=1;for(int k=0;k<nED;k++)if(ED[k].st==1){int R=DKR[ED[k].k]+6;int ddx=ED[k].x-tx,ddy=ED[k].y-ty;if(ddx*ddx+ddy*ddy<R*R)far=0;}if(far&&(frame&15)==0){pressed|=K_B;keys=0;return;}}
 if(useR!=exCtl&&(exRob||!useR)){pressed|=K_SEL;keys=0;return;}
 int cx=exCtl?exRX>>8:tx,cy=exCtl?exRY>>8:ty;
 int k=exCtl?0:K_A;int dx=gx-cx,dy=gy-cy;
 {static int bg=-1;int gc=(gx>>3)|(((gy-16)>>3)<<5);if(gc!=bg||(frame&31)==0){ex_bfs(gx>>3,(gy-16)>>3,exAT);bg=gc;}
  int ccx=cx>>3,ccy=(cy-16)>>3;int here=exBD[ccy][ccx];
  if(here!=255&&here>0){static const s8 DX[4]={1,-1,0,0},DY[4]={0,0,1,-1};int bk=-1,bv=here;for(int q=0;q<4;q++){int nx=ccx+DX[q],ny=ccy+DY[q];if(nx<0||nx>=30||ny<0||ny>=18)continue;if(exBD[ny][nx]<bv){bv=exBD[ny][nx];bk=q;}}
   if(bk>=0){int nx=(ccx+DX[bk])*8+4,ny=16+(ccy+DY[bk])*8+4;dx=nx-cx;dy=ny-cy;if(DX[bk])dy=(16+ccy*8+4)-cy;else dx=(ccx*8+4)-cx;}}}
 if(iabs(dx)>1)k|=dx>0?K_RIGHT:K_LEFT;if(iabs(dy)>1)k|=dy>0?K_DOWN:K_UP;
 // avoid known armed pressure devices / wires that are not the target
 for(int i=0;i<nED;i++){EDv*d=&ED[i];if(i==exAT||d->hid||d->st>=2||d->dec)continue;if((d->x-gx)*(d->x-gx)+(d->y-gy)*(d->y-gy)<16*16)continue;int ex=d->x-cx,ey=d->y-cy;if(ex*ex+ey*ey<12*12){if(iabs(ex)>iabs(ey))k=(k&~(K_LEFT|K_RIGHT))|(ey>0?K_UP:K_DOWN);else k=(k&~(K_UP|K_DOWN))|(ex>0?K_LEFT:K_RIGHT);}}
 for(int i=0;i<nTW;i++){ETw*w=&ETW[i];if(!w->rev||w->cut||i==wire)continue;if(seg_d2(cx,cy,w->x0,w->y0,w->x1,w->y1)<6*6){if(w->x0==w->x1)k=(k&~(K_LEFT|K_RIGHT))|(cx<w->x0?K_LEFT:K_RIGHT);else k=(k&~(K_UP|K_DOWN))|(cy<w->y0?K_UP:K_DOWN);}}
 static int lx,ly;if(cx==lx&&cy==ly&&(k&0xF0))exAstk++;else exAstk=0;lx=cx;ly=cy;if(exAstk>30){k=(k&~0xF0)|((frame>>5)&1?K_UP:K_DOWN);}
 keys=k;
 if(iabs(gx-cx)<=2&&iabs(gy-cy)<=2){if(act==2){pressed|=K_A;}else if(act==1){EDv*d=&ED[exAT];if(d->id&&d->st==0)pressed|=K_A;}}}
// ---------------------------------------------------------------- frame update
static int ex_near(int x,int y,int r){for(int i=0;i<nED;i++){EDv*d=&ED[i];if(d->hid||d->st>=2)continue;int dx=d->x-x,dy=d->y-y;if(dx*dx+dy*dy<=r*r)return i;}return -1;}
static void ex_move(s32*X,s32*Y,int sp,int*face){int dx=0,dy=0;if(keys&K_LEFT){dx=-sp;*face=1;}if(keys&K_RIGHT){dx=sp;*face=0;}if(keys&K_UP){dy=-sp;*face=2;}if(keys&K_DOWN){dy=sp;*face=3;}
 s32 nx=*X+dx,ny=*Y+dy;if(!ex_solid((nx>>8)-3,*Y>>8)&&!ex_solid((nx>>8)+3,*Y>>8))*X=nx;if(!ex_solid(*X>>8,(ny>>8)-2)&&!ex_solid(*X>>8,(ny>>8)+2))*Y=ny;
 if(*X<(4<<8))*X=4<<8;if(*X>(236<<8))*X=236<<8;}
static int seg_d2(int px,int py,int x0,int y0,int x1,int y1){int vx=x1-x0,vy=y1-y0,wx=px-x0,wy=py-y0;int L=vx*vx+vy*vy;int t=L?(wx*vx+wy*vy)*256/L:0;if(t<0)t=0;if(t>256)t=256;int qx=x0+vx*t/256-px,qy=y0+vy*t/256-py;return qx*qx+qy*qy;}
static void ex_triggers(int x,int y,int tech){
 for(int i=0;i<nED;i++){EDv*d=&ED[i];if(d->st>=2||d->dec)continue;int dx=d->x-x,dy=d->y-y;int pr=(d->k==DK_MINE||d->k==DK_SMINE||d->k==DK_ATM||(d->k==DK_IED&&(exFl&EF_PLATE)))?4:0;
  if(d->k==DK_ATM&&!(tech&&exSuit))pr=0;if(pr&&dx*dx+dy*dy<=pr*pr){ex_msg(cat(DKN[d->k]," - PRESSURE TRIGGER!"));ex_boom(d,0);return;}}
 for(int i=0;i<nTW;i++){ETw*w=&ETW[i];if(w->cut)continue;if(seg_d2(x,y,w->x0,w->y0,w->x1,w->y1)<=3){w->cut=1;EDv*d=&ED[w->dv];ex_rebake(iminv(w->x0,w->x1)-1,iminv(w->y0,w->y1)-1,imaxv(w->x0,w->x1)+1,imaxv(w->y0,w->y1)+1);if(d->st<2){ex_msg("TRIPWIRE!");ex_boom(d,0);}return;}}}
static void ex_update(void){ex_auto();int tx=exX>>8,ty=exY>>8,rx=exRX>>8,ry=exRY>>8;
 if(exFl&EF_TIMED){if(--exT<=0){exT=0;for(int i=0;i<nED;i++)if(ED[i].tm&&ED[i].st<2&&!ED[i].dec){ex_msg("FUZE RAN DOWN - DEVICE FUNCTIONED");ex_boom(&ED[i],0);break;}exFail=1;}if(exT%60==0)exHud=1;}else{exT++;if(exT%60==0)exHud=1;}
 if(exMsgT>0){if(--exMsgT==0)exHud=1;}
 if(pressed&K_SEL){if(exFl&EF_ROBOT&&exRob){exCtl^=1;exHud=1;sfx_menu();}else if(exFl&EF_ROBOT){ex_msg("ROBOT IS GONE");}else ex_msg("NO ROBOT ON THIS TASK");}
 exSweep=0;
 if(!exCtl){int sweep=(keys&K_A)!=0;int sp=exSuit?(sweep?72:115):(sweep?150:280);ex_move(&exX,&exY,sp,&exFace);exSweep=sweep;
  // detector
  if(sweep){int best=999;int bi=-1;for(int i=0;i<nED;i++){EDv*d=&ED[i];if(!d->hid||d->st>=2)continue;int dx=d->x-tx,dy=d->y-ty;int dd=idist(dx,dy);if(dd<best){best=dd;bi=i;}}
   int nb=best<40?40-best:0;if(nb!=exBeep)exHud=1;exBeep=nb;if(nb>0&&(frame%(2+best/3))==0)sfx1(0x08,0x8,1,2,1500+nb*10,4);
   if(bi>=0&&best<13){EDv*d=&ED[bi];d->hid=0;ex_msg("CONTACT - MARKED");exHud=1;ex_dev_rebake(d);}
   for(int i=0;i<nTW;i++){ETw*w=&ETW[i];if(!w->rev&&!w->cut&&seg_d2(tx,ty,w->x0,w->y0,w->x1,w->y1)<14*14){w->rev=1;ex_msg("TRIPWIRE SPOTTED");ex_rebake(iminv(w->x0,w->x1)-1,iminv(w->y0,w->y1)-1,imaxv(w->x0,w->x1)+1,imaxv(w->y0,w->y1)+1);}}}
  else exBeep=0;
  ex_triggers(exX>>8,exY>>8,1);if(exFail)return;tx=exX>>8;ty=exY>>8;
  // identify suspicious items by eye
  for(int i=0;i<nED;i++){EDv*d=&ED[i];if(d->hid||d->id||d->st>=2)continue;int dx=d->x-tx,dy=d->y-ty;if(dx*dx+dy*dy<15*15){d->id=1;if(d->dec){d->st=2;exDecN++;exScore+=200;ex_msg("DECOY - SCRAP. CLEARED +200");}else ex_msg(cat("IDENTIFIED: ",DKN[d->k]));ex_dev_rebake(d);}}
  if(pressed&K_A){int i=ex_near(tx,ty,14);if(i>=0&&ED[i].id&&ED[i].st==0){pressed=0;ex_act(&ED[i]);return;}
   for(int k=0;k<nTW;k++){ETw*w=&ETW[k];if(w->rev&&!w->cut&&seg_d2(tx,ty,w->x0,w->y0,w->x1,w->y1)<8*8){int mx=(w->x0+w->x1)/2,my=(w->y0+w->y1)/2;if((tx-mx)*(tx-mx)+(ty-my)*(ty-my)<10*10){w->cut=1;exScore+=150;ex_msg("TRIPWIRE CUT SAFELY +150");sfx_ok();ex_rebake(iminv(w->x0,w->x1)-1,iminv(w->y0,w->y1)-1,imaxv(w->x0,w->x1)+1,imaxv(w->y0,w->y1)+1);break;}}}}}
 else if(exRob){ex_move(&exRX,&exRY,180,&exRDir);rx=exRX>>8;ry=exRY>>8;
  for(int i=0;i<nED;i++){EDv*d=&ED[i];if(d->st>=2)continue;int dx=d->x-rx,dy=d->y-ry;int d2=dx*dx+dy*dy;if(d->hid&&d2<14*14){d->hid=0;ex_msg("ROBOT CAMERA: CONTACT");ex_dev_rebake(d);}
   if(!d->hid&&!d->id&&d2<22*22){d->id=1;if(d->dec){d->st=2;exDecN++;exScore+=200;ex_msg("ROBOT: DECOY. CLEARED +200");}else ex_msg(cat("ROBOT ID: ",DKN[d->k]));ex_dev_rebake(d);}}
  for(int i=0;i<nTW;i++){ETw*w=&ETW[i];if(!w->rev&&!w->cut&&seg_d2(rx,ry,w->x0,w->y0,w->x1,w->y1)<26*26){w->rev=1;ex_msg("ROBOT CAMERA: TRIPWIRE");ex_rebake(iminv(w->x0,w->x1)-1,iminv(w->y0,w->y1)-1,imaxv(w->x0,w->x1)+1,imaxv(w->y0,w->y1)+1);}}
  ex_triggers(rx,ry,0);if(exFail)return;
  if(pressed&K_A){int i=ex_near(rx,ry,16);if(i>=0&&ED[i].id&&ED[i].st==0){pressed=0;ex_act(&ED[i]);return;}}}
 // BIP: place (handled in menu) / fire
 if(pressed&K_B){int i=ex_near(exCtl?rx:tx,exCtl?ry:ty,14);
  if(i>=0&&ED[i].st==0&&ED[i].id){if(exFl&EF_NOBIP)ex_msg("BLOW-IN-PLACE NOT AUTHORISED HERE");else{ED[i].st=1;exBip=1;ex_msg("CHARGE SET - CLEAR THE AREA, B TO FIRE");ex_dev_rebake(&ED[i]);sfx_menu();}}
  else if(exBip){int okk=1;for(int k=0;k<nED;k++)if(ED[k].st==1){int R=DKR[ED[k].k];int dx=ED[k].x-tx,dy=ED[k].y-ty;if(dx*dx+dy*dy<R*R)okk=0;}
   if(!okk)ex_msg("TOO CLOSE! GET OUTSIDE THE BLAST RADIUS");
   else{for(int k=0;k<nED;k++)if(ED[k].st==1){ex_boom(&ED[k],1);exBipN++;exScore+=300;}exBip=0;ex_msg("FIRING... BIP COMPLETE +300");}}}
 // win check
 int left=0;for(int i=0;i<nED;i++)if(!ED[i].dec&&ED[i].st<2)left++;
 if(!left&&!exFail){if(tx<26&&ty>16+5*8&&ty<16+13*8){exDone=1;}else if(exMsgT==0)ex_msg("ALL CLEAR - WALK BACK TO THE ICP");}}
static void ex_draw(void){ex_restore();if(exHud)ex_hud();u16*sv=lfb;lfb=(u16*)0x06000000;
 // ICP box
 if(exFxT>0){int r=exFxR*(36-exFxT)/36+4;if(r>exFxR)r=exFxR;int a=exFxX-r,b=exFxY-r;lf(a+r/3,b+r/3,r*4/3,r*4/3,exFxT>24?RGB(255,220,120):exFxT>12?RGB(240,120,40):RGB(90,80,70));lf(a+r/2,b,r,r*2,exFxT>20?RGB(255,180,80):RGB(110,100,90));lf(a,b+r/2,r*2,r,exFxT>20?RGB(255,180,80):RGB(110,100,90));ex_dirty(a-1,b-1,2*r+3,2*r+3);exFxT--;}
 if(exFl&EF_ROBOT)ex_robot(exRX>>8,exRY>>8);ex_tech(exX>>8,exY>>8,(keys&(K_LEFT|K_RIGHT|K_UP|K_DOWN))&&((frame>>3)&1));
 if(exCtl&&exRob){int x=exRX>>8,y=exRY>>8;lf(x-1,y-11,3,2,C_BLUE);ex_dirty(x-2,y-12,5,4);}
 lfb=sv;}
// ---------------------------------------------------------------- save
typedef struct{u16 best[EX_NS];u8 stars[EX_NS];u16 endBest;u8 endRounds,last;u16 safe,bip;}ExSave;static ExSave EXS EWRAM;
static void exs_write(void){const u8*b=(const u8*)&EXS;u8 s=0x58;SRAM[EX_OFF]='A';SRAM[EX_OFF+1]='O';SRAM[EX_OFF+2]='E';SRAM[EX_OFF+3]='X';SRAM[EX_OFF+4]=1;for(int i=0;i<(int)sizeof EXS;i++){SRAM[EX_OFF+5+i]=b[i];s+=b[i]*(i+1);}SRAM[EX_OFF+5+sizeof EXS]=s;}
static void exs_read(void){memset(&EXS,0,sizeof EXS);if(SRAM[EX_OFF]!='A'||SRAM[EX_OFF+1]!='O'||SRAM[EX_OFF+2]!='E'||SRAM[EX_OFF+3]!='X'||SRAM[EX_OFF+4]!=1)return;u8 b[sizeof EXS];u8 s=0x58;for(int i=0;i<(int)sizeof EXS;i++){b[i]=SRAM[EX_OFF+5+i];s+=b[i]*(i+1);}if(s==SRAM[EX_OFF+5+sizeof EXS])memcpy(&EXS,b,sizeof EXS);}
static void rec_eod(const char**L){static char b[5][40];exs_read();int n=0,st=0;for(int i=0;i<EX_NS;i++){n+=EXS.stars[i]>0;st+=EXS.stars[i];}
 for(int k=0;k<5;k++){const char*a=k==0?cat3("TASKS COMPLETE: ",itoa(n),"/13"):k==1?cat3("STARS: ",itoa(st),"/39"):k==2?cat3("ENDLESS BEST: ",itoa(EXS.endBest),cat(" (R",cat(itoa(EXS.endRounds),")"))):k==3?cat3("RENDERED SAFE: ",itoa(EXS.safe),""):cat3("BLOWN IN PLACE: ",itoa(EXS.bip),"");int j=0;while(a[j]&&j<38){b[k][j]=a[j];j++;}b[k][j]=0;L[k]=b[k];}}
// ---------------------------------------------------------------- run
static void ex_present_beat(void){BEAT->screen=SCR_EOD;BEAT->hp=exHP;BEAT->kills=exSafeN;BEAT->scoreA=exScore;BEAT->scoreB=exBipN|(exDecN<<8);BEAT->phase=exFail?5:exDone?4:0;BEAT->x=exCtl?exRX>>8:exX>>8;BEAT->y=exCtl?exRY>>8:exY>>8;BEAT->timer=exT;
 int left=0,hid=0;for(int i=0;i<nED;i++){if(!ED[i].dec&&ED[i].st<2)left++;if(ED[i].hid)hid++;}BEAT->extra=left|(hid<<8)|(exCtl<<16)|(exRob<<17);}
static int ex_play(void){ex_bake_all();ex_full_restore();bgm_t(TR_EOD);int endT=0;
 for(;;){ex_present_beat();vsync();ex_draw();frame++;BEAT->frame=frame;poll();
  if(!exFail&&!exDone)ex_update();else endT++;
  if(endT>100)break;
  if(pressed&K_START){int q=0;u16*V=(u16*)0x06000000;u16*sv=lfb;lfb=V;lf(50,66,140,28,RGB(16,20,40));ltc(70,"PAUSED",C_YEL);ltc(82,"START: RESUME  B: ABORT",C_WHITE);lfb=sv;for(;;){next_frame();if(pressed&K_START)break;if(pressed&K_B){q=1;break;}}pressed=0;ex_full_restore();if(q)return -1;}}
 if(exFail){if(exScore<0)exScore=0;music_defeat();return 0;}
 int tb=(exFl&EF_TIMED)?exT/60*5:(exTsec*60>exT?(exTsec*60-exT)/60*3:0);exScore+=tb+exHP*100+(exRob?300:0)-(exLost?500:0);if(exScore<0)exScore=0;return 1;}
static int ex_stars(const EScn*S){int real=S->nmine+S->natm+S->nuxb+S->nied+S->nvb+S->nchg+S->nbag+S->nsm;int ideal=real*1000+S->ndec*200+600;return 1+(exScore*10>=ideal*6)+(exScore*10>=ideal*9);}
static void ex_result(const char*n,int ok,int stars,int best,int endless){int t=0;for(;;){BEAT->screen=SCR_EOD;BEAT->phase=ok?4:5;BEAT->scoreA=exScore;BEAT->scoreB=stars;
  if(t==0){chrome(CAT_SPECIAL,0);m3_box(16,24,208,116,1);m3_ctext(32,ok?"AREA CLEAR":"TASK FAILED",ok?C_YEL:C_RED,2);m3_ctext(52,n,C_WHITE,1);
   m3_text(30,68,cat3("RENDERED SAFE ",itoa(exSafeN),cat("   BIP ",itoa(exBipN))),C_WHITE,1);m3_text(30,80,cat3("DECOYS CLEARED ",itoa(exDecN),exLost?"   ROBOT LOST":""),C_WHITE,1);
   m3_text(30,92,cat3("SCORE ",itoa(exScore),best?"  BEST!":""),best?C_GREEN:C_WHITE,1);if(ok&&!endless)for(int s=0;s<3;s++)m3_fill(96+s*18,106,12,12,s<stars?C_YEL:RGB(50,50,60));
   m3_ctext(126,"A: CONTINUE",C_GRAY,1);m3_present();}
  next_frame();t++;if(ex_isauto()&&t>60)pressed=K_A;if(t>20&&(pressed&(K_A|K_B))){pressed=0;return;}}}
static int ex_wrap(int x,int y,const char*t,u16 c,int mx){static char b[48];int n=0;int ln=0;while(*t){int k=0,last=-1;while(t[k]&&k<mx){if(t[k]==' ')last=k;k++;}if(t[k]&&last>0)k=last;int j;for(j=0;j<k&&j<47;j++)b[j]=t[j];b[j]=0;m3_text(x,y+ln*10,b,c,1);ln++;t+=k;while(*t==' ')t++;n++;if(n>3)break;}return ln;}
static void ex_brief(int si){const EScn*S=&ESC[si];int redraw=1,suit=1;
 for(;;){BEAT->screen=SCR_EODB;BEAT->mode=si;BEAT->x=suit;
  if(redraw){cat_bgc(CAT_SPECIAL);banner(S->n,S->loc,C_YEL);m3_box(4,38,232,118,1);{int l=ex_wrap(12,44,S->b1,C_WHITE,36);ex_wrap(12,44+l*10,S->b2,C_GRAY,36);}
   static const char*const TH[6]={"FIELD","STREET","DOCKS","ROUTE","MARKET","COMPOUND"};m3_text(12,76,cat3("ERA ",S->era?"MODERN":"WW2",cat("   AREA ",TH[S->th])),C_BLUE,1);
   m3_text(12,86,cat3((S->fl&EF_TIMED)?"FUZE TIMER ":"PAR TIME ",itoa(S->tsec),"S"),(S->fl&EF_TIMED)?C_RED:C_WHITE,1);m3_text(120,86,(S->fl&EF_ROBOT)?"ROBOT: TALON":"ROBOT: NONE",C_WHITE,1);
   m3_text(12,98,(S->fl&EF_NOBIP)?"BLOW-IN-PLACE: FORBIDDEN":"BLOW-IN-PLACE: AUTHORISED",(S->fl&EF_NOBIP)?C_RED:C_GREEN,1);
   m3_text(12,112,cat("BOMB SUIT <L R>: ",suit?"ON (SLOW, 3 HP)":"OFF (FAST, 1 HP)"),C_YEL,1);
   m3_text(12,126,cat3("BEST ",itoa(EXS.best[si]),cat("   STARS ",itoa(EXS.stars[si]))),C_GRAY,1);m3_ctext(142,"A: DEPLOY   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();if(pressed&(K_L|K_R|K_LEFT|K_RIGHT)){suit^=1;redraw=1;sfx_menu();}
  if(pressed&K_B){pressed=0;return;}
  if(pressed&K_A){pressed=0;sfx_ok();ex_setup(S,si);exSuit=suit;exHP=suit?3:1;int r=ex_play();
   if(r>=0){int st=r?ex_stars(S):0;int best=r&&exScore>EXS.best[si];if(best)EXS.best[si]=exScore;if(st>EXS.stars[si])EXS.stars[si]=st;EXS.safe+=exSafeN;EXS.bip+=exBipN;EXS.last=si;exs_write();ex_result(S->n,r,st,best,0);if(r)music_victory();}
   bgm_t(TR_EOD);redraw=1;}}}
static void ex_endless(void){int round=0,total=0;static EScn R;
 for(;;){round++;int era=round&1?0:2;R.n="ENDLESS";R.loc="RANDOM TASKING";R.b1="";R.b2="";R.era=era;R.th=era?3+rr(3):rr(3);
  int k=2+round/2;if(k>10)k=10;R.nmine=era?0:rr(k/2+1);R.natm=era?0:rr(2);R.nuxb=era?0:(round>2?rr(2):0);R.nied=era?1+rr(k/2+1):0;R.nvb=era&&round>3?rr(2):0;R.nchg=era?0:rr(2);R.nbag=era?rr(2):0;R.nsm=era?0:rr(2);R.ndec=1+rr(3);R.ntrip=round>2?rr(3):0;
  if(R.nmine+R.natm+R.nuxb+R.nied+R.nvb+R.nchg+R.nbag+R.nsm==0)R.nmine=1;R.fl=(era?EF_ROBOT|EF_PLATE:0)|(round>2&&rr(2)?EF_TIMED:0)|(round>4?EF_AT:0)|(R.th==4?EF_NOBIP:0);R.tsec=150+k*15;
  ex_setup(&R,1000+round*31+(int)(frame&255));int r=ex_play();if(r<0)return;total+=r?exScore:0;exScore=total;
  if(!r){int best=total>EXS.endBest;if(best){EXS.endBest=total;EXS.endRounds=round-1;}exs_write();ex_result(cat3("ENDLESS - ROUND ",itoa(round),""),0,0,best,1);return;}
  {int t=0;for(;;){if(t==0){chrome(CAT_SPECIAL,0);m3_box(30,50,180,60,1);m3_ctext(60,cat3("ROUND ",itoa(round)," CLEAR"),C_YEL,2);m3_ctext(84,cat("TOTAL ",itoa(total)),C_WHITE,1);m3_ctext(98,"A: NEXT TASK   B: STAND DOWN",C_GRAY,1);m3_present();}
   BEAT->screen=SCR_EODB;BEAT->mode=99;BEAT->scoreA=total;BEAT->kills=round;next_frame();t++;if(ex_isauto()&&t>60&&round<3)pressed=K_A;else if(ex_isauto()&&t>60)pressed=K_B;if(t>20&&(pressed&K_A)){pressed=0;break;}if(t>20&&(pressed&K_B)){pressed=0;if(total>EXS.endBest){EXS.endBest=total;EXS.endRounds=round;}exs_write();return;}}}}}
static void eod_ops(void){exs_read();int sel=EXS.last<EX_NS?EXS.last:0,redraw=1;bgm_t(TR_EOD);const int N=EX_NS+1;
 for(;;){BEAT->screen=SCR_EODSEL;BEAT->x=sel;
  if(redraw){cat_bgc(CAT_SPECIAL);banner("EOD","BOMB DISPOSAL - SELECT A TASK",C_YEL);m3_box(4,38,232,118,1);int top=sel>8?sel-8:0;
   for(int i=top;i<top+9&&i<N;i++){int y=44+(i-top)*11;m3_text(10,y,i==sel?">":" ",C_YEL,1);
    if(i==EX_NS){m3_text(34,y,"ENDLESS",i==sel?C_YEL:C_WHITE,1);m3_text(140,y,cat("BEST ",itoa(EXS.endBest)),C_GRAY,1);continue;}
    m3_text(18,y,itoa(i+1),C_GRAY,1);m3_text(34,y,ESC[i].n,i==sel?C_YEL:C_WHITE,1);m3_text(150,y,ESC[i].era?"MOD":"WW2",C_GRAY,1);
    for(int s=0;s<3;s++)m3_fill(176+s*8,y+1,6,6,s<EXS.stars[i]?C_YEL:RGB(50,50,60));}
   m3_ctext(146,"A: BRIEFING   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){sel=(sel+N-1)%N;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%N;redraw=1;sfx_menu();}
  if(pressed&K_A){sfx_ok();pressed=0;if(sel==EX_NS)ex_endless();else ex_brief(sel);bgm_t(TR_EOD);redraw=1;}
  if(pressed&K_B){pressed=0;bgCached=-1;return;}}}
