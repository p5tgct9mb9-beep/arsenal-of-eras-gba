// ds_hooks.inc - appended to gen/game.c by tools/port.py (DS build only).
// Small read-only accessors that let the DS touch UI label its buttons with the game's own menus.
int ds_ui_query(int scr,int key,int idx,const char**l1,const char**l2){
 *l1=0;*l2=0;
 if(scr==SCR_TILES){if(idx<0||idx>=TI_N)return TI_N;*l1=TL[idx].n;*l2=TL[idx].tag;return TI_N;}
 if((scr==SCR_CAT||scr==SCR_HUB)&&key>=CAT_N&&key<CAT_N+TI_N){const MCat*C=&MC3[key-CAT_N];int nm=C->nm;
  if(idx>=100){int sel=BEAT->menu;if(sel<0||sel>=nm)sel=0;int md=C->m[0]==MD_CAMP?MD_CAMP:C->m[sel];u8 ops[8];int no=mode_ops(md,ops);
   if(MD_PSEUDO(md)){if(idx==100)*l1="OPEN";return 1;}
   if(idx-100<no)*l1=op_label(md,ops[idx-100]);return no;}
  if(idx<0||idx>=nm)return nm;
  if(C->m[0]==MD_CAMP){static const char*const EN[4]={"WORLD WAR II","COLD WAR","MODERN","WHAT-IF"};*l1=EN[idx&3];}
  else *l1=mln(C->m[idx]);
  return nm;}
 return 0;}

// ---- tactical panel (bottom screen) for the shared-unit arena modes: infantry, armor, air, naval, campaign
typedef struct{s16 x,y;u8 team,kind;u16 ang;}DsBlip;
typedef struct{int mw,mh,camx,camy,maxhp,hp,gren,fuel,alive,resp,ang,nb;const char*wpn;}DsTac;
int ds_tac(int scr,DsTac*t,DsBlip*b,int max){
 if(!(scr==SCR_INF||scr==SCR_TANK||scr==SCR_AIR||scr==SCR_SEA||scr==SCR_CAMP))return 0;
 if(mapW<=0||mapH<=0||mapW>MAXW||mapH>64)return 0;
 Unit*P=&U[0];t->mw=mapW;t->mh=mapH;t->camx=camX;t->camy=camY;t->maxhp=P->maxhp;t->hp=P->hp;t->gren=P->gren;t->fuel=P->fuel;
 t->alive=P->alive;t->resp=P->resp;t->ang=P->ang;t->wpn=0;
 if((scr==SCR_INF||scr==SCR_CAMP)&&P->wpn<3)t->wpn=PW[P->wpn].n;
 int n=0;for(int k=0;k<NU&&n<max;k++){Unit*u=&U[k];if(!u->alive||u->team>1)continue;
  b[n].x=u->x>>8;b[n].y=u->y>>8;b[n].team=k==0?2:u->team;b[n].kind=0;b[n].ang=u->ang;n++;}
 t->nb=n;return 1;}
// cell id at (cx,cy) and the average colour of each map metatile, read back from the BG tiles and palette
// actually on screen (falls back to flag colours when the map layer cannot be identified)
int ds_tac_cell(int cx,int cy){return map[cy*MAXW+cx]&31;}
int ds_tac_flags(int t){return tflag[t&31];}
int ds_tac_colours(u16*col){
 int px=(U[0].x>>8)>>4,py=(U[0].y>>8)>>4;if(px<0||py<0||px>=mapW||py>=mapH)return 0;
 int t=map[py*MAXW+px];
 if(ds_ter_on){int cb=(REG_BGCNT(1)>>2)&3; // DS painted terrain: average each metatile's variant 0 in its own bank
  for(int m=0;m<32;m++){u16 e=ds_ter_entry(m,0,0,(u16)((m*4)|(mtBank[m]<<12)));int pb=e>>12,tb=e&0x3FF;int r=0,g=0,bl=0,nn=0;
   for(int q=0;q<4;q++){volatile u32*T=(volatile u32*)(0x06000000+cb*0x4000+(tb+q)*32);
    for(int w=0;w<8;w++){u32 v=T[w];for(int s=0;s<8;s+=2){int ix=(v>>(s*4))&15;if(!ix)continue;u16 c=PAL_BG[pb*16+ix];r+=c&31;g+=(c>>5)&31;bl+=(c>>10)&31;nn++;}}}
   col[m]=nn?(u16)((r/nn)|((g/nn)<<5)|((bl/nn)<<10)):PAL_BG[0];}
  return 1;}
 for(int bg=0;bg<4;bg++){u16 cnt=REG_BGCNT(bg);int sb=(cnt>>8)&31,cb=(cnt>>2)&3;
  volatile u16*S=(volatile u16*)(0x06000000+sb*0x800);u16 e=S[((py*2)&31)*32+((px*2)&31)];
  if((e&0x3FF)!=(u16)(t*4))continue;
  int bank=(e>>12)-mtBank[t&31];
  for(int m=0;m<32;m++){int r=0,g=0,bl=0,nn=0;int pb=(bank+mtBank[m])&15;
   for(int q=0;q<4;q++){volatile u32*T=(volatile u32*)(0x06000000+cb*0x4000+(m*4+q)*32);
    for(int w=0;w<8;w++){u32 v=T[w];for(int s=0;s<8;s+=2){int ix=(v>>(s*4))&15;if(!ix)continue;u16 c=PAL_BG[pb*16+ix];r+=c&31;g+=(c>>5)&31;bl+=(c>>10)&31;nn++;}}}
   col[m]=nn?(u16)((r/nn)|((g/nn)<<5)|((bl/nn)<<10)):PAL_BG[0];}
  return 1;}
 return 0;}

// ---- Grand Prix track map for the bottom screen
typedef struct{int n,laps,phase,me;const u8*surf;s16 x[8],y[8];u8 lap[8],order[8];const char*name[8];u16 h;}DsRace;
int ds_race(DsRace*r){if(!rcSurf||rcN<=0||rcN>8)return 0;r->n=rcN;r->laps=rcLaps;r->phase=rcPhase;r->me=0;r->surf=rcSurf;r->h=rc[0].h;
 for(int i=0;i<rcN;i++){r->x[i]=rc[i].x>>(RQ+3);r->y[i]=rc[i].y>>(RQ+3);r->lap[i]=rc[i].lap;r->order[i]=rcOrder[i];r->name[i]=RCARS[rc[i].car].n;}
 return 1;}
