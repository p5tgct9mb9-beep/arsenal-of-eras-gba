// ===================================================================== v1.3.2 SANDBOX - custom mixed-era battles on the real 6v6 engine
// Content rules carry over from the battle modes: fictional/generic factions only, no real leaders, no civilians on the field.
#define SCR_SAND 100
typedef struct{u8 type,mapE,mapI,fac0,fac1,era0,era1,cnt0,cnt1,mix,bot,win,tgt,pad[3];}SbP;
typedef struct{SbP p[3];u8 used[3],pad;u16 played,won;}SbSave;static SbSave SBS EWRAM;static int sbs_ld;
static void sbs_load(void){if(sbs_ld)return;sbs_ld=1;if(!sv_get(0x2400,'S',1,&SBS,sizeof SBS))memset(&SBS,0,sizeof SBS);}
static void sbs_save(void){sv_put(0x2400,'S',1,&SBS,sizeof SBS);}
static const char*const SBT[3]={"INFANTRY","ARMOR + VEHICLES","AIRCRAFT"};
static const char*const SBERA[7]={"MEDIEVAL","AMERICAN REV.","WORLD WAR I","WORLD WAR II","COLD WAR","MODERN","FUTURE"};
static const u8 SBEI[7]={7,6,5,0,1,2,3}; // -> engine era index
static const char*const SBW[4]={"TEAM DEATHMATCH","TIME LIMIT","LAST STANDING","SCORE (YOU X2)"};
static const u8 SBTG[5]={10,20,30,50,75};static const u8 SBTM[4]={3,5,8,12};
static const char*const SBBOT[4]={"RECRUIT","REGULAR","VETERAN","ELITE"};
static SbP sbDef(int k){SbP p;memset(&p,0,sizeof p);p.type=0;p.mapE=3;p.fac0=0;p.fac1=2;p.era0=0;p.era1=5;p.cnt0=6;p.cnt1=6;p.bot=1;p.win=0;p.tgt=1;
 if(k==1){p.type=1;p.era0=6;p.era1=3;p.mapE=3;p.cnt0=4;p.cnt1=6;p.win=2;}return p;} // preset 2: future tanks vs WW2 armour, last standing
static const char*sb_val(const SbP*p,int r){switch(r){case 0:return SBT[p->type];case 1:return cat3(ERA_NAME[SBEI[p->mapE]]," ",itoa(p->mapI+1));case 2:return FAC[p->fac0].n;case 3:return p->mix?"(MIXED)":SBERA[p->era0];case 4:return itoa(p->cnt0);
 case 5:return FAC[p->fac1].n;case 6:return p->mix?"(MIXED)":SBERA[p->era1];case 7:return itoa(p->cnt1);case 8:return p->mix?"EVERY UNIT RANDOM":"ONE ERA PER SIDE";case 9:return SBBOT[p->bot];case 10:return SBW[p->win];
 case 11:return p->win==1?cat(itoa(SBTM[p->tgt&3])," MIN"):p->win==2?"NO RESPAWNS":cat(itoa(SBTG[p->tgt%5])," KILLS");}return "";}
static void sb_launch(SbP*p){int mode=p->type==0?MM_INF:p->type==1?MM_TANK:MM_AIR;sbOn=1;sbMix=p->mix;sbEra[0]=SBEI[p->era0];sbEra[1]=SBEI[p->era1];sbCnt[0]=p->cnt0;sbCnt[1]=p->cnt1;sbWin=p->win;
 sbTarget=SBTG[p->tgt%5];sbTime=p->win==1?SBTM[p->tgt&3]*3600:p->win==2?60*60*10:60*60*5;int sb0=SET.bot;SET.bot=p->bot;g6.fac0=p->fac0;g6.fac1=p->fac1;
 int me=SBEI[p->mapE];loading(cat("SANDBOX: ",map_def(mode,me,p->mapI)->n));mpMixed=1;mp_run(mode,me,p->mapI);mpMixed=0;SET.bot=sb0;sbOn=0;REG_BLDCNT=0;REG_BLDY=0;
 sbs_load();SBS.played++;if(score[0]>score[1])SBS.won++;sbs_save();BEAT->kills=U[0].kills;bgCached=-1;}
static void sandbox_mode(void){sbs_load();static SbP P;static int init=0;if(!init){P=sbDef(0);init=1;}int r=0,redraw=1;bgm_t(TR_MARCH);
 static const char*const RL[15]={"BATTLE","MAP","BLUE FORCE","BLUE ERA","BLUE UNITS","RED FORCE","RED ERA","RED UNITS","ERA MIX","BOT SKILL","WIN BY","LIMIT","SAVE PRESET","LOAD PRESET","START BATTLE"};
 int slot=0,go=0;
 for(;;){BEAT->screen=SCR_SAND;BEAT->menu=r;BEAT->mode=P.type;BEAT->era=P.era0*16+P.era1;BEAT->x=P.cnt0;BEAT->y=P.cnt1;BEAT->extra=P.win;
  {u32 j=FR_JUMP;if((j>>16)==0x5342){FR_JUMP=0;P=sbDef(j&1);if(j&0x100)P.mix=1;r=14;redraw=1;go=1;}}
  if(redraw){redraw=0;fr_bgspace();fr_panel(4,4,232,152);m3_ctext(8,"SANDBOX BATTLE",C_YEL,2);
   for(int i=0;i<15;i++){int y=26+i*8+(i>=12?4:0);u16 c=i==r?C_YEL:C_WHITE;if(i<12){m3_text(10,y,RL[i],i==r?C_YEL:C_GRAY,1);const char*v=sb_val(&P,i);m3_text(86,y,v,c,1);}
    else m3_text(10,y,i==r?cat(">",RL[i]):RL[i],i==14?(i==r?C_GREEN:RGB(120,200,120)):c,1);}
   m3_text(110,26+12*8+4,cat("SLOT ",itoa(slot+1)),C_GRAY,1);m3_text(150,26+12*8+4,SBS.used[slot]?"SAVED":"EMPTY",SBS.used[slot]?C_GREEN:C_GRAY,1);
   m3_text(130,148,"L/R: CHANGE",C_GRAY,1);m3_present();}
  next_frame();if(go){go=0;pressed=K_A;}
  if(pressed&K_UP){r=(r+14)%15;redraw=1;sfx_menu();}if(pressed&K_DOWN){r=(r+1)%15;redraw=1;sfx_menu();}
  int dx=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;
  if(dx){redraw=1;sfx_menu();switch(r){case 0:P.type=(P.type+3+dx)%3;break;case 1:{int k=P.mapE*4+P.mapI;k=(k+28+dx)%28;P.mapE=k/4;P.mapI=k%4;break;}
   case 2:do P.fac0=(P.fac0+NFAC+dx)%NFAC;while(P.fac0==P.fac1);break;case 3:P.era0=(P.era0+7+dx)%7;break;case 4:P.cnt0=1+(P.cnt0-1+6+dx)%6;break;
   case 5:do P.fac1=(P.fac1+NFAC+dx)%NFAC;while(P.fac0==P.fac1);break;case 6:P.era1=(P.era1+7+dx)%7;break;case 7:P.cnt1=1+(P.cnt1-1+6+dx)%6;break;
   case 8:P.mix^=1;break;case 9:P.bot=(P.bot+4+dx)&3;break;case 10:P.win=(P.win+4+dx)&3;break;case 11:P.tgt=(P.tgt+5+dx)%5;break;case 12:case 13:slot=(slot+3+dx)%3;break;}}
  if(pressed&(K_A|K_START)){pressed=0;if(r==12){SBS.p[slot]=P;SBS.used[slot]=1;sbs_save();sfx_ok();redraw=1;}else if(r==13){if(SBS.used[slot]){P=SBS.p[slot];sfx_ok();}else sfx_menu();redraw=1;}
   else if(r==14){sfx_ok();sb_launch(&P);bgm_t(TR_MARCH);redraw=1;if(FR_AUTO)return;}else{r=14;redraw=1;}}
  if(pressed&K_B){pressed=0;return;}}}
static void rec_sand(const char**L){sbs_load();static char b[4][40] EWRAM;{char*o=sp_put(b[0],"BATTLES ");o=sp_int(o,SBS.played);o=sp_put(o,"  WON ");sp_int(o,SBS.won);L[0]=b[0];}
 for(int i=0;i<3;i++){char*o=sp_put(b[i+1],"PRESET ");o=sp_int(o,i+1);o=sp_put(o,": ");if(SBS.used[i]){SbP*p=&SBS.p[i];o=sp_put(o,SBERA[p->era0]);o=sp_put(o," V ");sp_put(o,SBERA[p->era1]);}else sp_put(o,"EMPTY");L[i+1]=b[i+1];}}
