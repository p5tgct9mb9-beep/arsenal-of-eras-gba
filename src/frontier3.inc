// ===================================================================== ROCKET BUILDER (KSP-lite, real engine data)
// Stack up to 3 stages: engine + count + tank. Delta-v by Tsiolkovsky: dv = Isp * g0 * ln(m0/mf); stage 1 uses the mean of
// sea-level and vacuum Isp. The design is then flown with the Space Launch physics (l_ascent) - SELECT = autopilot.
typedef struct{const char*n;u16 tsl,tvac;u16 isl,ivac;u16 kg;u16 cost;}REng; // thrust kN, Isp s, dry mass kg, cost $M
static const REng RENG[11]={{"NONE",0,0,0,0,0,0},{"MERLIN 1D",845,914,282,311,470,1},{"MERLIN VAC",0,981,0,348,490,2},{"RAPTOR 2",2256,2450,327,350,1630,2},
 {"RAPTOR VAC",0,2530,0,380,2000,3},{"RS-25",1860,2279,366,452,3527,40},{"F-1",6770,7770,263,304,8400,30},{"J-2",0,1033,0,421,1788,15},
 {"RD-180",3830,4150,311,338,5480,25},{"RL10",0,110,0,465,300,10},{"RUTHERFORD",24,26,303,343,35,1}};
#define NRENG 11
static const u16 RTANK[8]={5,15,40,100,200,400,800,1200}; // propellant tonnes; dry mass = 6% + 1 t
static const char*const RPAY[5]={"CUBESAT 10 KG","COMSAT 4 T","CREW CAPSULE 12 T","LUNAR LANDER 16 T","STATION MODULE 50 T"};
static const u16 RPAYT[5]={1,400,1200,1600,5000}; // x10 kg
static const char*const RTGT[6]={"SUBORBITAL HOP","LOW EARTH ORBIT","HEAVY LEO (50 T)","GEO TRANSFER","TRANS-LUNAR","MARS TRANSFER"};
static const u16 RTDV[6]={2000,9400,9400,11840,12550,13000}; // ideal dv from the pad incl. gravity/drag losses, m/s
typedef struct{u8 e[3],c[3],t[3],pay,tgt;}RDes;
static RDes rdes={{1,2,0},{9,1,1},{5,3,0},1,1};
static const RDes RPRE[4]={{{1,2,0},{9,1,1},{5,3,0},1,1},{{6,7,7},{5,5,1},{7,5,3},3,4},{{10,0,0},{4,1,1},{0,0,0},0,0},{{3,4,0},{9,3,1},{6,5,0},4,2}};
static const char*const RPREN[4]={"F9-STYLE","SATURN-STYLE","SMALLSAT","STARSHIP-STYLE"};
static int rb_ln1000(int r1000){ // 1000*ln(r/1000), r>=1000
 int y=0;while(r1000>=2000){r1000=r1000*1000/2718;y+=1000;}int x=r1000-1000;return y+x-x*x/2000+x*x/1000*x/3000000;}
typedef struct{int dv[3],bt[3],m0[3],tw,tot,ns,cost;}RCalc;
static void rb_calc(const RDes*D,RCalc*R){int pay=D->tgt==2?5000:RPAYT[D->pay];int above=pay; // tonnes*100 (10 kg units)
 for(int s=0;s<3;s++){R->dv[s]=R->bt[s]=R->m0[s]=0;}R->ns=0;R->cost=0;
 int ms[3],mp[3];for(int s=0;s<3;s++){const REng*E=&RENG[D->e[s]];if(!D->e[s]){ms[s]=mp[s]=0;continue;}int p=RTANK[D->t[s]]*100;mp[s]=p;ms[s]=p*6/100+100+E->kg*D->c[s]/10;R->cost+=E->cost*D->c[s]+RTANK[D->t[s]]/20+1;}
 for(int s=2;s>=0;s--){if(!D->e[s])continue;const REng*E=&RENG[D->e[s]];int m0=above+ms[s]+mp[s],mf=above+ms[s];R->m0[s]=m0;
  int isp=s==0?(E->isl?(E->isl+E->ivac)/2:E->ivac/2):E->ivac;int ln=rb_ln1000((int)((long long)m0*1000/mf));R->dv[s]=isp*981/100*ln/1000;
  int thr=(s==0&&E->tsl?E->tsl:E->tvac)*D->c[s];R->bt[s]=thr?(int)((long long)mp[s]*isp*981/((long long)thr*10000)):0;above=m0;}
 for(int s=0;s<3;s++)if(D->e[s])R->ns=s+1;
 {const REng*E=&RENG[D->e[0]];int thr=E->tsl*D->c[0];R->tw=R->m0[0]?(int)((long long)thr*1000000/((long long)R->m0[0]*981)):0;} // TWR x100: kN / (m0[10 kg] * 0.0981)
 R->tot=0;for(int s=0;s<3;s++)R->tot+=R->dv[s];}
static Rk rbRk;static char rbName[20];
static int rb_fly(const RDes*D,const RCalc*R){Rk*k=&rbRk;const char*nm="YOUR ROCKET";int i=0;while(nm[i]){rbName[i]=nm[i];i++;}rbName[i]=0;k->n=rbName;k->f="CUSTOM DESIGN";k->ag=0;k->nb=0;k->top=D->pay>=2?2:0;k->craft=D->pay>=2?3:0;k->land=0;k->kind=D->tgt==0?LK_SUB:LK_ORBIT;k->pad=D->tgt==0?50:0;k->bw=k->bh=0;k->bc=0;
 int n=0;for(int s=0;s<3;s++)if(D->e[s]){int dv=R->dv[s]/10,bt=R->bt[s];if(bt<20)bt=20;if(bt>600)bt=600;if(dv<10)dv=10;
   k->w[n]=D->t[s]>=6?14:D->t[s]>=4?12:D->t[s]>=2?10:7;k->h[n]=12+D->t[s]*5;k->c[n]=n==0?RGB(236,236,232):n==1?RGB(214,214,220):RGB(200,200,206);k->dv[n]=dv;k->bt[n]=bt;n++;}
 if(!n)return 0;k->ns=n;for(int s=n;s<3;s++){k->w[s]=k->h[s]=0;k->c[s]=0;k->dv[s]=0;k->bt[s]=1;}
 lScore=0;lAuto=FR_AUTO?1:0;lIgn=0;lHull=3;lMsgT=0;return l_ascent(k,-1);}
static void rb_stack(const RDes*D,int cx,int by){int y=by;for(int s=0;s<3;s++){if(!D->e[s])continue;int h=10+D->t[s]*4,w=D->t[s]>=6?16:D->t[s]>=4?13:D->t[s]>=2?10:7;
  lf(cx-w/2,y-h,w,h,s==0?RGB(236,236,232):RGB(210,212,220));lf(cx-w/2,y-h,1,h,RGB(255,255,255));lf(cx+w/2-2,y-h,2,h,RGB(150,150,160));lf(cx-w/2,y-h,w,1,RGB(60,60,66));
  int c=D->c[s];for(int k=0;k<c&&k<9;k++)lf(cx-w/2+1+(k*(w-2))/(c>1?c:1),y,2,3,RGB(70,70,76));y-=h;}
 int pw=D->pay>=4?16:D->pay>=2?10:7;for(int j=0;j<8;j++){int ww=pw*(j+1)/8;lf(cx-ww/2,y-8+j,ww?ww:1,1,D->pay>=2?RGB(180,186,196):RGB(236,236,232));}if(D->pay>=2)lf(cx,y-16,1,8,RGB(200,90,60));}
static void rocket_builder(void){v131_load();int sel=0,redraw=1,pre=0;
 {u32 j=FR_JUMP;if((j&0xFFFFFF00)==0x46524A00){FR_JUMP=0;int t=j&255;pre=t==0?2:t==1?0:t==2?3:1;rdes=RPRE[pre];rdes.tgt=t;sel=13;}}
 for(;;){RCalc R;rb_calc(&rdes,&R);int need=RTDV[rdes.tgt];int ok=R.tot>=need&&R.tw>=120&&R.ns>0;
  BEAT->screen=SCR_RB;BEAT->menu=sel;BEAT->scoreA=R.tot;BEAT->scoreB=R.tw;BEAT->extra=rdes.tgt;
  if(redraw){redraw=0;fr_bgspace();fr_panel(2,2,140,156);fr_panel(146,2,92,156);m3_text(8,6,"ROCKET BUILDER",C_YEL,1);
   static const char*const LB[3]={"ENGINE","COUNT ","TANK  "};
   for(int r=0;r<14;r++){int y=17+r*9;u16 c=r==sel?C_YEL:C_WHITE;const char*v="";char b[24];
    if(r<9){int s=2-r/3,f=r%3;if(f==0){m3_text(6,y,cat("S",itoa(s+1)),RGB(110,200,230),1);}
     v=f==0?RENG[rdes.e[s]].n:f==1?itoa(rdes.c[s]):cat(itoa(RTANK[rdes.t[s]])," T PROP");if(!rdes.e[s]&&f)v="-";
     m3_text(20,y,LB[f],r==sel?C_YEL:C_GRAY,1);m3_text(62,y,v,c,1);}
    else if(r==9){m3_text(6,y,"PAYLOAD",r==sel?C_YEL:C_GRAY,1);m3_text(52,y,rdes.tgt==2?"MODULE 50 T":RPAY[rdes.pay],c,1);}
    else if(r==10){m3_text(6,y,"TARGET",r==sel?C_YEL:C_GRAY,1);m3_text(52,y,RTGT[rdes.tgt],c,1);}
    else if(r==11){m3_text(6,y,"PRESET",r==sel?C_YEL:C_GRAY,1);m3_text(52,y,RPREN[pre],c,1);}
    else if(r==12){m3_text(6,y,VS.rbDone[rdes.tgt]?"REACHED *":"",C_GREEN,1);}
    else m3_text(6,y,r==sel?"> LAUNCH!":"  LAUNCH!",ok?C_GREEN:C_RED,1);(void)b;}
   rb_stack(&rdes,192,92);
   m3_text(150,96,cat("DV ",itoa(R.tot)),R.tot>=need?C_GREEN:C_RED,1);m3_text(150,105,cat("NEED ",itoa(need)),C_GRAY,1);
   {char b[12];int t=R.tw;b[0]='0'+t/100%10;b[1]='.';b[2]='0'+t/10%10;b[3]='0'+t%10;b[4]=0;m3_text(150,114,cat("TWR ",b),R.tw>=120?C_GREEN:C_RED,1);}
   m3_text(150,123,cat3("COST ",itoa(R.cost),"M USD"),C_WHITE,1);
   for(int s=0;s<3;s++)if(rdes.e[s])m3_text(150,132+s*8-(R.ns>2?0:0),cat3("S",itoa(s+1),cat(" ",itoa(R.dv[s]))),C_GRAY,1);
   m3_text(8,148,"UP/DN L/R  A:OK  B:BACK",C_GRAY,1);m3_present();}
  next_frame();int d=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;
  if(pressed&K_UP){sel=(sel+13)%14;if(sel==12)sel=11;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%14;if(sel==12)sel=13;redraw=1;sfx_menu();}
  if(d){if(sel<9){int s=2-sel/3,f=sel%3;if(f==0)rdes.e[s]=(rdes.e[s]+NRENG+d)%NRENG;else if(f==1)rdes.c[s]=clampi(rdes.c[s]+d,1,9);else rdes.t[s]=(rdes.t[s]+8+d)&7;if(s==0&&rdes.e[0]==0)rdes.e[0]=1;}
   else if(sel==9)rdes.pay=(rdes.pay+5+d)%5;else if(sel==10)rdes.tgt=(rdes.tgt+6+d)%6;else if(sel==11){pre=(pre+4+d)%4;int t=rdes.tgt;rdes=RPRE[pre];rdes.tgt=t;}redraw=1;sfx_menu();}
  if(pressed&K_B){sfx_menu();return;}
  if(sel==13&&(pressed&(K_A|K_START))){if(R.tw<120){guide_tip("TOO HEAVY TO LIFT","LIFTOFF THRUST MUST BEAT WEIGHT BY 20% OR MORE. ADD ENGINES TO STAGE 1 OR SHRINK THE TANKS.");redraw=1;continue;}
   if(RENG[rdes.e[0]].tsl==0){guide_tip("VACUUM ENGINE","THAT ENGINE IS BUILT FOR VACUUM - ITS NOZZLE CANNOT WORK AT SEA LEVEL. USE IT ON AN UPPER STAGE.");redraw=1;continue;}
   int r=rb_fly(&rdes,&R);int reached=r==1&&R.tot>=need;
   int sc=reached?5000+(R.tot-need)/4+(RPAYT[rdes.pay]/10)-R.cost*5:lScore/4;if(sc<0)sc=0;int nb=0;
   if(reached){if(!VS.rbDone[rdes.tgt]){VS.rbDone[rdes.tgt]=1;int n=0;for(int i=0;i<6;i++)n+=VS.rbDone[i];VS.prog[3]=n;}if(sc>VS.rbBest[rdes.tgt])VS.rbBest[rdes.tgt]=sc;v131_save();nb=fr_best(3,sc);}
   fr_result(reached?"TARGET REACHED":r==1?"ORBIT, NOT ENOUGH DV":"MISSION FAILED",RTGT[rdes.tgt],fr_s2("DESIGN DV ",R.tot," M/S, NEED ",need),reached,sc,nb);
   BEAT->screen=SCR_RB;BEAT->phase=reached?3:4;redraw=1;bgm_t(TR_SPACE);if(FR_AUTO){for(int w=0;w<600;w++)next_frame();return;}}}}
