// ======================================================================== v1.3 LRRP - Long Range Recon Patrol, Vietnam 1966-70 (fictional ops)
// 6-man team, top-down stealth across connected zones. Phases: insert, recon (OP/binoculars), intel (photo, sensors, snatch),
// fire direction (arty/air/gunship, adjust, danger close), claymore ambush, booby traps, evasion + hot extraction.
#define SCR_LRSEL 67
#define SCR_LRB 68
#define SCR_LR 69
#define SCR_LRF 70
#define LR_OFF 0xB00
enum{LT_GRASS,LT_TRAIL,LT_JUNGLE,LT_TREE,LT_PADDY,LT_RIVER,LT_HUT,LT_EGRASS,LT_FORD,LT_BUNKER};
enum{Z_LZ,Z_TRAIL,Z_VILLAGE,Z_CAMP,Z_RIVER};
enum{O_OBS,O_PHOTO,O_SENSOR,O_SNATCH,O_FIRE,O_AMBUSH,O_EXFIL};
static const char*const OBN[7]={"OBSERVE","PHOTOGRAPH","PLANT SENSOR","SNATCH PRISONER","CALL FIRE ON","SPRING AMBUSH","EXTRACT"};
typedef struct{u8 t,z,x,y,opt;const char*n;}LObj;
typedef struct{const char*n,*loc,*b1;u8 night,nz,zt[4];u8 no;LObj o[6];u8 ene;}LMis;
static const LMis LM[]={
 {"OP TIGER EYE","A SHAU VALLEY, 1966","WATCH THE TRAIL. COUNT THE TRUCKS.",0,3,{Z_LZ,Z_TRAIL,Z_CAMP},4,{{O_OBS,1,15,4,0,"TRAIL TRAFFIC"},{O_PHOTO,2,20,9,0,"TRUCK PARK"},{O_SENSOR,1,22,9,1,"TRAIL"},{O_EXFIL,0,15,9,0,"LZ"}},3},
 {"BRIGHT LIGHT","KONTUM, 1967","FIND THE CACHE. NOBODY SEES YOU.",0,3,{Z_LZ,Z_RIVER,Z_CAMP},3,{{O_PHOTO,2,16,6,0,"WEAPONS CACHE"},{O_SNATCH,2,22,12,1,"SENTRY"},{O_EXFIL,0,15,9,0,"LZ"}},3},
 {"NIGHT OWL","IA DRANG, 1966","SENSORS ON THE RIDGE TRAIL TONIGHT.",1,3,{Z_LZ,Z_TRAIL,Z_TRAIL},3,{{O_SENSOR,1,12,9,0,"TRAIL NORTH"},{O_SENSOR,2,18,9,0,"TRAIL SOUTH"},{O_EXFIL,0,15,9,0,"LZ"}},3},
 {"STEEL RAIN","QUANG TRI, 1968","MARK THE TRUCK PARK. BRING THE ARTY.",0,3,{Z_LZ,Z_TRAIL,Z_CAMP},3,{{O_OBS,2,6,4,0,"TRUCK PARK"},{O_FIRE,2,20,9,0,"TRUCK PARK"},{O_EXFIL,0,15,9,0,"LZ"}},4},
 {"CLAYMORE ROAD","WAR ZONE C, 1968","SET THE AMBUSH. WAIT FOR THE PATROL.",0,2,{Z_LZ,Z_TRAIL},2,{{O_AMBUSH,1,15,9,0,"TRAIL BEND"},{O_EXFIL,0,15,9,0,"LZ"}},2},
 {"PADDY WALK","MEKONG DELTA, 1969","VILLAGE WATCH. DO NOT HARM CIVILIANS",0,3,{Z_LZ,Z_VILLAGE,Z_RIVER},3,{{O_PHOTO,1,20,6,0,"TAX COLLECTOR"},{O_OBS,2,14,3,0,"FERRY"},{O_EXFIL,0,15,9,0,"LZ"}},2},
 {"SPOOKY","PLEIKU, 1969","CALL THE GUNSHIP ON THE AA SITE.",1,3,{Z_LZ,Z_TRAIL,Z_CAMP},3,{{O_PHOTO,2,12,5,0,"AA SITE"},{O_FIRE,2,12,5,0,"AA SITE"},{O_EXFIL,0,15,9,0,"LZ"}},4},
 {"SNATCH","BINH DINH, 1967","BRING BACK A PRISONER. ALIVE.",0,3,{Z_LZ,Z_TRAIL,Z_CAMP},2,{{O_SNATCH,2,8,12,0,"COURIER"},{O_EXFIL,0,15,9,0,"LZ"}},3},
 {"DANGER CLOSE","KHE SANH, 1968","THEY ARE ALL AROUND. ADJUST FIRE.",0,3,{Z_LZ,Z_CAMP,Z_TRAIL},4,{{O_OBS,1,15,3,0,"BATTALION HQ"},{O_FIRE,1,15,8,0,"BATTALION HQ"},{O_AMBUSH,2,14,9,1,"RETREAT ROUTE"},{O_EXFIL,0,15,9,0,"LZ"}},5},
 {"LAST LIGHT","CAMBODIA BORDER, 1970","EVERYTHING. THEN GET OUT ALIVE.",1,4,{Z_LZ,Z_RIVER,Z_TRAIL,Z_CAMP},5,{{O_SENSOR,1,20,9,0,"FORD"},{O_PHOTO,3,16,6,0,"HQ BUNKER"},{O_FIRE,3,16,6,0,"HQ BUNKER"},{O_AMBUSH,2,14,9,1,"TRAIL"},{O_EXFIL,0,15,9,0,"LZ"}},5},
};
#define LR_NM ((int)(sizeof LM/sizeof LM[0]))
static LMis lrRand;
static const LMis*lrM;static int lrMi;
static u8 lgrid[18][30] EWRAM;
typedef struct{s32 x,y;s16 hp,cd,dir,st,tm,wp,k,nwp;s16 wx[4],wy[4];s16 ix,iy;u8 zone,alive,seen;}LEn; // k:0 NVA 1 VC 2 sentry 3 civ 4 patrol(ambush)
static LEn LE[16] EWRAM;static int nLE;
typedef struct{s16 x,y;u8 zone,hid,typ,hit;}LTr;static LTr LTR[8] EWRAM;static int nLTR;
typedef struct{s16 x,y,on;}LCl;static LCl LCL[3] EWRAM;static int nLCL;
static s16 lrTX[64] EWRAM,lrTY[64] EWRAM;static int lrTH;static s16 lrHP[6] EWRAM;static const char*const LRN[6]={"PT","TL","RTO","M79","AS","RS"};
__attribute__((section(".ewram"))) static s32 lrX,lrY;__attribute__((section(".ewram"))) static int lrZone,lrProne,lrDet,lrComp,lrT,lrScore,lrMsgT,lrHud,lrFail,lrDone,lrWpn,lrM79,lrObsT,lrHuey,lrHueyT,lrHueyX,lrHueyY,lrFace,lrBoard,lrQRF,lrCiv,lrKills,lrPh[5],lrDone_o[6],lrFFx,lrFFy,lrFFt,lrFFk,lrFFeff,lrClack,lrAmbT,lrNight;
static int lr_ghost(void);
static const char*lrMsg;static char lrMsgB[44] EWRAM;
static void lr_msg(const char*s){int k=0;while(s[k]&&k<42){lrMsgB[k]=s[k];k++;}lrMsgB[k]=0;lrMsg=lrMsgB;lrMsgT=180;lrHud=1;}
static u16 lr_n(u16 c){if(!lrNight)return c;int r=c&31,g=(c>>5)&31,b=(c>>10)&31;r=r*35/100;g=g*42/100;b=b*55/100+3;if(b>31)b=31;return r|(g<<5)|(b<<10);}
// ---------------------------------------------------------------- zones
static void lr_genzone(int z){int zt=lrM->zt[z];memset(lgrid,LT_JUNGLE,sizeof lgrid);rs=0x1A5A+lrMi*131+z*977;
 for(int y=0;y<18;y++)for(int x=0;x<30;x++){u32 h=exHash(x+z*31,y+lrMi*7);if((h&7)==0)lgrid[y][x]=LT_TREE;else if((h&7)<3)lgrid[y][x]=LT_EGRASS;}
 // main E-W trail / path so zones connect
 int ty=9;for(int x=0;x<30;x++){int c=ty+(int)(SIN(x*11+z*40)*2>>8);for(int d=0;d<2;d++)if(c+d>=0&&c+d<18)lgrid[c+d][x]=zt==Z_LZ?LT_GRASS:LT_TRAIL;}
 if(zt==Z_LZ){for(int y=4;y<15;y++)for(int x=9;x<22;x++){int dx=x-15,dy=(y-9)*3/2;if(dx*dx+dy*dy<40)lgrid[y][x]=(dx*dx+dy*dy<26)?LT_GRASS:LT_EGRASS;}}
 if(zt==Z_VILLAGE){for(int y=1;y<8;y++)for(int x=2;x<28;x++)lgrid[y][x]=((x/5+y/3)&1)&&y!=4?LT_PADDY:LT_GRASS;for(int k=0;k<5;k++){int x=4+k*5,y=12+(k&1);lgrid[y][x]=LT_HUT;lgrid[y][x+1]=LT_HUT;lgrid[y+1][x]=LT_HUT;lgrid[y+1][x+1]=LT_HUT;for(int a=-1;a<3;a++)for(int b=-1;b<3;b++)if(lgrid[y+b][x+a]!=LT_HUT&&y+b<18)lgrid[y+b][x+a]=LT_GRASS;}}
 if(zt==Z_CAMP){for(int y=3;y<16;y++)for(int x=5;x<26;x++)lgrid[y][x]=LT_GRASS;for(int k=0;k<4;k++){int x=7+k*5,y=k&1?4:13;lgrid[y][x]=LT_HUT;lgrid[y][x+1]=LT_HUT;lgrid[y+1][x]=LT_HUT;lgrid[y+1][x+1]=LT_HUT;}lgrid[6][20]=LT_BUNKER;lgrid[6][21]=LT_BUNKER;lgrid[11][10]=LT_BUNKER;
  for(int y=0;y<18;y++)if(y<8||y>11){lgrid[y][4]=LT_JUNGLE;}}
 if(zt==Z_RIVER){for(int y=0;y<18;y++){int x=14+(int)(SIN(y*13)*3>>8);for(int d=0;d<3;d++)lgrid[y][x+d]=(y>=8&&y<=10)?LT_FORD:LT_RIVER;}}
 if(zt==Z_TRAIL){for(int k=0;k<4;k++){int x=3+k*7,y=2+(int)(exHash(k,z)%4);lgrid[y][x]=LT_TREE;}}
 for(int y=7;y<12;y++){if(lgrid[y][0]==LT_TREE||lgrid[y][0]==LT_RIVER)lgrid[y][0]=LT_JUNGLE;if(lgrid[y][29]==LT_TREE||lgrid[y][29]==LT_RIVER)lgrid[y][29]=LT_JUNGLE;}}
static void lr_tile(int cx,int cy){int x=cx*8,y=16+cy*8,t=lgrid[cy][cx];
 static const u16 GR[3]={RGB(80,120,50),RGB(92,134,58),RGB(70,106,44)};
 for(int j=0;j<8;j+=2)for(int i=0;i<8;i+=2){u32 h=exHash(x+i,y+j);u16 c;
  switch(t){case LT_TRAIL:c=(h&3)?RGB(140,110,70):RGB(120,94,60);break;case LT_JUNGLE:c=(h&3)==0?RGB(20,60,24):(h&3)==1?RGB(34,84,32):RGB(26,72,28);break;case LT_TREE:c=(h&1)?RGB(18,50,20):RGB(28,64,26);break;
   case LT_PADDY:c=(h&3)?RGB(80,120,110):RGB(110,150,90);break;case LT_RIVER:c=(h&3)?RGB(70,90,70):RGB(86,104,80);break;case LT_FORD:c=(h&1)?RGB(96,110,84):RGB(120,120,90);break;
   case LT_EGRASS:c=(h&3)==0?RGB(130,140,60):RGB(100,124,50);break;case LT_HUT:c=(h&1)?RGB(170,140,80):RGB(150,120,64);break;case LT_BUNKER:c=(h&1)?RGB(96,80,56):RGB(80,66,46);break;default:c=GR[h%3];}
  lf(x+i,y+j,2,2,lr_n(c));}
 if(t==LT_TREE){lf(x+2,y+2,4,4,lr_n(RGB(14,40,16)));lf(x+3,y+3,2,2,lr_n(RGB(60,44,28)));}
 if(t==LT_EGRASS&&(cx+cy)&1)lf(x+3,y+1,1,5,lr_n(RGB(150,160,70)));
 if(t==LT_HUT){int top=cy>0&&lgrid[cy-1][cx]!=LT_HUT;if(top)lf(x,y,8,2,lr_n(RGB(200,170,100)));}
 if(t==LT_BUNKER)lf(x+1,y+3,6,2,lr_n(RGB(20,16,12)));}
static int lr_solid(int px,int py){if(px<0||px>=240||py<16||py>=160)return 1;int t=lgrid[(py-16)>>3][px>>3];return t==LT_TREE||t==LT_RIVER||t==LT_HUT||t==LT_BUNKER;}
static int lr_tt(int px,int py){if(px<0||px>=240||py<16||py>=160)return LT_TREE;return lgrid[(py-16)>>3][px>>3];}
static void lr_objart(int i){const LObj*o=&lrM->o[i];if(o->z!=lrZone)return;int x=o->x*8+4,y=16+o->y*8+4;int done=lrDone_o[i];
 switch(o->t){case O_OBS:lf(x-3,y-3,7,1,lr_n(RGB(240,240,120)));lf(x-3,y+3,7,1,lr_n(RGB(240,240,120)));lf(x-3,y-3,1,7,lr_n(RGB(240,240,120)));lf(x+3,y-3,1,7,lr_n(RGB(240,240,120)));break;
  case O_PHOTO:case O_FIRE:if(lrM->zt[lrZone]==Z_CAMP||o->t==O_FIRE){if(done&&o->t==O_FIRE){lf(x-8,y-5,16,10,lr_n(RGB(40,34,30)));break;}
    lf(x-8,y-4,7,6,lr_n(RGB(90,100,60)));lf(x+1,y-4,7,6,lr_n(RGB(90,100,60)));lf(x-7,y-6,5,2,lr_n(RGB(60,70,40)));lf(x+2,y+3,6,3,lr_n(RGB(120,90,50)));}
   else{lf(x-3,y-3,6,6,lr_n(RGB(120,90,50)));lf(x-2,y-2,4,1,lr_n(RGB(150,120,70)));}break;
  case O_SENSOR:if(done){lf(x,y-5,1,6,lr_n(RGB(40,40,40)));lf(x-1,y-6,3,1,lr_n(RGB(60,60,60)));}else{lf(x-3,y,7,1,lr_n(RGB(240,240,120)));lf(x,y-3,1,7,lr_n(RGB(240,240,120)));}break;
  case O_AMBUSH:lf(x-12,y-1,25,1,lr_n(RGB(200,60,40)));break;
  case O_EXFIL:lf(x-6,y-1,13,3,lr_n(RGB(230,230,230)));lf(x-1,y-6,3,13,lr_n(RGB(230,230,230)));break;}}
static void lr_bake(void){u16*sv=lfb;lfb=dl_bg;for(int cy=0;cy<18;cy++)for(int cx=0;cx<30;cx++)lr_tile(cx,cy);for(int i=0;i<lrM->no;i++)lr_objart(i);
 for(int i=0;i<nLTR;i++)if(LTR[i].zone==lrZone&&!LTR[i].hid){int x=LTR[i].x,y=LTR[i].y;for(int k=-3;k<=3;k+=2)lf(x+k,y-2,1,4,lr_n(RGB(200,180,120)));}
 for(int i=0;i<nLCL;i++)if(LCL[i].on==1&&lrZone==lrM->o[0].z+0*0){}lfb=sv;}
// dirty rects
typedef struct{s16 x,y,w,h;}LrR;static LrR lrDirty[48] EWRAM;static int nLrD;
static void lr_dirty(int x,int y,int w,int h){if(nLrD<48){LrR*r=&lrDirty[nLrD++];r->x=x;r->y=y;r->w=w;r->h=h;}}
static void lr_restore(void){for(int i=0;i<nLrD;i++){LrR*r=&lrDirty[i];int x=r->x,y=r->y,w=r->w,h=r->h;if(x<0){w+=x;x=0;}if(y<16){h-=16-y;y=16;}if(x+w>240)w=240-x;if(y+h>160)h=160-y;if(w<=0||h<=0)continue;for(int j=0;j<h;j++)dma16c(dl_bg+(y+j)*240+x,(u16*)0x06000000+(y+j)*240+x,w);}nLrD=0;}
static void lr_full(void){vsync();dma32(dl_bg,(void*)0x06000000,240*160/2);REG_DISPCNT=3|(1<<10);lrHud=1;nLrD=0;}
// sprites
static void lr_man(int x,int y,int prone,int kind,int st,int face){if(prone){lr_dirty(x-5,y-3,11,6);lf(x-4,y-1,9,3,lr_n(kind==9?RGB(50,70,40):RGB(110,100,70)));lf(face?x-5:x+4,y-1,2,2,lr_n(RGB(200,160,120)));return;}
 lr_dirty(x-4,y-10,9,13);u16 b=kind==9?RGB(54,76,42):kind==1||kind==4?RGB(30,30,34):kind==3?RGB(120,110,90):RGB(120,110,70);u16 h=kind==9?RGB(40,56,32):kind==3?RGB(200,180,110):kind==0||kind==4?RGB(140,130,80):RGB(30,30,30);
 lf(x-2,y-6,5,6,lr_n(b));if(kind==9){lf(x-2,y-5,5,1,lr_n(RGB(20,30,16)));lf(x-1,y-3,4,1,lr_n(RGB(20,30,16)));}lf(x-1,y-8,3,2,lr_n(RGB(200,160,120)));
 if(kind==3){lf(x-3,y-9,7,1,lr_n(h));lf(x-1,y-10,3,1,lr_n(h));}else lf(x-2,y-9,5,2,lr_n(h));lf(x-2,y,2,2,lr_n(RGB(40,30,20)));lf(x+1,y,2,2,lr_n(RGB(40,30,20)));
 if(kind!=3){lf(face?x-4:x+2,y-5,3,1,RGB(30,30,30));}
 if(st==1){lf(x,y-10,1,2,C_YEL);}else if(st==2){lf(x,y-10,1,2,C_RED);lf(x-1,y-10,3,1,C_RED);}}
static void lr_huey(int x,int y,int t){lr_dirty(x-22,y-14,45,28);lf(x-8,y-4,16,8,RGB(70,84,50));lf(x+8,y-2,12,3,RGB(70,84,50));lf(x+18,y-4,2,7,RGB(70,84,50));lf(x-10,y-3,3,6,RGB(110,140,160));
 int a=t&3;if(a<2){lf(x-20,y,40,1,RGB(180,180,180));lf(x,y-12,1,24,RGB(180,180,180));}else{for(int k=-14;k<=14;k+=2){lf(x+k,y+k*2/3,2,1,RGB(180,180,180));lf(x+k,y-k*2/3,2,1,RGB(180,180,180));}}}
// ---------------------------------------------------------------- HUD
static int lr_teamn(void){int n=0;for(int i=0;i<6;i++)n+=lrHP[i]>0;return n;}
static void lr_hud(void){u16*sv=lfb;lfb=(u16*)0x06000000;lf(0,0,240,16,RGB(12,16,12));lf(0,15,240,1,RGB(60,80,40));
 int s=lrT/60;ltx(2,1,cat3(itoa(s/60),s%60<10?":0":":",itoa(s%60)),C_WHITE);ltx(38,1,cat3("Z",itoa(lrZone+1),cat("/",itoa(lrM->nz))),C_GRAY);
 for(int i=0;i<6;i++)lf(70+i*6,2,4,5,lrHP[i]>1?C_GREEN:lrHP[i]==1?C_YEL:RGB(70,30,30));
 ltx(110,1,lrComp?"COMPROMISED":"DETECT",lrComp?C_RED:C_GRAY);if(!lrComp){lf(150,2,40,5,RGB(40,40,40));lf(150,2,lrDet*40/1000,5,lrDet>600?C_RED:lrDet>300?C_YEL:C_GREEN);}
 ltx(196,1,lrWpn?cat("M79x",itoa(lrM79)):"CAR-15",C_YEL);
 if(lrMsgT>0)ltx(2,8,lrMsg,C_YEL);else{int nx=-1;for(int i=0;i<lrM->no;i++)if(!lrDone_o[i]&&!lrM->o[i].opt){nx=i;break;}if(nx<0)for(int i=0;i<lrM->no;i++)if(!lrDone_o[i]){nx=i;break;}
  if(nx>=0){const LObj*o=&lrM->o[nx];static char ob[44];int k=0;const char*a=OBN[o->t];while(*a&&k<40)ob[k++]=*a++;ob[k++]=':';ob[k++]=' ';a=o->n;while(*a&&k<36)ob[k++]=*a++;ob[k++]=' ';ob[k++]='Z';ob[k++]='1'+o->z;ob[k]=0;ltx(2,8,ob,o->z==lrZone?C_WHITE:C_GRAY);}}
 lfb=sv;lrHud=0;}
// ---------------------------------------------------------------- sim
static int lr_los(int x0,int y0,int x1,int y1,int range){int dx=x1-x0,dy=y1-y0;int d=idist(dx,dy);if(d>range)return 0;int n=d/4+1;int cost=d;
 for(int i=1;i<n;i++){int x=x0+dx*i/n,y=y0+dy*i/n;int t=lr_tt(x,y);if(t==LT_TREE||t==LT_HUT||t==LT_BUNKER)return 0;if(t==LT_JUNGLE)cost+=7;else if(t==LT_EGRASS)cost+=3;if(cost>range)return 0;}return 1;}
static int lr_member(int i,int*x,int*y){int k=(lrTH-1-i*6)&63;if(i==0){*x=lrX>>8;*y=lrY>>8;return lrHP[0]>0;}*x=lrTX[k];*y=lrTY[k];return lrHP[i]>0;}
static void lr_noise(int x,int y,int r){for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(!e->alive||e->zone!=lrZone||e->st>=2||e->k==3)continue;int dx=(e->x>>8)-x,dy=(e->y>>8)-y;if(idist(dx,dy)<r){e->st=1;e->ix=x;e->iy=y;e->tm=300;}}}
static void lr_compromise(void){if(lrComp)return;lrComp=1;lrDet=1000;lr_msg("COMPROMISED! BREAK CONTACT - RUN FOR THE LZ");for(int i=0;i<nLE;i++)if(LE[i].alive&&LE[i].k!=3)LE[i].st=2;lrQRF=600;lrHud=1;sfx_hurt();}
static LEn*lr_add(int zone,int k,int x,int y){if(nLE>=16)return 0;LEn*e=&LE[nLE++];memset(e,0,sizeof*e);e->zone=zone;e->k=k;e->x=x<<8;e->y=y<<8;e->hp=k==3?1:2;e->alive=1;e->dir=rr(256);e->nwp=0;return e;}
static void lr_setup(int mi){lrMi=mi;lrM=mi<LR_NM?&LM[mi]:&lrRand;lrNight=lrM->night;nLE=0;nLTR=0;nLCL=0;
 for(int z=0;z<lrM->nz;z++){int zt=lrM->zt[z];if(z==0)continue;int ne=lrM->ene;
  for(int k=0;k<ne;k++){int x=60+rr(130),y=30+rr(110);for(int a=0;a<30&&lr_solid(x,y);a++){x=60+rr(130);y=30+rr(110);}LEn*e=lr_add(z,zt==Z_CAMP?(k&1):1,x,y);if(!e)break;
   e->nwp=2+rr(3);for(int w=0;w<e->nwp;w++){int wx=56+rr(140),wy=26+rr(120);e->wx[w]=wx;e->wy[w]=wy;}if(zt==Z_CAMP&&k==0){e->k=2;e->nwp=0;}}
  if(zt==Z_VILLAGE)for(int k=0;k<4;k++){LEn*e=lr_add(z,3,30+k*50,110+rr(20));if(e){e->nwp=2;e->wx[0]=20+k*50;e->wy[0]=100;e->wx[1]=40+k*50;e->wy[1]=140;}}
  for(int k=0;k<2&&nLTR<8;k++){LTr*t=&LTR[nLTR++];t->zone=z;t->hid=1;t->typ=rr(2);t->hit=0;t->x=40+rr(170);t->y=16+9*8+4;}}
 for(int i=0;i<lrM->no;i++){const LObj*o=&lrM->o[i];if(o->t==O_SNATCH){LEn*e=lr_add(o->z,2,o->x*8+4,16+o->y*8+4);if(e){e->k=2;e->dir=0;e->hp=9;e->seen=1;}}
  if(o->t==O_PHOTO&&(o->n[0]=='T'&&o->n[1]=='A')){LEn*e=lr_add(o->z,3,o->x*8+4,16+o->y*8+4);if(e){e->k=3;e->nwp=0;e->seen=2;}}}
 memset(lrDone_o,0,sizeof lrDone_o);for(int i=0;i<6;i++)lrHP[i]=2;lrZone=0;lrX=(15*8+4)<<8;lrY=(16+9*8+4)<<8;for(int k=0;k<64;k++){lrTX[k]=lrX>>8;lrTY[k]=lrY>>8;}lrTH=0;
 lrProne=0;lrDet=0;lrComp=0;lrT=0;lrScore=0;lrMsgT=0;lrFail=lrDone=0;lrWpn=0;lrM79=6;lrObsT=0;lrHuey=0;lrBoard=0;lrQRF=0;lrCiv=0;lrKills=0;memset(lrPh,0,sizeof lrPh);lrFFt=0;lrClack=0;lrAmbT=0;lrFace=0;}
static void lr_enter(int z,int fromLeft){lrZone=z;lr_genzone(z);lrX=(fromLeft?6:233)<<8;int y=lrY>>8;if(lr_solid(lrX>>8,y))y=16+9*8+4;lrY=y<<8;for(int k=0;k<64;k++){lrTX[k]=lrX>>8;lrTY[k]=y;}lr_bake();lr_full();
 lr_msg(cat3("ZONE ",itoa(z+1),lrNight?" - NIGHT":""));}
static int lr_objnear(int t,int r){int x=lrX>>8,y=lrY>>8;for(int i=0;i<lrM->no;i++){const LObj*o=&lrM->o[i];if(o->t!=t||o->z!=lrZone||lrDone_o[i])continue;int ox=o->x*8+4,oy=16+o->y*8+4;if(idist(ox-x,oy-y)<=r)return i;}return -1;}
static void lr_obj_done(int i,int pts,int ph){lrDone_o[i]=1;lrScore+=pts;lrPh[ph]+=pts;sfx_ok();u16*sv=lfb;lfb=dl_bg;const LObj*o=&lrM->o[i];int x=o->x*8,y=16+o->y*8;for(int cy=o->y-2;cy<=o->y+2;cy++)for(int cx=o->x-2;cx<=o->x+2;cx++)if(cx>=0&&cx<30&&cy>=0&&cy<18)lr_tile(cx,cy);lr_objart(i);lfb=sv;
 for(int yy=y-16;yy<y+24;yy++)if(yy>=16&&yy<160){int x0=x-16<0?0:x-16;int w=40;if(x0+w>240)w=240-x0;dma16c(dl_bg+yy*240+x0,(u16*)0x06000000+yy*240+x0,w);}lrHud=1;}
static void lr_kill(LEn*e){e->alive=0;if(e->k==3){lrCiv=1;lrFail=1;lr_msg("CIVILIAN KILLED - MISSION FAILED");return;}lrKills++;}
// fire mission overlay: returns via lrFF* pending strike
static int lr_grid(int x,int y){return (48+lrMi*3+lrZone)*100+x*99/240;}
static void lr_firemission(void){if(lrHP[2]<=0){lr_msg("RTO IS DOWN - NO RADIO");return;}int cx=lrX>>8,cy=(lrY>>8)-30,typ=lrNight?2:0,err=34,shot=0,ex=0,ey=0;u16*V=(u16*)0x06000000;
 for(;;){lr_restore();u16*sv=lfb;lfb=V;lf(0,0,240,16,RGB(10,14,10));static const char*const TY[3]={"ARTY 105MM","AIR: F-4 NAPALM","GUNSHIP: SPOOKY"};ltx(2,1,cat("FIRE MISSION <L R> ",TY[typ]),C_YEL);
  ltx(2,8,cat3("GRID ",itoa(lr_grid(cx,cy)),cat(" ",itoa(4000+(cy-16)*60/144))),C_WHITE);ltx(126,8,shot?"A:ADJ R:FFE B:END":"A:SPOT  B:CANCEL",C_GRAY);
  lf(cx-6,cy,4,1,C_RED);lf(cx+3,cy,4,1,C_RED);lf(cx,cy-6,1,4,C_RED);lf(cx,cy+3,1,4,C_RED);lr_dirty(cx-7,cy-7,15,15);
  if(shot){lf(ex-2,ey-2,5,5,RGB(240,240,240));lr_dirty(ex-3,ey-3,7,7);}
  int dc=0;for(int i=0;i<6;i++){int mx,my;if(lr_member(i,&mx,&my)&&idist(mx-cx,my-cy)<36)dc=1;}if(dc){lf(170,1,70,7,RGB(10,14,10));ltx(178,1,"DANGER CLOSE",C_RED);}
  lfb=sv;next_frame();BEAT->screen=SCR_LRF;BEAT->x=cx;BEAT->y=cy;BEAT->extra=shot|(err<<8);
  if(*(volatile u32*)0x0203FFD0==0x4F545541u){int f=-1;for(int i=0;i<lrM->no;i++)if(lrM->o[i].t==O_FIRE&&!lrDone_o[i])f=i;if(f>=0){int tx=lrM->o[f].x*8+4,ty=16+lrM->o[f].y*8+4;keys=0;pressed=0;if(cx<tx-1)keys|=K_RIGHT;if(cx>tx+1)keys|=K_LEFT;if(cy<ty-1)keys|=K_DOWN;if(cy>ty+1)keys|=K_UP;if(!keys&&(frame&15)==0)pressed=shot&&err<14?K_R:K_A;}}
  if(keys&K_LEFT)cx-=2;if(keys&K_RIGHT)cx+=2;if(keys&K_UP)cy-=2;if(keys&K_DOWN)cy+=2;if(cx<4)cx=4;if(cx>236)cx=236;if(cy<20)cy=20;if(cy>156)cy=156;
  if(pressed&(K_L)){typ=(typ+2)%3;}if(pressed&K_R&&!shot){typ=(typ+1)%3;}
  if(pressed&K_A){shot=1;int a=rr(256);ex=cx+(COS(a)*err>>8);ey=cy+(SIN(a)*err>>8);err=err*55/100;sfx_shot(1);lr_msg("SPLASH - ADJUST");}
  if(pressed&K_R&&shot){pressed=0;lrFFx=cx;lrFFy=cy;lrFFt=150;lrFFk=typ;lrFFeff=err;lr_full();lr_msg(typ==0?"FIRE FOR EFFECT - SHOT OUT":typ==1?"FAST MOVERS INBOUND":"SPOOKY ON STATION");return;}
  if(pressed&K_B){pressed=0;lr_full();return;}}}
static void lr_strike(void){int x=lrFFx,y=lrFFy;int R=lrFFk==1?30:24;sfx_boom();shake(8);
 for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(!e->alive||e->zone!=lrZone)continue;if(idist((e->x>>8)-x,(e->y>>8)-y)<R)lr_kill(e);}
 for(int i=0;i<6;i++){int mx,my;if(lr_member(i,&mx,&my)&&idist(mx-x,my-y)<R-4){lrHP[i]--;lr_msg("FRIENDLY CASUALTY - DANGER CLOSE!");}}
 for(int i=0;i<lrM->no;i++){const LObj*o=&lrM->o[i];if(o->t!=O_FIRE||o->z!=lrZone||lrDone_o[i])continue;int ox=o->x*8+4,oy=16+o->y*8+4;if(idist(ox-x,oy-y)<18){lr_obj_done(i,1500,2);lr_msg("TARGET DESTROYED - GOOD EFFECT");}else lr_msg("NO EFFECT ON TARGET");}
 lr_noise(x,y,200);u16*sv=lfb;lfb=dl_bg;for(int k=0;k<6;k++){int a=rr(256),r=rr(R);lf(x+(COS(a)*r>>8)-3,y+(SIN(a)*r>>8)-2,6,4,lr_n(RGB(40,34,30)));}lfb=sv;
 for(int yy=y-R;yy<y+R;yy++)if(yy>=16&&yy<160){int x0=x-R<0?0:x-R;int w=2*R;if(x0+w>240)w=240-x0;dma16c(dl_bg+yy*240+x0,(u16*)0x06000000+yy*240+x0,w);}lrHud=1;}
static void lr_shoot(int x0,int y0,int x1,int y1,u16 c){u16*sv=lfb;lfb=(u16*)0x06000000;ex_line(x0,y0,x1,y1,c);lfb=sv;lr_dirty(iminv(x0,x1)-1,iminv(y0,y1)-1,iabs(x1-x0)+3,iabs(y1-y0)+3);}
// ---------------------------------------------------------------- enemies
static int lr_sees(LEn*e,int mx,int my){int ex=e->x>>8,ey=e->y>>8;int dx=mx-ex,dy=my-ey;int d=idist(dx,dy);int R=lrNight?34:62;if(d>R)return 0;
 if(d>12){int a=atan2i(dx,dy);int df=(a-e->dir)&255;if(df>128)df=256-df;if(df>30)return 0;}return lr_los(ex,ey,mx,my,R);}
static void lr_emove(LEn*e,int tx,int ty,int sp){int ex=e->x>>8,ey=e->y>>8;int dx=tx-ex,dy=ty-ey;if(iabs(dx)+iabs(dy)<2)return;e->dir=atan2i(dx,dy);
 s32 nx=e->x+(SIN(e->dir)*sp>>8),ny=e->y-(COS(e->dir)*sp>>8);if(!lr_solid(nx>>8,ny>>8)){e->x=nx;e->y=ny;}else{e->dir=(e->dir+64)&255;s32 ax=e->x+(SIN(e->dir)*sp>>8),ay=e->y-(COS(e->dir)*sp>>8);if(!lr_solid(ax>>8,ay>>8)){e->x=ax;e->y=ay;}}}
static void lr_enemies(void){int seenAny=0;
 for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(!e->alive||e->zone!=lrZone)continue;int ex=e->x>>8,ey=e->y>>8;
  if(e->st==0){if(e->nwp){int tx=e->wx[e->wp],ty=e->wy[e->wp];lr_emove(e,tx,ty,e->k==3?60:90);if(idist(tx-ex,ty-ey)<4){e->wp=(e->wp+1)%e->nwp;}}else if((lrT&127)==0&&e->k!=3)e->dir=(e->dir+(e->seen==1?0:48))&255;}
  else if(e->st==1){lr_emove(e,e->ix,e->iy,110);if(--e->tm<=0)e->st=0;}
  else if(e->st==2&&e->k!=3){int bi=-1,bd=999,bx=0,by=0;for(int m=0;m<6;m++){int mx,my;if(!lr_member(m,&mx,&my))continue;int d=idist(mx-ex,my-ey);if(d<bd){bd=d;bi=m;bx=mx;by=my;}}
   if(bi>=0){if(bd>44)lr_emove(e,bx,by,120);else e->dir=atan2i(bx-ex,by-ey);
    if(--e->cd<=0){e->cd=50+rr(30);if(bd<96&&lr_los(ex,ey,bx,by,110)){lr_shoot(ex,ey-4,bx,by-4,RGB(255,220,120));sfx_shot(0);if(rr(100)<(lrNight?14:22)){lrHP[bi]--;lrHud=1;if(lrHP[bi]<=0)lr_msg(cat(LRN[bi]," IS DOWN!"));}}}}}
  // detection
  if(e->st<2){for(int m=0;m<6;m++){int mx,my;if(!lr_member(m,&mx,&my))continue;if(!lr_sees(e,mx,my))continue;seenAny=1;int t=lr_tt(mx,my);int c=t==LT_JUNGLE?35:t==LT_EGRASS?60:100;if(lrProne)c/=2;
     if(keys&0xF0)c=c*3/2;int d=idist(mx-ex,my-ey);int R=lrNight?34:62;int add=(R-d+8)*c/900;if(add<1)add=1;if(lr_ghost())add=0;if(e->k==2&&e->seen==1)add=add*3/2;lrDet+=add;if(lrDet>500&&e->k!=3){e->st=1;e->ix=mx;e->iy=my;e->tm=200;}break;}}}
 if(!seenAny&&lrDet>0&&!lrComp)lrDet--;
 if(lrDet>=1000&&!lrComp)lr_compromise();
 if(lrComp&&lrQRF>0&&--lrQRF==0){lrQRF=720;for(int k=0;k<2;k++){LEn*e=0;for(int j=0;j<nLE;j++)if(!LE[j].alive&&LE[j].k!=3&&!(LE[j].k==2&&LE[j].seen==1)){e=&LE[j];break;}if(!e&&nLE<16)e=&LE[nLE++];if(!e)break;
   memset(e,0,sizeof*e);e->zone=lrZone;e->k=0;e->alive=1;e->hp=2;e->st=2;e->x=(k?236:4)<<8;e->y=(16+9*8+4)<<8;e->cd=90;}lr_msg("ENEMY QRF CLOSING IN");}}
static LEn*lr_target(int x,int y,int R){LEn*b=0;int bd=R;for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(!e->alive||e->zone!=lrZone||e->k==3)continue;int d=idist((e->x>>8)-x,(e->y>>8)-y);if(d<bd&&lr_los(x,y,e->x>>8,e->y>>8,R+20)){bd=d;b=e;}}return b;}
static void lr_team_fire(void){static s16 cd[6];for(int m=1;m<6;m++){int mx,my;if(!lr_member(m,&mx,&my))continue;if(--cd[m]>0)continue;cd[m]=40+rr(25);LEn*e=lr_target(mx,my,84);if(!e||e->st<2)continue;
  int ex=e->x>>8,ey=e->y>>8;if(m==3&&rr(4)==0){lr_shoot(mx,my-4,ex,ey,RGB(255,160,80));for(int j=0;j<nLE;j++){LEn*o=&LE[j];if(o->alive&&o->zone==lrZone&&o->k!=3&&idist((o->x>>8)-ex,(o->y>>8)-ey)<12)lr_kill(o);}sfx_boom();continue;}
  lr_shoot(mx,my-4,ex,ey-4,RGB(255,255,160));sfx_shot(0);if(rr(100)<30){if(--e->hp<=0)lr_kill(e);}}}
static void lr_player_fire(void){static int cd;if(cd>0){cd--;return;}if(!(pressed&K_A)&&!(lrWpn==0&&(keys&K_A)&&lrComp))return;int x=lrX>>8,y=lrY>>8;if(!lrComp)for(int i=0;i<nLE;i++){LEn*c=&LE[i];if(c->alive&&c->k==2&&c->seen==1&&c->zone==lrZone&&idist((c->x>>8)-x,(c->y>>8)-y)<18){lr_msg("SNATCH: GET BEHIND HIM - HOLD FIRE");return;}}LEn*e=lr_target(x,y,96);
 if(lrWpn==1){if(lrM79<=0){lr_msg("OUT OF 40MM");return;}lrM79--;cd=60;int tx=e?e->x>>8:x+(lrFace?-60:60),ty=e?e->y>>8:y;lr_shoot(x,y-4,tx,ty,RGB(255,160,80));sfx_boom();
  for(int j=0;j<nLE;j++){LEn*o=&LE[j];if(o->alive&&o->zone==lrZone&&idist((o->x>>8)-tx,(o->y>>8)-ty)<13)lr_kill(o);}lr_noise(x,y,150);lrHud=1;return;}
 cd=12;if(!e){lr_noise(x,y,140);return;}int ex=e->x>>8,ey=e->y>>8;lr_shoot(x,y-4,ex,ey-4,RGB(255,255,160));sfx_shot(0);lr_noise(x,y,140);if(e->st<2){lr_compromise();}
 if(rr(100)<(lrWpn==0?50:50)){if(--e->hp<=0)lr_kill(e);}}
// ---------------------------------------------------------------- autopilot (test only)
static u8 lrBD[18][30] EWRAM;
static int lr_danger(int px,int py,int gx,int gy){for(int i=0;i<nLTR;i++){LTr*tr=&LTR[i];if(tr->zone==lrZone&&!tr->hit&&!tr->hid&&idist(tr->x-px,tr->y-py)<7)return 1;}int R=(lrNight?34:62)+6;if(idist(px-(gx*8+4),py-(16+gy*8+4))<20)return 0;
 for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(!e->alive||e->zone!=lrZone||e->st>=2||e->k==3)continue;int ex=e->x>>8,ey=e->y>>8;int d=idist(px-ex,py-ey);if(d>R)continue;if(d<18)return 1;
  int a=atan2i(px-ex,py-ey);int df=(a-e->dir)&255;if(df>128)df=256-df;if(df<=40||(e->nwp&&d<40))return 1;}return 0;}
static void lr_cat2(char*b,const char*a,int x,const char*c,int y){int n=0;while(*a)b[n++]=*a++;const char*q=itoa(x);while(*q)b[n++]=*q++;while(*c)b[n++]=*c++;q=itoa(y);while(*q)b[n++]=*q++;b[n]=0;}
static int lrAvoid=1;
static int lr_danger2(int px,int py,int gx,int gy){if(lrAvoid)return lr_danger(px,py,gx,gy);for(int i=0;i<nLTR;i++){LTr*tr=&LTR[i];if(tr->zone==lrZone&&!tr->hit&&!tr->hid&&idist(tr->x-px,tr->y-py)<7)return 1;}return 0;}
static void lr_bfs0(int gx,int gy);
static void lr_bfs(int gx,int gy){lrAvoid=!lrComp;lr_bfs0(gx,gy);int x=lrX>>8,y=lrY>>8;int cx=iminv(x>>3,29),cy=(y-16)>>3;if(cy<0)cy=0;if(cy>17)cy=17;if(lrAvoid&&lrBD[cy][cx]==255){lrAvoid=0;lr_bfs0(gx,gy);}}
static void lr_bfs0(int gx,int gy){static u8 q[1100] EWRAM;memset(lrBD,255,sizeof lrBD);int h=0,t=0;if(gx<0)gx=0;if(gx>29)gx=29;if(gy<0)gy=0;if(gy>17)gy=17;lrBD[gy][gx]=0;q[t++]=gx;q[t++]=gy;
 while(h<t){int x=q[h++],y=q[h++];static const s8 DX[4]={1,-1,0,0},DY[4]={0,0,1,-1};for(int k=0;k<4;k++){int nx=x+DX[k],ny=y+DY[k];if(nx<0||nx>=30||ny<0||ny>=18||lrBD[ny][nx]!=255)continue;int px=nx*8+4,py=16+ny*8+4;if(lr_solid(px,py)||lr_solid(px-3,py)||lr_solid(px+3,py))continue;if(lr_danger2(px,py,gx,gy))continue;lrBD[ny][nx]=lrBD[y][x]+1;if(t<1098){q[t++]=nx;q[t++]=ny;}}}}
static int lr_snd(int x,int y){int b=999;for(int i=0;i<nLE;i++){LEn*c=&LE[i];if(c->alive&&c->k==2&&c->seen==1&&c->zone==lrZone){int d=idist((c->x>>8)-x,(c->y>>8)-y);if(d<b)b=d;}}return b;}
static int lr_isauto(void){u32 v=*(volatile u32*)0x0203FFD0;return v==0x4F545541u||v==0x53545541u;}
static int lr_ghost(void){return *(volatile u32*)0x0203FFD0==0x53545541u;}
static void lr_auto(void){if(!lr_isauto())return;int x=lrX>>8,y=lrY>>8;int k=0;pressed&=~(K_A|K_B|K_SEL|K_L);
 if(lrHuey==3)return;
 int oi=-1;for(int i=0;i<lrM->no;i++)if(!lrDone_o[i]&&lrM->o[i].t!=O_EXFIL&&!(lrComp&&lrM->o[i].t==O_SNATCH)&&!(lrM->o[i].opt&&(lrComp||lr_teamn()<5))){oi=i;break;}if(oi<0)for(int i=0;i<lrM->no;i++)if(lrM->o[i].t==O_EXFIL)oi=i;
 const LObj*o=&lrM->o[oi];int gx=o->x*8+4,gy=16+o->y*8+4;int near=8;
 if(o->t==O_PHOTO)near=26;if(o->t==O_FIRE)near=60;if(o->t==O_SNATCH){for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(e->alive&&e->k==2&&e->seen==1&&e->zone==o->z){gx=(e->x>>8)-(SIN(e->dir)*9>>8);gy=(e->y>>8)+(COS(e->dir)*9>>8);if(lr_solid(gx,gy)){gx=(e->x>>8)-(SIN(e->dir)*6>>8);gy=(e->y>>8)+(COS(e->dir)*6>>8);}}}near=3;}
 if(o->t==O_SNATCH&&o->z==lrZone&&lr_snd(x,y)<11&&(lrT&15)==0){for(int i=0;i<nLE;i++){LEn*c=&LE[i];if(c->alive&&c->k==2&&c->seen==1&&c->zone==lrZone){int df=(atan2i(x-(c->x>>8),y-(c->y>>8))-c->dir)&255;if(df>128)df=256-df;if(df>56)pressed|=K_A;}}}
 if(o->t==O_EXFIL&&lrHuey==2){gx=lrHueyX;gy=lrHueyY+6;near=4;}
 if(o->z!=lrZone){gy=16+9*8+4;gx=o->z>lrZone?239:0;near=0;}
 if(lrComp){LEn*e=lr_target(x,y,90);if(e&&(lrT&7)==0)pressed|=K_A;}
 {static int bg=-1;int gc=(gx>>3)|(((gy-16)>>3)<<5)|(lrZone<<10);if(gc!=bg||(lrT&31)==0){lr_bfs(iminv(gx>>3,29),(gy-16)>>3);bg=gc;}}
 int dx=gx-x,dy=gy-y;int d=idist(dx,dy);
 if(d>near){int ccx=x>>3,ccy=(y-16)>>3;int here=lrBD[ccy][ccx];if(here!=255&&here>0){static const s8 DX[4]={1,-1,0,0},DY[4]={0,0,1,-1};int bk=-1,bv=here;for(int q=0;q<4;q++){int nx=ccx+DX[q],ny=ccy+DY[q];if(nx<0||nx>=30||ny<0||ny>=18)continue;if(lrBD[ny][nx]<bv){bv=lrBD[ny][nx];bk=q;}}
   if(bk>=0){int nx=(ccx+DX[bk])*8+4,ny=16+(ccy+DY[bk])*8+4;dx=nx-x;dy=ny-y;if(DX[bk])dy=(16+ccy*8+4)-y;else dx=(ccx*8+4)-x;}}
  if(iabs(dx)>1)k|=dx>0?K_RIGHT:K_LEFT;if(iabs(dy)>1)k|=dy>0?K_DOWN:K_UP;if(o->z!=lrZone&&iabs(dy)<=1)k|=gx>x?K_RIGHT:K_LEFT;if(lrProne&&(lrT&15)==0)pressed|=K_B;}
 else{switch(o->t){case O_OBS:k|=K_A;break;case O_PHOTO:case O_SNATCH:if((lrT&15)==0)pressed|=K_A;break;case O_SENSOR:if((lrT&15)==0)pressed|=K_SEL;break;case O_FIRE:if((lrT&31)==0)pressed|=K_L;break;
  case O_AMBUSH:if(nLCL<3){if((lrT&31)==0){pressed|=K_SEL;}if((lrT&31)==16)k|=nLCL&1?K_RIGHT:K_LEFT;}else{if(!lrProne&&(lrT&15)==0)pressed|=K_B;
   for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(e->alive&&e->k==4&&e->zone==lrZone)for(int c=0;c<nLCL;c++)if(idist((e->x>>8)-LCL[c].x,(e->y>>8)-LCL[c].y)<16)pressed|=K_A;}}break;
  case O_EXFIL:if(!lrHuey&&(lrT&15)==0)pressed|=K_SEL;break;}}
 keys=(keys&~0x1F0)|k|(k&K_A);}
// ---------------------------------------------------------------- update
static void lr_update(void){lrT++;lr_auto();int x=lrX>>8,y=lrY>>8;if(lrMsgT>0&&--lrMsgT==0)lrHud=1;if((lrT%60)==0)lrHud=1;
 if(lrHuey==3){if(++lrHueyT>150){lrHuey=0;lr_msg("INSERTED. HUEY IS GONE. YOU ARE ALONE.");}return;}
 if(pressed&K_B&&!(lrM->o[0].t==9)){lrProne^=1;lrHud=1;}
 int sprint=(keys&K_R)&&!lrProne;int t=lr_tt(x,y);int sp=lrProne?60:sprint?280:150;if(t==LT_JUNGLE||t==LT_PADDY||t==LT_FORD)sp=sp*2/3;
 int dx=0,dy=0;if(keys&K_LEFT){dx=-sp;lrFace=1;}if(keys&K_RIGHT){dx=sp;lrFace=0;}if(keys&K_UP)dy=-sp;if(keys&K_DOWN)dy=sp;
 if(lr_isauto()==0||1){s32 nx=lrX+dx,ny=lrY+dy;if(!lr_solid((nx>>8)-2,y)&&!lr_solid((nx>>8)+2,y)&&(nx>>8)>=0&&(nx>>8)<240)lrX=nx;if(!lr_solid(lrX>>8,(ny>>8)-2)&&!lr_solid(lrX>>8,(ny>>8)+1))lrY=ny;}
 x=lrX>>8;y=lrY>>8;
 if(x!=lrTX[(lrTH-1)&63]||y!=lrTY[(lrTH-1)&63]){if(idist(x-lrTX[(lrTH-1)&63],y-lrTY[(lrTH-1)&63])>=2){lrTX[lrTH&63]=x;lrTY[lrTH&63]=y;lrTH++;}}
 int moving=(dx|dy)!=0;
 if(moving&&(lrT%30)==0){int r=sprint?56:lrProne?6:12;if(t==LT_PADDY||t==LT_FORD)r*=2;lr_noise(x,y,r);}
 // zone change
 if(x>=237&&lrZone<lrM->nz-1){lr_enter(lrZone+1,1);return;}if(x<=2&&lrZone>0){lr_enter(lrZone-1,0);return;}
 // traps
 for(int i=0;i<nLTR;i++){LTr*tr=&LTR[i];if(tr->zone!=lrZone||tr->hit)continue;int d=idist(tr->x-x,tr->y-y);
  if(tr->hid&&d<11&&!sprint){tr->hid=0;lrScore+=100;lrPh[4]+=100;lr_msg(tr->typ?"TRIPWIRE GRENADE - AVOID":"PUNJI PIT - AVOID");u16*sv=lfb;lfb=dl_bg;for(int k=-3;k<=3;k+=2)lf(tr->x+k,tr->y-2,1,4,lr_n(RGB(200,180,120)));lfb=sv;for(int yy=tr->y-3;yy<tr->y+3;yy++)dma16c(dl_bg+yy*240+tr->x-4,(u16*)0x06000000+yy*240+tr->x-4,9);}
  for(int m=0;m<6;m++){int mx,my;if(!lr_member(m,&mx,&my))continue;if(idist(tr->x-mx,tr->y-my)<3&&(tr->hid||sprint)){tr->hit=1;lrHP[m]--;lr_msg(cat(LRN[m],tr->typ?" HIT A TRIPWIRE GRENADE":" FELL IN A PUNJI PIT"));sfx_hurt();if(tr->typ){sfx_boom();lr_noise(mx,my,90);}lrHud=1;break;}}}
 // leader death -> next takes point
 if(lrHP[0]<=0){for(int m=1;m<6;m++)if(lrHP[m]>0){lrHP[0]=lrHP[m];lrHP[m]=0;lr_msg(cat(LRN[m]," TAKES POINT"));break;}}
 if(lr_teamn()==0){lrFail=1;lr_msg("TEAM LOST");return;}
 lr_enemies();if(lrComp)lr_team_fire();
 if(lrFFt>0){if(--lrFFt==0){lrFFx+=rr(lrFFeff*2+1)-lrFFeff;lrFFy+=rr(lrFFeff*2+1)-lrFFeff;lr_strike();}}
 // actions
 int still=!(keys&0xF0);
 int oo=lr_objnear(O_OBS,10);if(oo>=0&&(keys&K_A)&&still){lrObsT++;if(lrObsT%30==0){lrMsgT=0;lr_msg(cat3("OBSERVING ",itoa(lrObsT*100/240),"%"));}if(lrObsT>=240){lrObsT=0;lr_obj_done(oo,800,0);lr_msg(cat("LOGGED: ",lrM->o[oo].n));}}
 else if(pressed&K_A){int a=lr_objnear(O_PHOTO,40);if(a>=0){lr_obj_done(a,600,1);lr_msg("PHOTO TAKEN");pressed&=~K_A;}
  int s=lr_objnear(O_SNATCH,240);if(s>=0&&!lrComp){for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(!e->alive||e->k!=2||e->seen!=1||e->zone!=lrZone)continue;int ex=e->x>>8,ey=e->y>>8;int df=(atan2i(x-ex,y-ey)-e->dir)&255;if(df>128)df=256-df;if(idist(ex-x,ey-y)<12&&df>56){e->alive=0;e->st=4;lr_obj_done(s,1500,1);lr_msg("PRISONER SNATCHED - KEEP HIM ALIVE");pressed&=~K_A;}}}
  if(lrClack&&(pressed&K_A)){lrClack=0;pressed&=~K_A;sfx_boom();shake(6);int kills=0;for(int c=0;c<nLCL;c++){for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(e->alive&&e->zone==lrZone&&e->k!=3&&idist((e->x>>8)-LCL[c].x,(e->y>>8)-LCL[c].y)<28){lr_kill(e);kills++;}}LCL[c].on=2;}
   lrScore+=kills*150;lrPh[3]+=kills*150;for(int i=0;i<nLE;i++)if(LE[i].alive&&LE[i].zone==lrZone&&LE[i].k==4){if((LE[i].x>>8)<6)LE[i].alive=0;else LE[i].st=2;}lr_noise(x,y,200);lr_compromise();lr_msg(cat3("CLAYMORES! ",itoa(kills)," KIA - IN CONTACT"));}}
 if(pressed&K_SEL){int s=lr_objnear(O_SENSOR,10);int am=lr_objnear(O_AMBUSH,30);int ex=lr_objnear(O_EXFIL,16);
  if(s>=0){lr_obj_done(s,500,1);lr_msg("SENSOR PLANTED");}
  else if(am>=0&&nLCL<3){LCl*c=&LCL[nLCL++];c->x=x+(lrFace?-6:6);c->y=y-4;c->on=1;lr_msg(cat3("CLAYMORE ",itoa(nLCL),"/3 SET"));sfx_menu();}
  else if(ex>=0&&!lrHuey){lrHuey=1;lrHueyT=0;lr_msg(lrNight?"STROBE ON - HUEY INBOUND":"SMOKE OUT - HUEY INBOUND");}
  else{lrWpn^=1;lrHud=1;}}
 if(pressed&K_L){if(lrFFt>0)lr_msg("ROUNDS IN FLIGHT - WAIT");else{lr_firemission();return;}}
 lr_player_fire();
 // ambush patrol
 {int am=-1;for(int i=0;i<lrM->no;i++)if(lrM->o[i].t==O_AMBUSH&&lrM->o[i].z==lrZone&&!lrDone_o[i])am=i;
  if(am>=0){int pat=0;for(int i=0;i<nLE;i++)if(LE[i].k==4&&LE[i].zone==lrZone&&LE[i].alive)pat++;
   if(nLCL>=2&&lrProne&&!lrAmbT&&!pat){lrAmbT=1;lr_msg("TEAM IN POSITION. WAIT FOR THEM...");}
   if(lrAmbT>0&&lrAmbT<9999){if(++lrAmbT>240){lrAmbT=9999;lrClack=1;for(int k=0;k<5;k++){LEn*e=lr_add(lrZone,4,2+k*0,16+9*8+4);if(!e)break;e->x=(-k*14)<<8;e->nwp=1;e->wx[0]=250;e->wy[0]=16+9*8+4;}lr_msg("PATROL ON THE TRAIL - A: CLACKER");}}
   if(lrAmbT==9999){int alive=0,gone=0;for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(e->k!=4||e->zone!=lrZone)continue;if(e->alive){alive++;if((e->x>>8)>238){e->alive=0;gone++;}}}
    if(!alive){lrDone_o[am]=1;if(lrPh[3]>0){lrScore+=1000;lrPh[3]+=1000;lr_msg("AMBUSH COMPLETE");}else lr_msg("THE PATROL GOT AWAY");lrHud=1;lrClack=0;}}}}
 // extraction
 if(lrHuey==1){if(++lrHueyT>480){lrHuey=2;int i=-1;for(int k=0;k<lrM->no;k++)if(lrM->o[k].t==O_EXFIL)i=k;lrHueyX=lrM->o[i].x*8+4;lrHueyY=16+lrM->o[i].y*8+4-12;lr_msg("HUEY ON THE GROUND - GET IN!");}}
 if(lrHuey==2){if(idist(x-lrHueyX,y-(lrHueyY+6))<14){lrDone=1;int i=-1;for(int k=0;k<lrM->no;k++)if(lrM->o[k].t==O_EXFIL)i=k;lrDone_o[i]=1;int sv=lr_teamn();int b=1000+sv*200+(lrComp?0:800);lrScore+=b;lrPh[4]+=b;}
  if(lrComp&&(lrT%30)==0){LEn*e=lr_target(lrHueyX,lrHueyY,110);if(e){lr_shoot(lrHueyX-6,lrHueyY,e->x>>8,e->y>>8,RGB(255,255,160));if(rr(2))lr_kill(e);}}}}
static void lr_draw(void){lr_restore();if(lrHud)lr_hud();u16*sv=lfb;lfb=(u16*)0x06000000;
 for(int c=0;c<nLCL;c++)if(LCL[c].on==1&&lrZone==lrM->o[0].z+0){}
 for(int c=0;c<nLCL;c++)if(LCL[c].on==1){lf(LCL[c].x-2,LCL[c].y-1,5,3,RGB(70,90,50));lf(LCL[c].x-2,LCL[c].y-2,5,1,RGB(40,50,30));lr_dirty(LCL[c].x-3,LCL[c].y-3,7,6);}
 for(int i=0;i<nLE;i++){LEn*e=&LE[i];if(!e->alive||e->zone!=lrZone)continue;int ex=e->x>>8,ey=e->y>>8;lr_man(ex,ey,0,e->k==4?0:e->k==2?0:e->k,e->k==3?0:e->st,SIN(e->dir)<0);
  if(lrNight&&!lr_los(lrX>>8,lrY>>8,ex,ey,40)){} }
 if(lrHuey!=3)for(int m=5;m>=0;m--){int mx,my;if(lr_member(m,&mx,&my))lr_man(mx,my,lrProne,9,0,lrFace);}
 if(lrFFt>0&&lrFFt<20){lf(lrFFx-10,lrFFy-10,20,20,RGB(255,200,90));lr_dirty(lrFFx-11,lrFFy-11,22,22);}
 if(lrHuey==1&&lrHueyT>380){int i=0;for(int k=0;k<lrM->no;k++)if(lrM->o[k].t==O_EXFIL)i=k;int tx=lrM->o[i].x*8+4,ty=16+lrM->o[i].y*8+4-12;int f=480-lrHueyT;lr_huey(tx-f*2,ty-f/3,lrT);}
 if(lrHuey==2)lr_huey(lrHueyX,lrHueyY,lrT);
 if(lrHuey==3){int f=lrHueyT;int hx=f<90?-20+f*2:f<120?160:160+(f-120)*4,hy=16+9*8-8;lr_huey(hx,hy,lrT);}
 if(lrNight&&lrHuey==1){int i=0;for(int k=0;k<lrM->no;k++)if(lrM->o[k].t==O_EXFIL)i=k;if(lrT&8){int sx=lrM->o[i].x*8+4,sy=16+lrM->o[i].y*8+4;lf(sx-1,sy-1,3,3,C_WHITE);lr_dirty(sx-2,sy-2,5,5);}}
 lfb=sv;}
// ---------------------------------------------------------------- save / menus
typedef struct{u16 best[LR_NM+1];u8 stars[LR_NM+1];u8 last,pad;u16 snatch,sensors,kia,patrols;}LrSave;static LrSave LRS EWRAM;
static void lrs_write(void){const u8*b=(const u8*)&LRS;u8 s=0x4C;SRAM[LR_OFF]='A';SRAM[LR_OFF+1]='O';SRAM[LR_OFF+2]='E';SRAM[LR_OFF+3]='L';SRAM[LR_OFF+4]=1;for(int i=0;i<(int)sizeof LRS;i++){SRAM[LR_OFF+5+i]=b[i];s+=b[i]*(i+1);}SRAM[LR_OFF+5+sizeof LRS]=s;}
static void lrs_read(void){memset(&LRS,0,sizeof LRS);if(SRAM[LR_OFF]!='A'||SRAM[LR_OFF+1]!='O'||SRAM[LR_OFF+2]!='E'||SRAM[LR_OFF+3]!='L'||SRAM[LR_OFF+4]!=1)return;u8 b[sizeof LRS];u8 s=0x4C;for(int i=0;i<(int)sizeof LRS;i++){b[i]=SRAM[LR_OFF+5+i];s+=b[i]*(i+1);}if(s==SRAM[LR_OFF+5+sizeof LRS])memcpy(&LRS,b,sizeof LRS);}
static void rec_lrrp(const char**L){static char b[5][40];lrs_read();int n=0,st=0;for(int i=0;i<LR_NM;i++){n+=LRS.stars[i]>0;st+=LRS.stars[i];}
 char sb[48];for(int k=0;k<5;k++){const char*a=k==0?cat3("MISSIONS COMPLETE: ",itoa(n),"/10"):k==1?cat3("STARS: ",itoa(st),"/30"):k==2?(lr_cat2(sb,"PRISONERS: ",LRS.snatch,"  SENSORS: ",LRS.sensors),sb):k==3?(lr_cat2(sb,"PATROLS: ",LRS.patrols,"  BEST ",LRS.best[LR_NM]),sb):"TEAM: CAR-15 M16A1 M79 PRC-25";int j=0;while(a[j]&&j<38){b[k][j]=a[j];j++;}b[k][j]=0;L[k]=b[k];}}
static int lr_play(void){lr_enter(0,1);lrX=(15*8+4)<<8;lrY=(16+9*8+4)<<8;for(int k=0;k<64;k++){lrTX[k]=lrX>>8;lrTY[k]=lrY>>8;}lrHuey=3;lrHueyT=0;bgm_t(TR_LRRP);int endT=0;
 for(;;){BEAT->screen=SCR_LR;BEAT->mode=lrMi;BEAT->hp=lr_teamn();BEAT->kills=lrKills;BEAT->scoreA=lrScore;BEAT->phase=lrFail?5:lrDone?4:lrComp?2:0;BEAT->x=lrX>>8;BEAT->y=lrY>>8;BEAT->timer=lrT;
  {int d=0;for(int i=0;i<lrM->no;i++)d|=lrDone_o[i]<<i;BEAT->extra=d|(lrZone<<8)|(lrDet<<12)|(lrHuey<<24);}
  vsync();lr_draw();frame++;BEAT->frame=frame;poll();
  if(!lrFail&&!lrDone)lr_update();else endT++;if(endT>90)break;
  if(pressed&K_START){int q=0;u16*V=(u16*)0x06000000;u16*sv=lfb;lfb=V;lf(50,66,140,28,RGB(16,20,16));ltc(70,"PAUSED",C_YEL);ltc(82,"START: RESUME  B: ABORT",C_WHITE);lfb=sv;for(;;){next_frame();if(pressed&K_START)break;if(pressed&K_B){q=1;break;}}pressed=0;lr_full();if(q)return -1;}}
 if(lrFail){music_defeat();return 0;}return 1;}
static int lr_missed(void){int m=0;for(int i=0;i<lrM->no;i++)if(!lrDone_o[i]&&!lrM->o[i].opt&&lrM->o[i].t!=O_EXFIL)m++;return m;}
static void lr_result(int ok,int stars,int best){int t=0;for(;;){BEAT->screen=SCR_LR;BEAT->phase=ok?4:5;BEAT->scoreA=lrScore;BEAT->scoreB=stars;
  if(t==0){chrome(CAT_SPECIAL,0);m3_box(12,16,216,132,1);m3_ctext(22,ok?"TEAM EXTRACTED":"MISSION FAILED",ok?C_YEL:C_RED,2);m3_ctext(40,ok&&lr_missed()?"PRIMARY OBJECTIVE MISSED":lrM->n,ok&&lr_missed()?C_RED:C_WHITE,1);
   static const char*const PN[5]={"RECON","INTEL","FIRE DIRECTION","AMBUSH","EVASION/EXTRACT"};for(int p=0;p<5;p++){m3_text(30,54+p*10,PN[p],C_GRAY,1);m3_text(150,54+p*10,itoa(lrPh[p]),C_WHITE,1);}
   {char b[32];int n=0;const char*q="TEAM ";while(*q)b[n++]=*q++;b[n++]='0'+lr_teamn();q="/6   KIA ";while(*q)b[n++]=*q++;q=itoa(lrKills);while(*q)b[n++]=*q++;b[n]=0;m3_text(30,106,b,C_WHITE,1);}m3_text(30,116,cat3("SCORE ",itoa(lrScore),best?"  BEST!":""),best?C_GREEN:C_WHITE,1);
   if(ok)for(int s=0;s<3;s++)m3_fill(150+s*14,116,10,10,s<stars?C_YEL:RGB(50,50,60));m3_ctext(134,"A: CONTINUE",C_GRAY,1);m3_present();}
  next_frame();t++;if(lr_isauto()&&t>300)pressed=K_A;if(t>20&&(pressed&(K_A|K_B))){pressed=0;return;}}}
static const char*const RNN[6]={"RANDOM PATROL","SOUTH OF THE DMZ","CENTRAL HIGHLANDS","III CORPS, 1969","THE DELTA","LAOS BORDER"};
static void lr_genrand(void){LMis*R=&lrRand;rs^=frame*2654435761u;R->n=RNN[0];R->loc=RNN[1+rr(5)];R->b1="RANDOM TASKING. EXPECT ANYTHING.";R->night=rr(3)==0;R->nz=3;R->zt[0]=Z_LZ;R->zt[1]=1+rr(4);R->zt[2]=1+rr(4);R->ene=2+rr(3);
 static const u8 TT[5]={O_OBS,O_PHOTO,O_SENSOR,O_FIRE,O_AMBUSH};int n=2+rr(2);R->no=0;for(int i=0;i<n;i++){LObj*o=&R->o[R->no++];o->t=TT[rr(5)];o->z=1+rr(2);o->x=8+rr(14);o->y=o->t==O_AMBUSH||o->t==O_SENSOR?9:3+rr(10);o->opt=0;o->n=o->t==O_FIRE?"SUPPLY DUMP":o->t==O_AMBUSH?"TRAIL":"TARGET";}
 LObj*e=&R->o[R->no++];e->t=O_EXFIL;e->z=0;e->x=15;e->y=9;e->opt=0;e->n="LZ";}
static void lr_brief(int mi){int redraw=1,wsel=0;
 for(;;){BEAT->screen=SCR_LRB;BEAT->mode=mi;BEAT->x=wsel;const LMis*M=mi<LR_NM?&LM[mi]:&lrRand;
  if(redraw){cat_bgc(CAT_SPECIAL);banner(M->n,M->loc,C_YEL);m3_box(4,38,232,118,1);m3_text(12,44,M->b1,C_WHITE,1);m3_text(12,56,M->night?"NIGHT INSERT - STROBE EXTRACTION":"DAY INSERT - SMOKE EXTRACTION",M->night?C_BLUE:C_GRAY,1);
   for(int i=0;i<M->no&&i<6;i++)m3_text(12,68+i*9,cat3(OBN[M->o[i].t]," ",cat(M->o[i].n,M->o[i].opt?" (OPT)":"")),C_WHITE,1);
   m3_text(12,122,cat("POINT <L R>: ",wsel?"M16A1":"CAR-15"),C_YEL,1);m3_text(12,132,"TEAM: M79 PRC-25 3xM18A1 STROBE",C_GRAY,1);
   m3_text(150,122,cat3("BEST ",itoa(LRS.best[mi<LR_NM?mi:LR_NM]),""),C_GRAY,1);m3_ctext(146,"A: INSERT   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();if(pressed&(K_L|K_R|K_LEFT|K_RIGHT)){wsel^=1;redraw=1;sfx_menu();}if(pressed&K_B){pressed=0;return;}
  if(pressed&K_A){pressed=0;sfx_ok();lr_setup(mi);int r=lr_play();if(r>=0){int ix=mi<LR_NM?mi:LR_NM;int ideal=lrM->no*900+2200;int st=r?1+(lrScore*10>=ideal*6)+(lrScore*10>=ideal*9):0;if(lr_missed())st=r?1:0;int best=r&&lrScore>LRS.best[ix];if(best)LRS.best[ix]=lrScore;if(st>LRS.stars[ix])LRS.stars[ix]=st;
     for(int i=0;i<lrM->no;i++)if(lrDone_o[i]){if(lrM->o[i].t==O_SNATCH)LRS.snatch++;if(lrM->o[i].t==O_SENSOR)LRS.sensors++;}if(mi>=LR_NM&&r)LRS.patrols++;LRS.kia+=lrKills;LRS.last=mi;lrs_write();lr_result(r,st,best);if(r)music_victory();}
   bgm_t(TR_LRRP);if(mi>=LR_NM)lr_genrand();redraw=1;}}}
static void lrrp_ops(void){lrs_read();int sel=LRS.last<=LR_NM?LRS.last:0,redraw=1;bgm_t(TR_LRRP);const int N=LR_NM+1;
 for(;;){BEAT->screen=SCR_LRSEL;BEAT->x=sel;
  if(redraw){cat_bgc(CAT_SPECIAL);banner("LRRP","VIETNAM 1966-70 - SELECT A MISSION",C_YEL);m3_box(4,38,232,118,1);int top=sel>8?sel-8:0;
   for(int i=top;i<top+9&&i<N;i++){int y=44+(i-top)*11;m3_text(10,y,i==sel?">":" ",C_YEL,1);if(i==LR_NM){m3_text(34,y,"RANDOM PATROL",i==sel?C_YEL:C_WHITE,1);continue;}
    m3_text(18,y,itoa(i+1),C_GRAY,1);m3_text(34,y,LM[i].n,i==sel?C_YEL:C_WHITE,1);m3_text(140,y,LM[i].night?"NIGHT":"DAY",C_GRAY,1);for(int s=0;s<3;s++)m3_fill(176+s*8,y+1,6,6,s<LRS.stars[i]?C_YEL:RGB(50,50,60));}
   m3_ctext(146,"A: BRIEFING   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();if(pressed&K_UP){sel=(sel+N-1)%N;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%N;redraw=1;sfx_menu();}
  if(pressed&K_A){sfx_ok();pressed=0;if(sel==LR_NM)lr_genrand();lr_brief(sel);bgm_t(TR_LRRP);redraw=1;}
  if(pressed&K_B){pressed=0;bgCached=-1;return;}}}
