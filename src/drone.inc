// ---------------------------------------------------------------- DRONE MODE (v1.1, Modern era only)
// Real-world drone types with fictional missions in generic modern settings.
enum{DW_HELLFIRE,DW_GBU12,DW_GBU38,DW_MAML,DW_BA7,DW_LS6,DW_ARTY,DW_AC130,DW_KAMI,DW_MARKSTRIKE,DW_NAVAL,DW_N};
enum{WK_MISSILE,WK_LASER,WK_GPS,WK_SMART,WK_ARTY,WK_GUNSHIP,WK_KAMI,WK_ASSET};
typedef struct{const char*n;u8 kind;u8 dmg,R,delay;u16 cd;}DWpn;
static const DWpn DWP[DW_N]={
 {"AGM-114 HELLFIRE",WK_MISSILE,130,18,24,40},{"GBU-12 PAVEWAY II",WK_LASER,170,34,75,60},{"GBU-38 JDAM",WK_GPS,170,38,70,60},
 {"MAM-L",WK_SMART,110,18,32,40},{"BLUE ARROW 7",WK_MISSILE,120,18,26,40},{"LS-6 GLIDE BOMB",WK_GPS,150,34,80,60},
 {"ARTILLERY MISSION",WK_ARTY,90,26,150,900},{"AC-130 STRAFE",WK_GUNSHIP,45,14,40,1200},{"KAMIKAZE DIVE",WK_KAMI,160,34,45,90},
 {"STRIKE ON MARKED",WK_ASSET,150,30,110,480},{"NAVAL STRIKE",WK_ASSET,160,32,110,420}};
typedef struct{const char*n;const char*role;s16 spd;u8 alt,sensor,hp;u16 loiter;u8 w[3],c[3];u8 recon;u8 kami;}DroneS;
#define NDRONE 9
static const DroneS DRN[NDRONE]={
 {"MQ-9 REAPER","HUNTER-KILLER, HEAVY PAYLOAD",400,3,110,100,300,{DW_HELLFIRE,DW_GBU12,DW_GBU38},{4,1,1},0,0},
 {"MQ-1C GRAY EAGLE","LONG LOITER, LIGHT PAYLOAD",340,3,100,90,420,{DW_HELLFIRE,255},{3,0},0,0},
 {"MQ-1 PREDATOR","CLASSIC ARMED RECON",300,2,90,70,360,{DW_HELLFIRE,255},{2,0},0,0},
 {"RQ-4 GLOBAL HAWK","RECON ONLY - MARKS FOR OTHER ASSETS",420,5,160,120,480,{DW_MARKSTRIKE,255},{4,0},1,0},
 {"BAYRAKTAR TB2","TACTICAL, MAM-L SMART MUNITIONS",300,2,100,80,400,{DW_MAML,255},{4,0},0,0},
 {"MQ-20 AVENGER","JET-POWERED, FAST, JDAMS",620,4,100,110,240,{DW_GBU38,255},{4,0},0,0},
 {"SWITCHBLADE 600","LOITERING MUNITION - YOU ARE THE WEAPON",520,1,80,20,60,{DW_KAMI,255},{3,0},0,1},
 {"WING LOONG II","MALE UCAV, BLUE ARROW + LS-6",380,3,105,100,320,{DW_BA7,DW_LS6,255},{4,2,0},0,0},
 {"MQ-4C TRITON","MARITIME RECON, UNARMED, WIDE SENSOR",380,5,200,120,600,{DW_NAVAL,255},{5,0},1,0}};
static const char*const CALLN[3]={"NONE","ARTILLERY MISSION","AC-130 STRAFE"};
// MQ-9 alt loadout: GBU-38 instead of GBU-12 selectable via the second slot cycling
enum{DM_CONVOY,DM_SAM,DM_TOWN,DM_COAST,DM_SCORE,DM_SUPPORT,DM_N};
static const char*const DMN[DM_N]={"HIGHWAY INTERDICTION","SAM BELT","TOWN OVERWATCH","COASTAL WATCH","SCORE ATTACK","SUPPORT 6V6"};
static const char*const DMD[DM_N]={"STOP 3 ARMED CONVOYS ON THE ROAD","DESTROY 4 SAM SITES + 2 ARTILLERY","SHIELD A FRIENDLY SQUAD. CIVILIANS!","MARK/SINK 6 HOSTILE BOATS + A SUB","ENDLESS WAVES UNTIL FUEL RUNS OUT","DRONE SUPPORT FOR A 6V6 BATTLE"};
static const char*const DMS[DM_N]={"FICTIONAL DESERT CORRIDOR","FICTIONAL COASTAL HILLS","FICTIONAL RIVER TOWN","FICTIONAL ARCHIPELAGO","FICTIONAL BORDER PLAIN","FICTIONAL CITY OUTSKIRTS"};
enum{DT_VEH,DT_SAM,DT_AAA,DT_ARTY,DT_INF,DT_CIV,DT_FRND,DT_BOAT,DT_SUB,DT_FISH};
typedef struct{s32 x,y;s16 hp,type,alive,scan,mark,cd,vx,vy,ang,grp,t;}DT;
#define NDT 44
static DT D[NDT];static int nD;
typedef struct{s32 x,y;s16 t,w,live,enemy,k;}Strike;
#define NSTR 12
static Strike ST[NSTR];
static s32 dX,dY;static int dHP,dLoiter,dMis,dScore,dKills,dCiv,dFr,dEsc,dUsed,dMarked,dWave,dTimer,dFail,dSel,dScanT,dScanK,dKamiT,dKamiN,dVis;
static int dAmmo[4],dCd[4],dW[4],dNW,samT;
static int dOver;
static int is_hostile(int t){return t<=DT_INF||t==DT_BOAT||t==DT_SUB;}
static DT dtDummy;
static DT*dt_add(int type,int x,int y,int hp){DT*d=0;for(int i=0;i<nD;i++)if(!D[i].alive&&D[i].type!=DT_FRND){d=&D[i];break;}if(!d){if(nD>=NDT)return &dtDummy;d=&D[nD++];}memset(d,0,sizeof *d);d->type=type;d->x=x<<8;d->y=y<<8;d->hp=hp;d->alive=1;d->ang=rr(256);d->cd=rr(120);return d;}
static int dwrap(int d,int W){return mapWrap?wrapd(d,W):d;}
static int ddist(DT*d,s32 x,s32 y){return idist(dwrap((d->x-x)>>8,mapW*16),dwrap((d->y-y)>>8,mapH*16));}
static void d_kill(DT*d){d->alive=0;fx_add(d->x>>8,d->y>>8,FXK_BIG);fx_add((d->x>>8)+3,(d->y>>8)-4,FXK_SMOKE);
 if(is_hostile(d->type)){dKills++;dScore+=d->type==DT_SAM?400:d->type==DT_INF?100:d->type==DT_SUB?600:250;}
 else if(d->type==DT_CIV||d->type==DT_FISH){dCiv++;dScore-=500;feed("CIVILIAN CASUALTY - ROE!",HB_RED);}
 else{dFr++;dScore-=300;feed("FRIENDLY FIRE!",HB_RED);}}
static void d_blast(s32 x,s32 y,int R,int dmg,int enemy){int gx=x>>8,gy=y>>8;fx_add(gx,gy,FXK_BIG);fx_add(gx+rr(9)-4,gy-5,FXK_SMOKE);sfx_boom();
 if(idist(dwrap(gx-(dX>>8),mapW*16),dwrap(gy-(dY>>8),mapH*16))<120)shake(6);
 for(int i=0;i<nD;i++){DT*d=&D[i];if(!d->alive)continue;if(d->type==DT_SUB&&d->t<200)continue;int dd=ddist(d,x,y);
  if(dd<R){int dm=dmg-dd*dmg/(R*2);if(enemy&&is_hostile(d->type))continue;d->hp-=dm;if(d->hp<=0)d_kill(d);}}
 if(dMis==DM_SUPPORT)for(int k=0;k<NU;k++){Unit*u=&U[k];if(!u->alive)continue;int dd=idist((u->x-x)>>8,(u->y-y)>>8);
  if(dd<R){if(enemy&&u->team==1)continue;int hpb=u->hp;damage(k,dmg-dd*dmg/(R*2),-1);if(!u->alive&&hpb>0){if(u->team==1){dKills++;dScore+=150;}else if(!enemy){dFr++;dScore-=300;feed("FRIENDLY FIRE!",HB_RED);}}}}}
static void st_add(s32 x,s32 y,int t,int w,int enemy,int k){for(int i=0;i<NSTR;i++)if(!ST[i].live){Strike*s=&ST[i];s->x=x;s->y=y;s->t=t;s->w=w;s->live=1;s->enemy=enemy;s->k=k;return;}}
// ---- mission setup
static void d_spawn_wave(void){int W=mapW*16,H=mapH*16;dWave++;
 if(dMis==DM_TOWN){int n=3+dWave;for(int k=0;k<n;k++){int sx=rr(2)?24+rr(60):W-24-rr(60);dt_add(DT_INF,sx,24+rr(40),40)->grp=1;}feed(cat("ENEMY SQUAD WAVE ",itoa(dWave)),HB_YEL);}
 else if(dMis==DM_SCORE){for(int k=0;k<3+dWave;k++)dt_add(rr(3)?DT_INF:DT_VEH,40+rr(W-80),24+rr(60),rr(3)?40:120);if(dWave%2==0)dt_add(DT_AAA,60+rr(W-120),80+rr(H-160),150);
  if(dWave%3==0)dt_add(DT_SAM,60+rr(W-120),80+rr(H-160),180);for(int k=0;k<2;k++)dt_add(DT_CIV,40+rr(W-80),40+rr(H-80),30);feed(cat("WAVE ",itoa(dWave)),HB_YEL);}
 else if(dMis==DM_CONVOY){for(int k=0;k<3;k++){DT*v=dt_add(DT_VEH,W/2+(k==1?10:-6),-20-k*36,130);if(v){v->grp=dWave;v->vy=110;}}feed(cat3("CONVOY ",itoa(dWave)," ON THE HIGHWAY"),HB_YEL);}}
static void d_setup(int mis){dMis=mis;nD=0;memset(ST,0,sizeof ST);dWave=0;
 int W=mapW*16,H=mapH*16;
 switch(mis){
  case DM_CONVOY:for(int k=0;k<2;k++)dt_add(DT_AAA,W/2+(k?70:-80),H/3+k*H/3,150);dt_add(DT_SAM,W/2-120,H/2,180);for(int k=0;k<5;k++)dt_add(DT_CIV,40+rr(W-80),40+rr(H-80),30);d_spawn_wave();dTimer=0;break;
  case DM_SAM:for(int k=0;k<4;k++)dt_add(DT_SAM,80+(k&1)*(W-160)+rr(40),90+(k>>1)*(H-200)+rr(40),180);for(int k=0;k<2;k++)dt_add(DT_ARTY,W/2-60+k*120,H/2+rr(40),140);
   for(int k=0;k<4;k++)dt_add(DT_AAA,60+rr(W-120),60+rr(H-120),150);for(int k=0;k<6;k++)dt_add(DT_INF,60+rr(W-120),60+rr(H-120),40);for(int k=0;k<4;k++)dt_add(DT_CIV,40+rr(W-80),40+rr(H-80),30);break;
  case DM_TOWN:for(int k=0;k<5;k++)dt_add(DT_FRND,W/2-30+k*14,H/2+30,100);for(int k=0;k<9;k++)dt_add(DT_CIV,40+rr(W-80),60+rr(H-120),30);dt_add(DT_AAA,W/2+90,60,150);d_spawn_wave();dTimer=0;break;
  case DM_COAST:for(int k=0;k<6;k++){DT*b=dt_add(DT_BOAT,rr(W),rr(H),k<2?220:90);if(b){b->vx=k<2?50:140;b->grp=k<2;}}{DT*s=dt_add(DT_SUB,rr(W),rr(H),200);if(s)s->vx=40;}
   for(int k=0;k<6;k++){DT*f=dt_add(DT_FISH,rr(W),rr(H),40);if(f)f->vx=60;}for(int k=0;k<2;k++)dt_add(DT_AAA,rr(W),rr(H),150)->grp=7;break; // AAA here = armed frigate escort
  case DM_SCORE:d_spawn_wave();break;
  case DM_SUPPORT:dt_add(DT_AAA,W/2,60,150);dt_add(DT_AAA,W/2+60,90,150);break;}}
static int d_objs_left(void){int n=0;
 switch(dMis){case DM_CONVOY:for(int i=0;i<nD;i++)if(D[i].alive&&D[i].type==DT_VEH)n++;return n+(dWave<3?1:0);
  case DM_SAM:for(int i=0;i<nD;i++)if(D[i].alive&&(D[i].type==DT_SAM||D[i].type==DT_ARTY))n++;return n;
  case DM_TOWN:for(int i=0;i<nD;i++)if(D[i].alive&&D[i].type==DT_INF)n++;return n+(dWave<3?1:0);
  case DM_COAST:for(int i=0;i<nD;i++)if(D[i].alive&&(D[i].type==DT_BOAT||D[i].type==DT_SUB))n++;return n;
  default:return 1;}}
static const char*d_obj_text(void){static char ob[32];int n=0;
 switch(dMis){case DM_CONVOY:{int k=0;for(int i=0;i<nD;i++)if(D[i].alive&&D[i].type==DT_VEH)k++;return cat("CONVOY VEH LEFT ",itoa(k+(3-dWave)*3));}
  case DM_SAM:return cat("SAM/ARTY LEFT ",itoa(d_objs_left()));
  case DM_TOWN:for(int i=0;i<nD;i++)if(D[i].alive&&D[i].type==DT_FRND)n++;return cat3("WAVE ",itoa(dWave),"/3  HOLD THE SQUAD");
  case DM_COAST:return cat("HOSTILE VESSELS ",itoa(d_objs_left()));
  case DM_SCORE:return cat("SCORE ",itoa(dScore));
  default:return "SUPPORT YOUR SQUAD";}
 (void)ob;}
// ---- entity AI
static void d_update(void){int W=mapW*16,H=mapH*16;const DroneS*Dr=&DRN[droneSel];
 for(int i=0;i<nD;i++){DT*d=&D[i];if(!d->alive)continue;d->t++;
  switch(d->type){
   case DT_VEH:if(dMis==DM_CONVOY){d->y+=d->vy;d->ang=128;if((d->y>>8)>H+10){d->alive=0;dEsc++;feed("CONVOY VEHICLE ESCAPED",HB_RED);}}
    else{if(--d->cd<=0){d->cd=90+rr(90);d->ang=rr(256);}int a=d->ang;s32 nx=d->x+SIN(a)/2,ny=d->y-COS(a)/2;if(!(flag_at(nx>>8,ny>>8)&TF_SOLID)&&nx>8<<8&&ny>8<<8&&nx<(W-8)<<8&&ny<(H-8)<<8){d->x=nx;d->y=ny;}else d->ang+=128;}break;
   case DT_INF:case DT_CIV:case DT_FRND:{int sp=d->type==DT_CIV?90:110;
    if(d->type==DT_INF&&dMis==DM_TOWN){int best=-1,bd=9999;for(int k=0;k<nD;k++)if(D[k].alive&&D[k].type==DT_FRND){int q=ddist(&D[k],d->x,d->y);if(q<bd){bd=q;best=k;}}
      if(best>=0){if(bd>50)d->ang=atan2i((D[best].x-d->x)>>8,(D[best].y-d->y)>>8);else{sp=0;if(--d->cd<=0){d->cd=50+rr(40);if(rr(100)<35){D[best].hp-=diffd(20);fx_add(D[best].x>>8,D[best].y>>8,FXK_SPARK);sfx_shot(0);if(D[best].hp<=0){D[best].alive=0;feed("FRIENDLY DOWN",HB_RED);}}}}}}
    else if(d->type==DT_FRND){sp=0;if(--d->cd<=0){d->cd=60+rr(40);for(int k=0;k<nD;k++)if(D[k].alive&&D[k].type==DT_INF&&ddist(&D[k],d->x,d->y)<70){if(rr(100)<30){D[k].hp-=25;fx_add(D[k].x>>8,D[k].y>>8,FXK_SPARK);if(D[k].hp<=0){D[k].alive=0;dScore+=50;}}break;}}}
    else if(--d->cd<=0){d->cd=60+rr(80);d->ang=rr(256);}
    if(sp){s32 nx=d->x+(SIN(d->ang)*sp>>8),ny=d->y-(COS(d->ang)*sp>>8);if(!(flag_at(nx>>8,ny>>8)&TF_SOLID)&&nx>8<<8&&ny>8<<8&&nx<(W-8)<<8&&ny<(H-8)<<8){d->x=nx;d->y=ny;}else d->ang+=100;}
    break;}
   case DT_BOAT:case DT_FISH:case DT_SUB:{if(--d->cd<=0){d->cd=120+rr(120);d->ang+=rr(80)-40;}d->x=(d->x+(SIN(d->ang)*d->vx>>8))&((W<<8)-1);d->y=(d->y-(COS(d->ang)*d->vx>>8))&((H<<8)-1);
    if(d->type==DT_SUB){if(d->t>360)d->t=0;}break;}
   case DT_SAM:case DT_AAA:case DT_ARTY:{int dd=ddist(d,dX,dY);
    if(d->type==DT_SAM){int R=(190-22*Dr->alt)*BOTD[SET.bot].eng/100;if(dd<R&&--d->cd<=0){d->cd=(200+rr(60))*100/BOTD[SET.bot].aa;Shot*s=shot_new();if(s){s->x=d->x;s->y=d->y;s->kind=K_MISSILE;s->team=1;s->owner=-1;s->life=80;s->dmg=diffd(52-8*Dr->alt)*BOTD[SET.bot].aa/100;s->dist=520;s->tgt=-2;s->ang=atan2i(dwrap((dX-d->x)>>8,W),dwrap((dY-d->y)>>8,H))<<8;}feed("SAM LAUNCH! EVADE",HB_RED);sfx_whoosh();}}
    else if(d->type==DT_AAA){int R=(130-18*Dr->alt)*BOTD[SET.bot].eng/100;if(dd<R&&--d->cd<=0){d->cd=(24+rr(20))*100/BOTD[SET.bot].aa;int sp=BOTD[SET.bot].jit*4+9;int fx=(dX>>8)+rr(sp*2)-sp,fy=(dY>>8)+rr(sp*8/5)-sp*4/5;fx_add(fx,fy,FXK_SMALL);if(idist(fx-(dX>>8),fy-(dY>>8))<16){dHP-=diffd(9-Dr->alt);sfx_hurt();}}}
    else if(--d->cd<=0){d->cd=300;for(int k=0;k<nD;k++)if(D[k].alive&&D[k].type==DT_FRND){fx_add(D[k].x>>8,(D[k].y>>8)+rr(30)-15,FXK_BIG);break;}}
    break;}}}
 // SAM missiles chase the drone (screen centre)
 for(int i=0;i<NS;i++){Shot*s=&S[i];if(s->life<=0||s->tgt!=-2)continue;s->life--;int aim=atan2i(dwrap((dX-s->x)>>8,W),dwrap((dY-s->y)>>8,H));turn_to(&s->ang,aim,640);
  int a=s->ang>>8;s->x+=SIN(a)*s->dist>>8;s->y-=COS(a)*s->dist>>8;if(mapWrap){s->x&=(W<<8)-1;s->y&=(H<<8)-1;}if((s->life&3)==0)fx_add(s->x>>8,s->y>>8,FXK_SMOKE);
  if(idist(dwrap((s->x-dX)>>8,W),dwrap((s->y-dY)>>8,H))<10){s->life=0;dHP-=s->dmg;fx_add(dX>>8,dY>>8,FXK_BIG);sfx_boom();shake(10);feed("HIT BY SAM!",HB_RED);}
  if(s->life==0)fx_add(s->x>>8,s->y>>8,FXK_SMALL);}
 // strikes
 for(int i=0;i<NSTR;i++){Strike*s=&ST[i];if(!s->live)continue;const DWpn*w=&DWP[s->w];
  if(w->kind==WK_LASER&&!s->enemy){s->x+=(dX-s->x)/6;s->y+=(dY-s->y)/6;} // laser-guided: follows the designator (reticle)
  if(w->kind==WK_KAMI){s->x=dX;s->y=dY;}
  if(w->kind==WK_GUNSHIP){if(--s->t>0)continue;if(s->k>0){s->k--;s->t=4;s->x+=(rr(13)-6)<<8;s->y-=6<<8;d_blast(s->x+((rr(24)-12)<<8),s->y,w->R,w->dmg,s->enemy);sfx_shot(0);}else s->live=0;continue;}
  if(--s->t<=0){if(w->kind==WK_ARTY&&s->k>0){s->k--;s->t=18;d_blast(s->x+((rr(48)-24)<<8),s->y+((rr(48)-24)<<8),w->R,w->dmg,s->enemy);continue;}
   d_blast(s->x,s->y,w->R,w->dmg,s->enemy);s->live=0;if(w->kind==WK_KAMI){dKamiT=90;dKamiN--;}}}}
// ---- rendering
static void d_draw(void){int W=mapW*16;const DroneS*Dr=&DRN[droneSel];
 for(int i=0;i<NSTR;i++){Strike*s=&ST[i];if(!s->live)continue;int x=scrx(s->x),y=scry(s->y);const DWpn*w=&DWP[s->w];
  if(s->enemy){if(frame&8)spr(x-8,y-8,OT_FX+FX_BRK_E,0,1,3,0);}
  else if(w->kind==WK_MISSILE||w->kind==WK_SMART){int k=w->delay-s->t;int sx=120+(x-120)*k/w->delay,sy=200+(y-200)*k/w->delay;spr_aff(sx,sy,OT_FX+FX_MISSILE,0,3,atan2i(x-sx,y-sy),0x100,0,0);}
  else if(w->kind==WK_GPS||w->kind==WK_LASER||w->kind==WK_ASSET){if(frame&4)spr(x-4,y-4,OT_FX+FX_GRENADE,0,0,3,0);}
  else if(w->kind==WK_ARTY||w->kind==WK_GUNSHIP){if(frame&8)spr(x-8,y-8,OT_FX+FX_BRK_C,0,1,3,0);}}
 for(int i=0;i<NS;i++){Shot*s=&S[i];if(s->life<=0||s->tgt!=-2)continue;spr_aff(scrx(s->x),scry(s->y),OT_FX+FX_MISSILE,0,3,s->ang>>8,0x100,1,0);}
 for(int i=0;i<nD;i++){DT*d=&D[i];if(!d->alive)continue;int x=scrx(d->x),y=scry(d->y);if(x<-20||x>260||y<-20||y>180)continue;
  int sen=ddist(d,dX,dY)<Dr->sensor;
  if(d->type==DT_SUB){if(d->t>200){if((frame&3)==0)fx_add(d->x>>8,d->y>>8,FXK_SMOKE);spr_aff(x,y,OT_ERA+ES_SHIP,2,1,d->ang,0x200,1,0);}if(d->mark&&(frame&16))spr(x-8,y-8,OT_FX+FX_BRK_E,0,1,3,0);continue;}
  int pal=d->type==DT_FRND?0:1;
  switch(d->type){case DT_VEH:spr_aff(x,y,OT_ERA+ES_HULL,2,pal,d->ang,0x100,1,0);break;
   case DT_SAM:case DT_AAA:case DT_ARTY:if(dMis==DM_COAST)spr_aff(x,y,OT_ERA+ES_SHIP,2,pal,d->ang,0x100,1,0);else{spr_aff(x,y,OT_ERA+ES_HULL,2,pal,0,0x140,1,0);spr_aff(x,y,OT_ERA+ES_TUR,2,pal,(d->type==DT_SAM?frame:d->t)&255,0x100,1,0);}break;
   case DT_BOAT:case DT_FISH:spr_aff(x,y,OT_ERA+ES_SHIP,2,pal,d->ang,d->grp?0x100:0x180,1,0);break;
   default:spr_aff(x,y,OT_ERA+ES_SOLDIER+((d->t>>3)&1)*4,1,pal,d->ang,0x100,1,0);}
  if(d->type==DT_FRND)spr(x-8,y-14,OT_FX+FX_IFF,0,1,3,0);
  else if(d->scan&&sen){if(is_hostile(d->type))spr(x-8,y-8,OT_FX+FX_BRK_E,0,1,3,0);else spr(x-8,y-8,OT_FX+FX_BRK_C,0,1,3,0);}
  else if(d->mark)spr(x-8,y-8,OT_FX+FX_BRK_E,0,1,3,0);}
 (void)W;}
static void d_hud(void){const DroneS*Dr=&DRN[droneSel];hud_clear();
 htxt(0,0,Dr->n,HB_GREEN);htxt(18,0,dVis==0?"WHOT":dVis==1?"BHOT":"TV",HB_GREEN);htxt(23,0,cat3("ALT",itoa(Dr->alt*5),"K"),HB_GREEN);
 hctxt(1,d_obj_text(),HB_YEL,0);
 hearts(0,19,5,dHP,Dr->hp);
 const DWpn*w=&DWP[dW[dSel]];htxt(6,19,w->n,dCd[dSel]?HB_GRAY:HB_WHITE);htxt(25,19,cat("X",itoa(dAmmo[dSel])),dAmmo[dSel]?HB_WHITE:HB_RED);
 htxt(0,18,cat("FUEL ",mmss(dLoiter)),dLoiter<60*30?HB_RED:HB_GREEN);htxt(13,18,cat("SCORE ",itoa(dScore)),HB_WHITE);
 if(dCd[dSel])hbar(25,18,4,w->cd-dCd[dSel],w->cd);else htxt(25,18,"RDY",HB_GREEN);
 if(dScanT>0)hbar(13,12,4,dScanT,20);
 if(dKamiT>0)hctxt(9,dKamiN>0?"LAUNCHING NEXT MUNITION":"NO MUNITIONS LEFT",HB_YEL,0);
 if(msgT>0){msgT--;hctxt(3,msgS,msgB,0);}
 if(SET.hints&&hintT>0){hintT--;hctxt(14,"A FIRE B HOLD=SCAN/MARK",HB_GRAY,0);hctxt(15,"L/R WEAPON SEL THERMAL",HB_GRAY,0);}
 // corner brackets of the targeting overlay
 htxt(0,2,"+",HB_GREEN);htxt(29,2,"+",HB_GREEN);htxt(0,17,"+",HB_GREEN);htxt(29,17,"+",HB_GREEN);}
static void d_palette(void){ // thermal overlay: BG -> greyscale; hot targets white (WHOT) / black (BHOT)
 for(int i=0;i<32;i++){u16 c=PAL_BG[i];int l=((c&31)*5+((c>>5)&31)*9+((c>>10)&31)*2)>>4;if(dVis==0)l=l*2/3;else if(dVis==1)l=31-l*2/3;
  if(dVis<2)PAL_BG[i]=l|(l<<5)|(l<<10);}
 for(int b=0;b<3;b++)for(int i=1;i<16;i++){u16 c=PAL_OBJ[b*16+i];int l=((c&31)*5+((c>>5)&31)*9+((c>>10)&31)*2)>>4;
  if(dVis==0){l=18+l/2;PAL_OBJ[b*16+i]=l|(l<<5)|(l<<10);}else if(dVis==1){l=l/3;PAL_OBJ[b*16+i]=l|(l<<5)|(l<<10);}}}
static void d_load(void){ // (re)load graphics for current mission
 if(dMis==DM_COAST)setup_mode0(sky_t(0),32*4*8,sky_p(0),2,0);
 else{setup_mode0(arena_t(2),AM_N*4*8,arena_p(2),2,0);for(int i=1;i<16;i++)PAL_BG[16+i]=water_p(2)[i];}
 d_palette();}
static void d_result(void){int win=!dFail;if(win)music_victory();else music_defeat();
 int stars=win?1+(dCiv==0&&dFr==0)+(dHP>DRN[droneSel].hp/2):0;
 for(int i=0;;i++){hud_clear();hpanel(2,2,26,16);hctxt(3,win?"MISSION COMPLETE":"MISSION FAILED",win?HB_GREEN:HB_RED,1);hctxt(5,DMN[dMis],HB_YEL,1);
  hptxt(4,7,"DRONE",HB_GRAY);hptxt(13,7,DRN[droneSel].n,HB_WHITE);
  hptxt(4,8,"TARGETS DESTROYED",HB_WHITE);hptxt(24,8,itoa(dKills),HB_WHITE);hptxt(4,9,"TARGETS MARKED",HB_WHITE);hptxt(24,9,itoa(dMarked),HB_WHITE);
  hptxt(4,10,"MUNITIONS USED",HB_WHITE);hptxt(24,10,itoa(dUsed),HB_WHITE);hptxt(4,11,"CIVILIAN CASUALTIES",dCiv?HB_RED:HB_WHITE);hptxt(24,11,itoa(dCiv),dCiv?HB_RED:HB_WHITE);
  hptxt(4,12,"FRIENDLY FIRE",dFr?HB_RED:HB_WHITE);hptxt(24,12,itoa(dFr),dFr?HB_RED:HB_WHITE);hptxt(4,13,"SCORE",HB_YEL);hptxt(24,13,itoa(dScore),HB_YEL);
  if(!win)hctxt(14,dFail==2?"ROE VIOLATION":dFail==3?"DRONE LOST":dFail==4?"BINGO FUEL":dFail==5?"SQUAD LOST":"OBJECTIVE FAILED",HB_RED,1);
  else{for(int k=0;k<3;k++)hptxt(11+k*3,14,k<stars?"*":"-",k<stars?HB_YEL:HB_GRAY);}
  if(i>40)hctxt(16,"A: CONTINUE",HB_YEL,1);BEAT->screen=SCR_RESULT;BEAT->phase=win;BEAT->kills=dKills;
  oam_begin();present(0,0,0);if(i>40&&(pressed&(K_A|K_START)))break;}
 music_stop();}
static void drone_run(int mis){
 mpMode=MM_INF;mpEra=2;BEAT->mode=4;BEAT->era=2;bgm_t(TR_LOITER);const DroneS*Dr=&DRN[droneSel];
 memset(U,0,sizeof U);memset(S,0,sizeof S);memset(FXs,0,sizeof FXs);memset(smT,0,sizeof smT);memset(zn,0,sizeof zn);score[0]=score[1]=0;msgT=0;hintT=300;
 static const MapDef DMAP[DM_N]={{"","",LY_FIELD,TH_DESERT,61},{"","",LY_FOREST,0,62},{"","",LY_TOWN,0,63},{"","",1,0,64},{"","",LY_FIELD,0,65},{"","",LY_TOWN,0,66}};
 curMap=&DMAP[mis];if(mis==DM_COAST){mpEra=0;gen_sky();mpEra=2;}else if(mis==DM_SUPPORT)gen_arena(40,40,0);else gen_arena(56,56,0);
 dMis=mis;dVis=0;d_load();
 dX=(mapW*8)<<8;dY=(mapH*16-60)<<8;if(mis==DM_COAST||mis==DM_SAM)dY=(mapH*8)<<8;
 dHP=Dr->hp;dLoiter=Dr->loiter*60;dScore=0;dKills=0;dCiv=0;dFr=0;dEsc=0;dUsed=0;dMarked=0;dFail=0;dSel=0;dScanT=0;dKamiT=0;dKamiN=Dr->c[0];dOver=0;samT=0;
 dNW=0;for(int k=0;k<3;k++)if(Dr->w[k]!=255&&Dr->c[k]){dW[dNW]=Dr->w[k];dAmmo[dNW]=Dr->c[k];dCd[dNW]=0;dNW++;}
 if(droneCall){dW[dNW]=droneCall==1?DW_ARTY:DW_AC130;dAmmo[dNW]=droneCall==1?2:1;dCd[dNW]=0;dNW++;}
 d_setup(mis);
 if(mis==DM_SUPPORT){xdmg=damage;for(int i=0;i<NU;i++){U[i].team=i<6?0:1;U[i].isp=0;U[i].think=i;U[i].wpn=rr(NPRIM_BOT[2]);spawn(&U[i],i);}mpTimer=60*60*4;}
 feed(DMS[mis],HB_YEL);
 int scr=SCR_DRONE;int enemyDroneT=900;
 for(;;){
  BEAT->screen=scr;BEAT->hp=dHP;BEAT->kills=dKills;BEAT->deaths=dCiv*16+dFr;BEAT->scoreA=dScore<0?0:dScore;BEAT->scoreB=score[1];BEAT->timer=dLoiter;BEAT->x=dX>>8;BEAT->y=dY>>8;BEAT->phase=dWave;BEAT->extra=d_objs_left();
  if(pressed&K_START){if(pause_menu())return;d_load();}
  if((keys&(K_L|K_R))==(K_L|K_R)&&(pressed&K_SEL)&&!dOver){for(int i=0;i<nD;i++)if(D[i].alive&&is_hostile(D[i].type))d_kill(&D[i]);dWave=9;if(mis==DM_SUPPORT)score[0]=30;} // test skip
  int W=mapW*16,H=mapH*16;
  if(!dOver){
   // flight
   if(dKamiT>0){dKamiT--;if(dKamiT==0){if(dKamiN<=0){dFail=d_objs_left()?1:0;dOver=60;}else{dX=(W/2)<<8;dY=(H-60)<<8;}}}
   else{int sp=Dr->spd;if(keys&K_R&&!(keys&K_L))sp/=2;int dx=0,dy=0;if(keys&K_LEFT)dx--;if(keys&K_RIGHT)dx++;if(keys&K_UP)dy--;if(keys&K_DOWN)dy++;if(dx&&dy)sp=sp*181/256;
    dX+=dx*sp;dY+=dy*sp;if(mapWrap){dX&=(W<<8)-1;dY&=(H<<8)-1;}else{dX=clampi(dX,(8)<<8,(W-8)<<8);dY=clampi(dY,8<<8,(H-8)<<8);}}
   if(pressed&K_L&&!(keys&K_R)){dSel=(dSel+1)%dNW;feed(DWP[dW[dSel]].n,HB_YEL);sfx_menu();}
   if(pressed&K_SEL&&!(keys&(K_L|K_R))){dVis=(dVis+1)%3;d_load();sfx_menu();}
   for(int k=0;k<dNW;k++)if(dCd[k])dCd[k]--;
   // scan / mark (hold B)
   int best=-1,bd=14;for(int i=0;i<nD;i++){DT*d=&D[i];if(!d->alive||d->type==DT_FRND||(d->type==DT_SUB&&d->t<200))continue;int q=ddist(d,dX,dY);if(q<bd){bd=q;best=i;}}
   if(keys&K_B&&best>=0&&dKamiT==0){if(dScanK!=best){dScanK=best;dScanT=0;}if(++dScanT>=20){DT*d=&D[best];if(!d->scan){d->scan=1;if(is_hostile(d->type)){d->mark=1;dMarked++;dScore+=25;feed(d->type==DT_SUB?"SUB MARKED":"HOSTILE MARKED",HB_RED);}else feed("CIVILIAN - DO NOT ENGAGE",HB_YEL);sfx_ok();}dScanT=0;}}
   else dScanT=0;
   // fire
   if(pressed&K_A&&dKamiT==0){int k=dSel;const DWpn*w=&DWP[dW[k]];
    if(dAmmo[k]<=0)feed("WINCHESTER - NO MUNITIONS",HB_RED);else if(dCd[k])feed("WEAPON CYCLING",HB_GRAY);
    else if(w->kind==WK_ASSET){int n=0;for(int i=0;i<nD;i++)if(D[i].alive&&D[i].mark&&n<6){st_add(D[i].x,D[i].y,w->delay+n*14,dW[k],0,0);n++;}
     if(n){dAmmo[k]--;dCd[k]=w->cd;dUsed+=n;feed(cat(dW[k]==DW_NAVAL?"NAVAL STRIKE ON ":"ASSETS STRIKING ",itoa(n)),HB_YEL);sfx_ok();}else feed("NO MARKED TARGETS - HOLD B",HB_RED);}
    else{dAmmo[k]--;dCd[k]=w->cd;dUsed++;st_add(dX,dY,w->delay,dW[k],0,w->kind==WK_ARTY?3:w->kind==WK_GUNSHIP?10:0);
     if(w->kind==WK_KAMI)feed("TERMINAL DIVE",HB_RED);else feed(w->kind==WK_LASER?"LASING - KEEP RETICLE ON":w->kind==WK_ARTY?"FIRE MISSION - SPLASH":w->kind==WK_GUNSHIP?"GUNSHIP INBOUND":"RIFLE - WEAPON AWAY",HB_YEL);sfx_whoosh();}}
   d_update();
   if(mis==DM_SUPPORT){for(int i=0;i<NU;i++){Unit*u=&U[i];if(!u->alive){if(--u->resp<=0)spawn(u,i);continue;}if(u->inv>0)u->inv--;inf_update(i);}shots_update();
    if(--enemyDroneT<=0){enemyDroneT=1100+rr(400);int t=-1;for(int k=0;k<6;k++)if(U[k].alive){t=k;break;}if(t>=0){st_add(U[t].x,U[t].y,140,DW_BA7,1,0);feed("ENEMY WING LOONG STRIKE INBOUND",HB_RED);}}
    if(--mpTimer<=0||score[0]>=30||score[1]>=30){dFail=score[0]>=score[1]?0:1;dOver=90;feed(dFail?"ENEMY TEAM WINS":"YOUR TEAM WINS",HB_YEL);}}
   // waves / timers
   dLoiter--;if(dLoiter<=0){dFail=(mis==DM_SCORE)?0:4;dOver=60;feed(mis==DM_SCORE?"BINGO FUEL - RTB":"BINGO FUEL",HB_YEL);}
   if(dHP<=0){dHP=0;dFail=3;dOver=90;fx_add(dX>>8,dY>>8,FXK_BIG);feed("DRONE LOST",HB_RED);}
   if(dCiv>=3||dFr>=2){dFail=2;dOver=90;}
   if(mis==DM_CONVOY){int alive=0;for(int i=0;i<nD;i++)if(D[i].alive&&D[i].type==DT_VEH)alive++;if(alive==0&&dWave<3)d_spawn_wave();if(dEsc>=3){dFail=1;dOver=90;feed("TOO MANY VEHICLES ESCAPED",HB_RED);}}
   if(mis==DM_TOWN){int en=0,fr=0;for(int i=0;i<nD;i++){if(D[i].alive&&D[i].type==DT_INF)en++;if(D[i].alive&&D[i].type==DT_FRND)fr++;}if(fr==0){dFail=5;dOver=90;}else if(en==0&&dWave<3)d_spawn_wave();}
   if(mis==DM_SCORE){int en=0;for(int i=0;i<nD;i++)if(D[i].alive&&(D[i].type==DT_INF||D[i].type==DT_VEH))en++;if(en==0)d_spawn_wave();}
   if(!dOver&&mis!=DM_SCORE&&mis!=DM_SUPPORT&&d_objs_left()==0&&(mis!=DM_CONVOY||dWave>=3)&&(mis!=DM_TOWN||dWave>=3)){dOver=90;feed("ALL OBJECTIVES COMPLETE",HB_GREEN);}
   if(dWave>=9&&!dOver){dOver=60;dFail=0;}
  }else if(--dOver==0){d_result();return;}
  camX=(dX>>8)-120;camY=(dY>>8)-80;if(mapWrap){camX&=W-1;camY&=H-1;}else{camX=clampi(camX,0,W-240);camY=clampi(camY,0,H-160);}
  // reticle is screen-centre in wrap maps; on clamped maps it follows the drone
  oam_begin();d_hud();
  {int rx=scrx(dX),ry=scry(dY);spr(rx-8,ry-8,OT_FX+FX_RETICLE,0,1,3,0);}
  d_draw();if(mis==DM_SUPPORT){draw_shots();draw_units();}fx_update_draw();
  draw_layer(map,bgw,camX,camY,0,mapWrap);present(0,0,0);}}
