// ===================================================================== CAMPAIGN: DOG GREEN
static const char*const CMAP[]={
"gggGgggggggGggg","ggggggcccgggggg","gGggbbcccbbggGg","gggbbcqrycbbggg","ggbbccamdccbbgg",
"gggggcuuucggggg","gGgggguuugggGgg","hhhhhhuuuhhhhhh","ggbbgbbugbbgbbg","ttttttttttttttt",
"gggGgguugggGggg","gGgggguuuggggGg","hhhhhhhuhhhhhhh","GgggggguggggggG","ggGggg.u.gggGgg",
"...,....,....,.","XXXXXXXXXXXXXXX","..,.....,....,.",".o.....,...o...","SSS.SSSSSSS.SSS",
"...,.......,...",".,.....o....,..","..H....,...H...","......H........",".H..,.....,..H.",
"....o...H......","..,..H.....,...",".........H...H.",".H..,..o.......","....H.....H...,",
"..,.....,......","......H..,..H..",".,H.......o....",".....,..H....H.","...H.........,.",
",......H...,...",".o..,...,.H....","..H......o.....","eeeeeeeeeeeeeee","eeHeeeeeeeeHeee",
"sssssssssssssss","wwwwwwwwwwwwwww","wwHwwwwwwwwwHww","wpppwwpppwwpppw","~LfJ~~LfJ~~LfJ~",
"~LfJ~~LfJ~~LfJ~","~zzz~~zzz~~zzz~","~~~~~~~~~~~~~~~","~~~~~~~~~~~~~~~"};
#define CROWS 49
static int cmchar(char c){switch(c){case '~':return CM_DEEP;case 'w':return CM_WATER;case 's':return CM_SURF;case 'e':return CM_WET;case '.':return CM_SAND;
 case ',':return CM_SAND2;case 'H':return CM_HEDGEHOG;case 'o':return CM_CRATER;case 'S':return CM_SHINGLE;case 'X':return CM_WIRE;case 'g':return CM_GRASS;
 case 'G':return CM_GRASS2;case 't':return CM_TRENCH;case 'b':return CM_BAGS;case 'q':return CM_BK_TL;case 'r':return CM_BK_T;case 'y':return CM_BK_TR;
 case 'a':return CM_BK_BL;case 'm':return CM_BK_EMB;case 'd':return CM_BK_BR;case 'L':return CM_BOAT_L;case 'J':return CM_BOAT_R;case 'f':return CM_BOAT_F;
 case 'p':return CM_BOAT_RAMP;case 'z':return CM_BOAT_ST;case 'u':return CM_DIRT;case 'c':return CM_CONC;case 'h':return CM_BLUFF;}return CM_SAND;}
typedef struct{const char*who;const char*l[3];}Msg;
static const Msg BRIEF[]={
 {"OPERATION OVERLORD",{"OMAHA BEACH, SECTOR DOG GREEN","6 JUNE 1944 - 0630 HOURS","PRESS A TO CONTINUE"}},
 {"LT. MORROW",{"CPL. HALE, LISTEN UP. THE","MG BUNKER ON THE BLUFF HAS","THIS WHOLE BEACH ZEROED."}},
 {"LT. MORROW",{"GET TO THE SEAWALL, BLOW THE","WIRE WITH THE BANGALORE, THEN","GRENADE THAT BUNKER."}},
 {"LT. MORROW",{"HEDGEHOGS AND THE SEAWALL","STOP BULLETS. USE THEM.","A FIRE  B GRENADE  R STRAFE"}},
};
static const Msg RADIO[]={
 {"COXSWAIN",{"RAMP GOING DOWN!","GO GO GO!",""}},
 {"SGT. KOWALSKI",{"MG FIRE FROM THE BLUFF!","MOVE HEDGEHOG TO HEDGEHOG!",""}},
 {"LT. MORROW",{"HALE! BANGALORE UP TO THE","WIRE - MARKER DEAD CENTER!","PRESS A AT THE ARROW."}},
 {"HALE",{"FIRE IN THE HOLE!","GET BACK!",""}},
 {"LT. MORROW",{"WIRE'S CUT! UP THE BLUFF!","GRENADE THAT BUNKER (B)!",""}},
 {"SGT. KOWALSKI",{"BUNKER'S DOWN! DOG GREEN","IS OPEN! NICE WORK, HALE!",""}},
 {"MEDIC",{"HALE'S HIT! DRAG HIM BACK -","HE'S STILL BREATHING.",""}},
 {"SGT. KOWALSKI",{"GRENADE CRATE! RESUPPLIED.","",""}},
};
static int radioId=-1,radioT;
static void radio(int id){radioId=id;radioT=230;}
static void draw_msg(const Msg*m){hpanel(0,15,30,5);hpanel(0,10,5,5);hptxt(5,14,m->who,HB_YEL);
 spr_aff(20,100,(CH.use&&m->who[0]=='Y')?CH_TILE:OT_ERA+ES_SOLDIER,1,(CH.use&&m->who[0]=='Y')?6:m->who[0]=='Y'?2:0,0,0x80,0,0); // speaker portrait in a framed box
 for(int i=0;i<3;i++)hptxt(1,16+i,m->l[i],HB_WHITE);}
static void camp_complete(int t,int kills,int downs,int grenades);
#define NEN 6
static int campKills,campWon;
static void camp_xdmg(int k,int dmg,int o){Unit*e=&U[k];if(k<5||!e->alive)return;e->hp-=dmg;if(e->hp<=0){e->alive=0;campKills++;fx_add(e->x>>8,e->y>>8,FXK_SMOKE);}}
static int smoke_blocks(int x0,int y0,int x1,int y1){for(int k=0;k<4;k++){if(smT[k]<=0)continue;for(int i=1;i<16;i++){int x=x0+(x1-x0)*i/16,y=y0+(y1-y0)*i/16;if(idist(x-smX[k],y-smY[k])<26)return 1;}}return 0;}
static void campaign(void){
 nU=12;msOn=0;
 BEAT->mode=0;BEAT->era=0;
 tflag[CM_DEEP]=TF_SOLID;tflag[CM_WATER]=TF_SLOW;tflag[CM_SURF]=TF_SLOW;
 for(int i=0;i<32;i++)tflag[i]=0;tflag[CM_DEEP]=TF_SOLID;tflag[CM_WATER]=TF_SLOW;tflag[CM_SURF]=TF_SLOW;
 tflag[CM_HEDGEHOG]=TF_SOLID|TF_SHOT;tflag[CM_SHINGLE]=TF_SOLID|TF_SHOT;tflag[CM_WIRE]=TF_SOLID;tflag[CM_BAGS]=TF_SOLID|TF_SHOT;
 for(int t=CM_BK_TL;t<=CM_RUBBLE;t++)tflag[t]=TF_SOLID|TF_SHOT;tflag[CM_BOAT_L]=tflag[CM_BOAT_R]=tflag[CM_BOAT_ST]=TF_SOLID|TF_SHOT;
 mapW=15;mapH=CROWS;mapWrap=0;borderTile=CM_DEEP;
 for(int y=0;y<CROWS;y++)for(int x=0;x<15;x++)map[y*MAXW+x]=cmchar(CMAP[y][x]);
 setup_mode0(camp_tiles,sizeof(camp_tiles)/4,camp_pal,0,0);
 memset(U,0,sizeof U);memset(S,0,sizeof S);memset(FXs,0,sizeof FXs);
 // U[0]=player, 1..4 allies, 5..10 enemy riflemen
 bgm(4,0);Unit*P=&U[0];
 int cpx[3]={7*16+8,7*16+8,7*16+8},cpy[3]={45*16+4,20*16+8,15*16+8};int cp=0;
 mpMode=MM_INF;mpEra=0;load_player(0);pSpeed=280;
 xdmg=camp_xdmg;memset(smT,0,sizeof smT);memset(zn,0,sizeof zn);pMark=0;campKills=0;
 P->x=cpx[0]<<8;P->y=cpy[0]<<8;P->alive=1;P->hp=P->maxhp=(pPerk==4?130:100)+pArmHP;P->isp=1;P->ang=0;P->gren=4+(PF_(AF_UGL)||PF_(AF_RGREN)?2:0);refill(P);P->tact=pTact==0?2:1;P->wpn=0;
 for(int i=1;i<=4;i++){Unit*a=&U[i];a->alive=1;a->team=0;a->x=((i<=2?1+i:10+i-2)*16+8)<<8;a->y=(45*16+4)<<8;a->ang=0;a->think=rr(60);a->gx=a->x;a->gy=(20*16+10)<<8;a->hp=100;}
 static const u8 ecol[NEN]={1,4,7,10,13,7};static const u8 erow[NEN]={9,9,9,9,9,6};
 for(int i=0;i<NEN;i++){Unit*e=&U[5+i];e->alive=1;e->team=1;e->x=(ecol[i]*16+8)<<8;e->y=(erow[i]*16+8)<<8;e->ang=128;e->hp=60;e->cd=60+rr(90);}
 int phase=0,bunkerHP=3,mgT=0,mgBurst=0,planted=0,fuse=0,endT=0,downT=0,t=0,downs=0,thrown=0,mgOn=1,briefPg=0,saidMG=0,crateCd=0;
 int ex=7*16+8,ey=4*16+10; // embrasure
 camY=(cpy[0]-100);
 for(;;){
  BEAT->screen=SCR_CAMP;BEAT->phase=phase;BEAT->hp=P->hp;BEAT->x=P->x>>8;BEAT->y=P->y>>8;BEAT->kills=campKills;BEAT->deaths=downs;BEAT->extra=bunkerHP;BEAT->timer=t;
  int brief=briefPg<4;
  if(brief){if(pressed&(K_A|K_START)){briefPg++;sfx_menu();if(briefPg==4)radio(0);}}
  else if(pressed&K_START){if(pause_menu())return;}
  if(!brief&&endT==0)t++;
  // ---- player
  if(!brief&&P->alive){
   int dx=0,dy=0;if(keys&K_LEFT)dx--;if(keys&K_RIGHT)dx++;if(keys&K_UP)dy--;if(keys&K_DOWN)dy++;
   int spd=(flag_at(P->x>>8,P->y>>8)&TF_SLOW)?150:280;spd=spd*PW[P->wpn].move/100;
   if(P->rl>0&&--P->rl==0)P->mag[P->wpn]=PW[P->wpn].mag;
   if(dx||dy){int a=atan2i(dx,dy);if(!(keys&K_R))P->tur=P->ang=a<<8;if(dx&&dy)spd=spd*181/256;move_unit(P,dx*spd,dy*spd,5);P->anim++;}
   int px=P->x>>8,py=P->y>>8;
   int nearMarker=!planted&&phase>=1&&idist(px-(7*16+8),py-(17*16+8))<20;
   if(P->cd>0)P->cd--;
   if(pressed&K_A&&nearMarker){planted=1;fuse=150;radio(3);sfx_ok();}
   else if(keys&K_A)inf_fire(0);
   if(pressed&K_L){next_wpn(P);sfx_menu();}
   if(pressed&K_B&&P->gren>0){throw_gren(0,P->tur>>8);thrown++;}
   if((keys&(K_L|K_R))==(K_L|K_R)&&(pressed&K_SEL)){ // debug warp (test harness / practice)
    static const s16 wy[3]={20*16+8,17*16+8,6*16+8};int ph=phase>2?2:phase;if(phase==1&&planted==2)ph=2;
    P->x=(7*16+8)<<8;P->y=wy[ph]<<8;P->tur=P->ang=0;P->inv=90;}
   else if(pressed&K_SEL)use_tact(0);
   if(P->inv>0)P->inv--;
   if(++P->regen>200&&P->hp<P->maxhp&&(t&3)==0)P->hp++;
   // checkpoints & triggers
   if(phase==0&&py<38*16&&!saidMG){saidMG=1;radio(1);}
   if(phase==0&&py<21*16){phase=1;cp=1;radio(2);}
   if(crateCd>0)crateCd--;
   if(P->gren<4&&crateCd==0&&((idist(px-(2*16+8),py-(20*16+8))<14)||(idist(px-(12*16+8),py-(14*16+8))<14))){P->gren=4;P->tact=pTact==0?2:1;refill(P);crateCd=120;radio(7);sfx_ok();}
  }
  if(!P->alive&&--downT<=0){P->alive=1;P->hp=100;P->x=cpx[cp]<<8;P->y=cpy[cp]<<8;P->inv=120;P->regen=0;P->hp=P->maxhp;if(P->gren<2)P->gren=2;if(P->tact<1)P->tact=1;refill(P);P->rl=0;}
  // ---- bangalore
  if(planted==1){if(--fuse<=0){planted=2;sfx_boom();for(int c=4;c<=10;c++){map[16*MAXW+c]=CM_WIREB;fx_add(c*16+8,16*16+8,FXK_BIG);fx_add(c*16+8,16*16+2,FXK_SMOKE);}
   if(P->alive&&idist((P->x>>8)-(7*16+8),(P->y>>8)-(16*16+8))<32){P->hp-=50;P->regen=0;sfx_hurt();}
   phase=2;cp=2;radio(4);}}
  // ---- MG bunker
  if(!brief&&mgOn&&P->alive&&(P->y>>8)<ey+560&&endT==0&&!smoke_blocks(ex,ey,P->x>>8,P->y>>8)){
   if(mgBurst>0){if(--mgT<=0){mgT=5;mgBurst--;Shot*s=shot_new();if(s){int a=atan2i((P->x>>8)-ex,(P->y>>8)-ey)+rr(13)-6;s->x=ex<<8;s->y=(ey+6)<<8;
       s->vx=SIN(a)*7/2;s->vy=-COS(a)*7/2;s->life=200;s->dmg=20;s->team=1;s->kind=K_BUL;s->dist=0;fx_add(ex,ey+6,FXK_FLASH);if((P->y>>8)<ey+300)sfx_shot(0);}}}
   else if(--mgT<=-70){mgBurst=8;mgT=0;}}
  // ---- allies (decor; advance to cover lines)
  for(int i=1;i<=4;i++){Unit*a=&U[i];if(brief)break;if(a->think>0){a->think--;continue;}
   if(phase>=2){static const u8 wy[5]={0,20,18,17,13};int gc=a->mode<=2?(i<=2?3:11):a->mode==3?7:(4+i);
    a->gx=(gc*16+8)<<8;a->gy=(wy[a->mode]*16+8)<<8;if(a->mode==0){a->mode=1;}
    if(idist((a->gx-a->x)>>8,(a->gy-a->y)>>8)<=6&&a->mode<4)a->mode++;}
   int dx=(a->gx-a->x)>>8,dy=(a->gy-a->y)>>8;
   if(idist(dx,dy)>6){int an=atan2i(dx,dy);int sp=(flag_at(a->x>>8,a->y>>8)&TF_SLOW)?120:220;
    if(!move_unit(a,SIN(an)*sp>>8,-COS(an)*sp>>8,5)){a->stuck++;if(a->stuck>10){move_unit(a,(i&1?1:-1)*256,0,5);}}else a->stuck=0;
    a->ang=an<<8;a->anim++;
    if(rr(200)==0)a->think=30+rr(60); // dive for cover briefly
   }else{ a->ang=0;
    if(phase>=2&&rr(40)==0){for(int k=5;k<5+NEN;k++)if(U[k].alive&&los(a->x>>8,a->y>>8,U[k].x>>8,U[k].y>>8)&&idist((U[k].x-a->x)>>8,(U[k].y-a->y)>>8)<170){
      Shot*s=shot_new();if(s){int an=atan2i((U[k].x-a->x)>>8,(U[k].y-a->y)>>8)+rr(9)-4;s->x=a->x;s->y=a->y;s->vx=SIN(an)*5;s->vy=-COS(an)*5;s->life=40;s->dmg=20;s->team=0;s->kind=K_BUL;a->ang=an<<8;}break;}}}
   if(phase>=1&&a->gy==(20*16+10)<<8){} }
  // ---- enemy riflemen
  for(int k=5;k<5+NEN;k++){Unit*e=&U[k];if(!e->alive||brief)continue;if(e->stun>0){e->stun--;continue;}int dx=(P->x-e->x)>>8,dy=(P->y-e->y)>>8;
   if(P->alive&&idist(dx,dy)<190){e->ang=atan2i(dx,dy)<<8;if(--e->cd<=0){e->cd=70+rr(60);
     if(los(e->x>>8,e->y>>8,P->x>>8,P->y>>8)&&!(stealth&&idist(dx,dy)>stealthR)){Shot*s=shot_new();if(s){int an=(e->ang>>8)+rr(11)-5;s->x=e->x;s->y=e->y;s->vx=SIN(an)*4;s->vy=-COS(an)*4;s->life=60;s->dmg=15;s->team=1;s->kind=K_BUL;sfx_shot(0);fx_add((e->x>>8)+(SIN(an)*9>>8),(e->y>>8)-(COS(an)*9>>8),FXK_FLASH);}}}}}
  // ---- shots
  for(int i=0;i<NS;i++){Shot*s=&S[i];if(s->life<=0)continue;s->life--;
   if(s->kind==K_GREN){s->x+=s->vx;s->y+=s->vy;if(s->sub<20){s->vx=s->vx*15/16;s->vy=s->vy*15/16;}int gx=s->x>>8,gy=s->y>>8;int c=cell(gx>>4,gy>>4);
    int hitBunker=(c>=CM_BK_TL&&c<=CM_BK_BR);
    if(s->sub==5&&s->life<880){for(int k=5;k<5+NEN;k++)if(U[k].alive&&idist(gx-(U[k].x>>8),gy-(U[k].y>>8))<26)s->life=0;}
    if(s->sub==20||s->sub==4||s->sub==8||s->sub==9){if(gy<0||(flag_at(gx,gy)&TF_SOLID))s->life=0;for(int k=5;k<5+NEN;k++)if(U[k].alive&&idist(gx-(U[k].x>>8),gy-(U[k].y>>8))<9)s->life=0;if(s->sub==20&&(s->life&3)==0)fx_add(gx,gy,FXK_SMOKE);}
    if((s->sub==1||s->sub==6||s->sub==7)&&(flag_at(gx+(s->vx>>6),gy+(s->vy>>6))&TF_SOLID))s->vx=s->vy=0;
    if(s->life<=0||hitBunker){s->life=0;det_explosive(s);
     if((s->sub<10||s->sub==20)&&mgOn&&idist(gx-ex,gy-ey)<(s->sub==2||s->sub==4?40:30)){bunkerHP--;BEAT->extra=bunkerHP;
      if(bunkerHP<=0){mgOn=0;phase=3;radio(5);endT=200;for(int yy=3;yy<=4;yy++)for(int xx=6;xx<=8;xx++){map[yy*MAXW+xx]=CM_RUBBLE;fx_add(xx*16+8,yy*16+8,FXK_BIG);fx_add(xx*16+8,yy*16,FXK_SMOKE);}
       for(int k=5;k<5+NEN;k++)if(U[k].alive&&U[k].y<(8*16)<<8){U[k].alive=0;campKills++;}}}}
    continue;}
   s->x+=s->vx;s->y+=s->vy;s->dist+=4;int bx=s->x>>8,by=s->y>>8;
   if(by<0||by>CROWS*16||bx<0||bx>240){s->life=0;continue;}
   if(s->dist>14&&(flag_at(bx,by)&TF_SHOT)){s->life=0;fx_add(bx,by,FXK_SPARK);continue;}
   if(s->team==1){if(P->alive&&!P->inv&&idist(bx-(P->x>>8),by-(P->y>>8))<6){s->life=0;P->hp-=diffd(s->dmg);P->regen=0;P->hurt=8;sfx_hurt();
     if(P->hp<=0){P->alive=0;downs++;downT=120;radio(6);fx_add(P->x>>8,P->y>>8,FXK_SMOKE);}}}
   else for(int k=5;k<5+NEN;k++){Unit*e=&U[k];if(e->alive&&idist(bx-(e->x>>8),by-(e->y>>8))<7){s->life=0;e->hp-=s->dmg;fx_add(bx,by,FXK_SPARK);if(e->hp<=0){e->alive=0;campKills++;fx_add(e->x>>8,e->y>>8,FXK_SMOKE);}break;}}}
  if(endT>0&&--endT==0){camp_complete(t,campKills,downs,thrown);return;}
  if(radioT>0)radioT--;
  // ---- camera
  int ty=clampi((P->y>>8)-96,0,CROWS*16-160);camY+=(ty-camY)/4;if(iabs(ty-camY)<4)camY=ty;camX=0;
  // ---- render
  oam_begin();hud_clear();
  // HUD
  static const char*const OBJ[4]={"OBJ: REACH THE SEAWALL","OBJ: BLOW THE WIRE","OBJ: GRENADE THE BUNKER","BUNKER DESTROYED!"};
  htxt(0,0,OBJ[phase],phase==3?HB_GREEN:HB_YEL);
  hearts(0,19,6,P->hp,P->maxhp);inf_hud(P);
  if(phase>=2&&mgOn){htxt(20,1,"BUNKER",HB_RED);for(int i=0;i<3;i++)hput(27+i,1,i<bunkerHP?HT_ICON+2:0,15);}
  if(!P->alive)hctxt(9,"HALE IS DOWN!",HB_RED,0);
  if(P->alive&&!planted&&phase>=1&&idist((P->x>>8)-(7*16+8),(P->y>>8)-(17*16+8))<20)hctxt(12,"A: PLACE BANGALORE",HB_YEL,0);
  if(planted==1){hctxt(12,cat("FUSE: ",itoa(fuse/60+1)),HB_RED,0);}
  if(brief&&briefPg<4)draw_msg(&BRIEF[briefPg]);else if(radioT>0&&radioId>=0)draw_msg(&RADIO[radioId]);
  // sprites (front to back)
  if(phase>=1&&!planted){int by=(17*16+2)+((frame>>3)&3)-camY;spr(7*16,by-12,OT_FX+FX_ARROW,0,1,3,1);}
  if(planted==1)spr(7*16,16*16-camY,OT_FX+FX_BANG,0,1,3,1);
  spr(2*16,20*16-camY,OT_FX+FX_CRATE,0,1,3,2);spr(12*16,14*16-camY,OT_FX+FX_CRATE,0,1,3,2);
  for(int i=0;i<NS;i++){Shot*s=&S[i];if(s->life<=0)continue;int x=(s->x>>8)-camX,y=(s->y>>8)-camY;
   if(s->kind==K_GREN&&s->sub==20)spr_aff(x,y,OT_FX+FX_ROCKET,0,3,atan2i(s->vx,s->vy),0x100,1,0);else spr(x-4,y-4,OT_FX+(s->kind==K_GREN?FX_GRENADE:FX_BULLET),0,0,3,1);}
  fx_update_draw();smoke_tick_draw();zones_tick();
  if(P->alive&&!((P->inv>>2)&1))spr_aff((P->x>>8)-camX,(P->y>>8)-camY,CH.use?CH_TILE+((P->anim>>3)&1)*4:OT_ERA+ES_SOLDIER+((P->anim>>3)&1)*4,1,CH.use?6:2,P->tur>>8,0x100,1,0);
  for(int i=1;i<5+NEN;i++){Unit*u=&U[i];if(!u->alive)continue;spr_aff((u->x>>8)-camX,(u->y>>8)-camY,OT_ERA+ES_SOLDIER+((u->anim>>3)&1)*4,1,u->team?1:0,u->ang>>8,0x100,1,0);}
  if(P->hurt>0){P->hurt--;}
  draw_layer(map,bgw,camX,camY,0,0);
  present(0,0,0);}
}
static void camp_complete(int t,int kills,int downs,int gren){
 campWon=1;
 music_victory();
 m3_title(1);m3_box(20,14,200,132,1);
 m3_ctext(22,"MISSION COMPLETE",C_YEL,2);m3_ctext(44,"DOG GREEN - OMAHA BEACH 1944",C_WHITE,1);
 m3_text(40,62,cat("TIME           ",mmss(t)),C_WHITE,1);m3_text(40,74,cat("ENEMIES KIA    ",itoa(kills)),C_WHITE,1);
 m3_text(40,86,cat("TIMES DOWNED   ",itoa(downs)),C_WHITE,1);m3_text(40,98,cat("GRENADES USED  ",itoa(gren)),C_WHITE,1);
 int stars=3-(downs>0)-(downs>3)-(t>60*240);if(stars<1)stars=1;
 m3_text(40,112,"RATING",C_GRAY,1);for(int i=0;i<3;i++)m3_text(100+i*14,110,"*",i<stars?C_YEL:C_GRAY,2);
 m3_ctext(128,"HALE: \"WE'RE OFF THE BEACH.\"",C_GRAY,1);m3_ctext(138,"PRESS A",C_YEL,1);
 BEAT->screen=SCR_DONE;m3_present();
 for(int i=0;;i++){next_frame();if(i>30&&(pressed&(K_A|K_START)))break;}
 music_stop();}
