// ===================================================================== v1.3 1v1 SWORD FIGHT
// Side-view duel vs CPU: attack high/mid/low (UP+A / A / DOWN+A), hold B to guard (UP/none/DOWN = high/mid/low),
// tap B just before a blow lands = PARRY (attacker staggered), L/R = dodge (i-frames). Stamina gates attacks/dodges.
// Best of 3 rounds, 60 s each; CPU skill follows Settings > Bot Difficulty. Static arena in a bg buffer + dirty rects.
#define SCR_DSEL 48
#define SCR_DUEL 49
typedef struct{const char*n,*w,*d;u8 spd,pow,reach,stam,armor,blk;u16 c0,c1,c2,skin;u8 hat,wk;}Fighter;
#define NFTR 7
static const Fighter FTR[NFTR]={
 {"KNIGHT","LONGSWORD","HEAVY ARMOR: -20% DAMAGE TAKEN",3,12,40,100,20,3,RGB(170,176,186),RGB(120,40,40),RGB(230,230,236),RGB(226,184,150),0,0},
 {"SAMURAI","KATANA","FASTEST CUTS, LIGHT ARMOR",6,12,38,85,5,2,RGB(150,40,40),RGB(40,40,50),RGB(220,220,230),RGB(230,190,150),1,1},
 {"VIKING","BEARDED AXE","HUGE POWER, SLOW SWINGS",2,16,34,110,8,2,RGB(110,90,60),RGB(70,90,120),RGB(190,190,196),RGB(236,196,160),2,2},
 {"CONTINENTAL OFFICER","CAVALRY SABRE","BALANCED, QUICK RECOVERY",5,10,38,100,0,2,RGB(40,60,130),RGB(230,220,190),RGB(210,210,216),RGB(232,188,150),3,3},
 {"PIRATE","CUTLASS","QUICK FEET: CHEAP DODGES",6,9,34,95,0,1,RGB(150,30,40),RGB(40,40,44),RGB(200,200,206),RGB(214,166,124),4,3},
 {"ROMAN LEGIONARY","GLADIUS + SCUTUM","SHIELD: BLOCKS COST LESS",4,10,30,105,10,4,RGB(170,40,40),RGB(200,170,60),RGB(210,210,214),RGB(220,176,136),5,4},
 {"SPARTAN HOPLITE","DORY SPEAR + ASPIS","LONGEST REACH",3,11,48,100,8,4,RGB(150,30,30),RGB(200,150,60),RGB(200,180,120),RGB(214,166,124),6,5}};
static const char*const FSN[NFTR]={"KNIGHT","SAMURAI","VIKING","OFFICER","PIRATE","LEGIONARY","HOPLITE"};
static const char*const ARN[4]={"CASTLE COURTYARD","DOJO","SHIP DECK","COLOSSEUM"};
static u16 dl_bg[240*160] EWRAM;
enum{DS_IDLE,DS_WALK,DS_WIND,DS_STRIKE,DS_REC,DS_BLOCK,DS_STUN,DS_HIT,DS_DODGE,DS_KO,DS_WIN};
typedef struct{int x,face,hp,st,s,t,h,g,pw,inv,f,wins,cd,ai_t,ai_plan,flash;}Duelist;
static Duelist DL[2];
static void dl_arena(int a){u16*o=fb;
 switch(a){
  case 0:lf(0,0,240,160,RGB(120,170,230));for(int x=0;x<240;x+=40){lf(x,20,30,104,RGB(150,140,124));lf(x+30,32,10,92,RGB(126,118,104));lf(x+4,8,8,12,RGB(150,140,124));lf(x+18,8,8,12,RGB(150,140,124));}
   for(int y=24;y<124;y+=8)for(int x=((y>>3)&1)*10;x<240;x+=20)lf(x,y,19,1,RGB(110,104,92));lf(100,50,40,74,RGB(70,50,34));lf(104,54,32,70,RGB(96,70,44));
   lf(0,124,240,36,RGB(150,130,100));for(int x=0;x<240;x+=16)lf(x,124,1,36,RGB(120,104,80));lf(30,40,6,24,RGB(160,30,40));lf(204,40,6,24,RGB(40,60,150));break;
  case 1:lf(0,0,240,124,RGB(214,190,140));for(int x=0;x<240;x+=30){lf(x,0,4,124,RGB(90,60,40));for(int y=10;y<120;y+=22)lf(x+4,y,26,1,RGB(150,130,96));}
   lf(0,0,240,10,RGB(70,46,30));lf(100,30,40,30,RGB(240,236,226));lf(112,36,16,18,RGB(30,30,30));lf(0,124,240,36,RGB(170,130,80));for(int x=0;x<240;x+=24)lf(x,124,1,36,RGB(130,96,60));for(int y=132;y<160;y+=10)lf(0,y,240,1,RGB(140,104,64));break;
  case 2:{lf(0,0,240,90,RGB(250,190,130));lf(0,40,240,50,RGB(240,150,110));lf(0,90,240,34,RGB(40,80,140));for(int i=0;i<20;i++)lf((i*37)%240,94+(i%5)*6,10,1,RGB(120,170,220));
   lf(36,6,6,118,RGB(90,60,40));lf(10,16,58,40,RGB(236,230,210));lf(180,10,5,114,RGB(90,60,40));lf(160,22,46,32,RGB(236,230,210));
   lf(0,112,240,12,RGB(120,80,50));for(int x=6;x<240;x+=14)lf(x,104,3,8,RGB(120,80,50));lf(0,104,240,2,RGB(110,70,40));
   lf(0,124,240,36,RGB(160,110,70));for(int y=128;y<160;y+=8)lf(0,y,240,1,RGB(130,86,52));}break;
  default:lf(0,0,240,160,RGB(140,190,240));for(int t=0;t<3;t++){int y=30+t*28;lf(0,y,240,28,RGB(200-t*14,176-t*14,140-t*14));for(int x=(t&1)*12;x<240;x+=24){lf(x,y+4,14,16,RGB(90-t*10,70-t*10,50));}}
   for(int i=0;i<40;i++)lf((i*29)%236,40+(i*13)%60,3,4,RGB(150+(i*7)%80,80+(i*11)%80,60+(i*5)%60));
   lf(0,114,240,10,RGB(150,120,90));lf(0,124,240,36,RGB(214,186,130));for(int i=0;i<30;i++)lf((i*47)%236,128+(i*11)%28,3,1,RGB(190,160,110));}
 (void)o;dma32(fb,dl_bg,240*160/2);}
// fighters render into 8bpp 96x80 canvases (EWRAM), converted to Mode-3 OBJ tiles (5 sprites each) in vblank
#define CW 96
#define CH 80
static u8 dcv[2][CW*CH] EWRAM;static u8*cvb;static int cvx,cvy; // current canvas + screen origin
static u16 dpal[256];static int dpn;
static int dpi(u16 c){for(int i=1;i<dpn;i++)if(dpal[i]==c)return i;if(dpn>=255)return 1;dpal[dpn]=c;((volatile u16*)0x05000200)[dpn]=c;return dpn++;}
static void dpreset(void){dpn=1;}
IWCODE static void cf(int x,int y,int w,int h,u16 c){x-=cvx;y-=cvy;if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>CW)w=CW-x;if(y+h>CH)h=CH-y;if(w<=0||h<=0)return;
 int ci=dpi(c);u8*row=cvb+y*CW+x;for(int j=0;j<h;j++,row+=CW){for(int i=0;i<w;i++)row[i]=ci;}}
static const u8 SPX[5]={0,64,0,32,64},SPY[5]={0,0,64,64,64},SPW[5]={8,4,4,4,4},SPH[5]={8,8,2,2,2};
IWCODE static void cv_upload(int k){volatile u32*T=(volatile u32*)(0x06014000+k*CW*CH);const u8*cv=dcv[k];
 for(int s=0;s<5;s++)for(int ty=0;ty<SPH[s];ty++)for(int tx=0;tx<SPW[s];tx++){const u32*src=(const u32*)(cv+(SPY[s]+ty*8)*CW+SPX[s]+tx*8);for(int r=0;r<8;r++,src+=CW/4){*T++=src[0];*T++=src[1];}}}
static void cv_oam(int k,int sx,int sy,int show){volatile u16*O=(volatile u16*)0x07000000+k*5*4;int tile=512+k*(CW*CH/32);
 static const u16 SH[5]={0,2<<14,1<<14,1<<14,1<<14},SZ[5]={3<<14,3<<14,2<<14,2<<14,2<<14};
 for(int s=0;s<5;s++,O+=4){int x=sx+SPX[s],y=sy+SPY[s];if(!show||y>=160||y<=-64||x>=240||x<=-64){O[0]=1<<9;continue;}
  O[0]=(y&255)|(1<<13)|SH[s];O[1]=(x&511)|SZ[s];O[2]=tile|(0<<10);tile+=SPW[s]*SPH[s]*2;}}
static void cv_hide(void){volatile u16*O=(volatile u16*)0x07000000;for(int i=0;i<128;i++)O[i*4]=1<<9;}
static void cv_blit(int k){const u8*cv=dcv[k];for(int y=0;y<CH;y++){int sy=cvy+y;if(sy<0||sy>=160)continue;for(int x=0;x<CW;x++){int sx=cvx+x;u8 c=cv[y*CW+x];if(c&&(unsigned)sx<240)fb[sy*240+sx]=dpal[c];}}}
static void cv_begin(int k,int fx,int gy){cvb=dcv[k];cvx=fx-CW/2;cvy=gy-CH+2;u32*q=(u32*)cvb;for(int i=0;i<CW*CH/4;i++)q[i]=0;}
// fighter drawing: feet at (x,gy), facing f (+1 right). pose from state.
IWCODE static void dl_line(int x0,int y0,int x1,int y1,int th,u16 c){int dx=x1-x0,dy=y1-y0,n=iabs(dx)>iabs(dy)?iabs(dx):iabs(dy);if(!n)n=1;for(int i=0;i<=n;i++)cf(x0+dx*i/n-th/2,y0+dy*i/n-th/2,th,th,c);}
static void dl_draw(const Duelist*d,int gy){const Fighter*F=&FTR[d->f];int x=d->x>>8,f=d->face;
 int lean=0,arm=1,crouch=0,armA=0;// arm angle (deg*): 0 = forward level
 switch(d->s){case DS_WIND:lean=-2;armA=d->h==0?-70:d->h==1?-40:20;break;case DS_STRIKE:lean=3;armA=d->h==0?-20:d->h==1?0:35;break;case DS_REC:armA=d->h==2?30:10;break;
  case DS_BLOCK:armA=d->g==0?-75:d->g==1?-30:40;crouch=d->g==2?6:0;break;case DS_STUN:lean=-4;armA=60;break;case DS_HIT:lean=-5;armA=50;break;case DS_DODGE:lean=-6;crouch=4;armA=30;break;
  case DS_KO:{u16 c0=F->c0;cf(x-18,gy-8,36,8,c0);cf(x+f*14-4,gy-10,8,8,F->skin);return;}
  case DS_WALK:lean=((frame>>3)&1)?1:-1;armA=20;break;default:armA=25;}
 int hip=gy-22+crouch,sh=gy-44+crouch;int lx=x+lean*f;u16 c0=d->flash&2?RGB(255,255,255):F->c0,c1=F->c1;
 // legs
 int st=d->s==DS_WALK?((frame>>2)&3)*2-3:0;dl_line(x-4*f,hip,x-8*f+st,gy,5,c1);dl_line(x+4*f,hip,x+8*f-st,gy,5,c1);cf(x-11,gy-2,7,2,RGB(40,30,24));cf(x+5,gy-2,7,2,RGB(40,30,24));
 // torso
 cf(lx-7,sh,14,hip-sh,c0);cf(lx-7,hip-4,14,3,c1);if(F->armor>=10)cf(lx-7,sh+4,14,2,RGB(200,200,206));
 // head + hat
 int hx=lx+f*1,hy=sh-10;cf(hx-5,hy,10,10,F->skin);cf(hx+f*2,hy+3,2,2,RGB(30,30,30));
 switch(F->hat){case 0:cf(hx-6,hy-2,12,9,RGB(170,176,186));cf(hx-1+f*2,hy+3,5,2,RGB(30,30,34));break;
  case 1:cf(hx-5,hy-3,10,4,RGB(30,30,34));cf(hx-1,hy-6,3,3,RGB(30,30,34));break;
  case 2:cf(hx-6,hy-3,12,5,RGB(150,150,156));cf(hx-6,hy+6,12,6,RGB(190,120,60));break;
  case 3:cf(hx-7,hy-4,14,4,RGB(30,30,40));cf(hx-5,hy-7,10,3,RGB(30,30,40));break;
  case 4:cf(hx-6,hy-3,12,4,RGB(150,30,40));cf(hx+f*6,hy-1,3,6,RGB(150,30,40));cf(hx-f*3,hy+3,3,2,RGB(20,20,20));break;
  case 5:cf(hx-6,hy-3,12,6,RGB(190,170,80));cf(hx-1,hy-8,3,5,RGB(170,30,30));break;
  default:cf(hx-6,hy-3,12,11,RGB(190,150,60));cf(hx-6,hy-8,12,5,RGB(170,30,30));cf(hx-1+f*3,hy+2,3,6,F->skin);break;}
 // shield arm (roman/spartan/knight hold shields or 2nd hand)
 if(F->wk==4||F->wk==5){int sx=lx+f*8;cf(sx-4,sh+2,8,24,F->wk==4?RGB(170,40,40):RGB(200,150,60));cf(sx-1,sh+10,2,8,RGB(230,210,120));}
 // weapon arm: shoulder -> hand, blade beyond
 int ang=armA;int ca=COS(deg2a(ang)),sa=SIN(deg2a(ang));int hx2=lx+f*(ca*14>>8),hy2=sh+4+(sa*14>>8);
 dl_line(lx+f*3,sh+4,hx2,hy2,4,c0);cf(hx2-2,hy2-2,4,4,F->skin);
 int L=F->reach-14;if(d->s==DS_STRIKE)L+=2;int bx=hx2+f*(ca*L>>8),by=hy2+(sa*L>>8);
 u16 bc=d->s==DS_STRIKE?RGB(255,255,230):F->c2;
 if(F->wk==2){dl_line(hx2,hy2,bx,by,2,RGB(120,80,40));cf(bx-3,by-4,6,8,bc);}
 else if(F->wk==5){dl_line(hx2-f*(ca*10>>8),hy2-(sa*10>>8),bx,by,2,RGB(140,100,60));cf(bx-2,by-2,4,4,bc);}
 else {dl_line(hx2,hy2,bx,by,F->wk==4?3:2,bc);cf(hx2-2*(sa!=0),hy2-2,4,1+2*(sa==0),RGB(200,170,60));}
 if(d->s==DS_STRIKE){for(int k=0;k<3;k++)cf(bx+f*(2+k),by+(k-1)*2,1,1,RGB(255,255,255));}}
static int cvx0(int i){return (DL[i].x>>8)-CW/2;}
static void dl_msgclr(void){dma32(dl_bg+40*240,(void*)(0x06000000+40*480),240*12/2);}
static int dl_lo(int d){return d<0?0:d;}
// combat
static void dl_strike(int a){Duelist*A=&DL[a],*D=&DL[1-a];const Fighter*FA=&FTR[A->f],*FD=&FTR[D->f];
 int dist=iabs((A->x-D->x)>>8);if(dist>FA->reach+6||D->inv>0||D->s==DS_KO)return;
 int dmg=(A->h==0?14:A->h==1?10:8)*FA->pow/10;
 if(D->s==DS_BLOCK&&D->g==A->h){if(D->pw<=(8+FD->blk)){A->s=DS_STUN;A->t=42;D->st=clampi(D->st+15,0,100);D->s=DS_IDLE;sfx_ok();lmsg("PARRY!",40);D->flash=6;return;}
  D->st-=14-FD->blk*2;D->x+=(D->face<0?1:-1)*(D->face<0?-1:1)*0;D->x-=D->face*(5<<8);sfx_shot(0);
  if(D->st<=0){D->st=0;D->s=DS_STUN;D->t=30;D->hp-=dmg/2;lmsg("GUARD BREAK!",40);}return;}
 if(D->s==DS_BLOCK&&D->g!=A->h)lmsg(A->h==0?"HIGH CUT!":A->h==2?"LOW SWEEP!":"THRUST!",30);
 dmg=dmg*(100-FD->armor)/100;D->hp-=dmg;D->s=DS_HIT;D->t=16;D->x-=D->face*(8<<8);D->flash=8;sfx_hurt();if(D->hp<=0){D->hp=0;D->s=DS_KO;D->t=0;}}
static void dl_act(int i,int left,int right,int up,int down,int atk,int blk,int bpress,int dodge){Duelist*d=&DL[i];const Fighter*F=&FTR[d->f];
 if(d->inv>0)d->inv--;if(d->flash>0)d->flash--;if(d->pw<99)d->pw++;
 if(d->s==DS_KO||d->s==DS_WIN)return;
 if(d->t>0){d->t--;
  if(d->s==DS_WIND&&d->t==0){d->s=DS_STRIKE;d->t=5;dl_strike(i);return;}
  if(d->s==DS_STRIKE&&d->t==0){d->s=DS_REC;d->t=18-F->spd*2;return;}
  if(d->s==DS_DODGE)d->x-=d->face*(3<<8);
  if(d->t>0)return;d->s=DS_IDLE;}
 if(d->s==DS_STRIKE||d->s==DS_REC||d->s==DS_STUN||d->s==DS_HIT||d->s==DS_DODGE)d->s=DS_IDLE;
 if(bpress)d->pw=0;
 if(dodge&&d->st>=12){d->st-=F->spd>=6?12:20;d->s=DS_DODGE;d->t=12;d->inv=10;sfx_whoosh();return;}
 if(atk&&d->st>=10){d->st-=up?14:down?10:12;d->s=DS_WIND;d->h=up?0:down?2:1;d->t=16-F->spd*2+(up?3:0);sfx_whoosh();return;}
 if(blk){d->s=DS_BLOCK;d->g=up?0:down?2:1;return;}
 d->st+=d->st<100?(F->stam>=100?1:(frame&1)):0;if(d->st>F->stam)d->st=F->stam;
 int mv=(right?1:0)-(left?1:0);if(mv){d->x+=mv*(0x120+F->spd*0x30);d->s=DS_WALK;}else d->s=DS_IDLE;}
static void dl_cpu(int lvl,int*l,int*r,int*u,int*dn,int*a,int*b,int*bp,int*dg){Duelist*me=&DL[1],*pl=&DL[0];const Fighter*F=&FTR[me->f];
 static const u8 REACT[4]={22,14,9,5},BLOCKP[4]={30,55,75,90},PARRYP[4]={5,20,40,65};
 *l=*r=*u=*dn=*a=*b=*bp=*dg=0;int dist=iabs((me->x-pl->x)>>8);
 if(pl->s==DS_WIND){if(me->ai_t==0)me->ai_t=REACT[lvl];
  if(--me->ai_t<=1){if(me->ai_plan==0)me->ai_plan=rr(100)<BLOCKP[lvl]?1+(rr(100)<PARRYP[lvl]):(rr(100)<20+lvl*8?3:4);
   if(me->ai_plan==1||me->ai_plan==2){*b=1;*u=pl->h==0;*dn=pl->h==2;if(me->ai_plan==2&&pl->t<=3)*bp=1;else if(me->ai_plan==1&&me->s!=DS_BLOCK)*bp=0;}
   if(me->ai_plan==3&&me->st>25)*dg=1;}
  return;}
 me->ai_t=0;me->ai_plan=0;
 if(me->st<20){if(dist<F->reach+20){*r=me->face<0;*l=me->face>0;}return;}
 if(dist>F->reach){if(me->face>0)*r=1;else *l=1;return;}
 if(rr(100)<10+lvl*8){*a=1;int k=rr(3);*u=k==0;*dn=k==2;if(pl->s==DS_BLOCK){*u=pl->g!=0&&rr(2);*dn=!*u&&pl->g!=2;}}
 else if(rr(100)<8){if(me->face>0)*l=1;else *r=1;}}
static void dl_hud(int round,int tleft,int force){static int last[6];int cur[6]={DL[0].hp,DL[0].st,DL[1].hp,DL[1].st,tleft,DL[0].wins|(DL[1].wins<<4)|(round<<8)};
 if(!force){int same=1;for(int i=0;i<6;i++)if(cur[i]!=last[i])same=0;if(same)return;}for(int i=0;i<6;i++)last[i]=cur[i];
 lfb=(u16*)0x06000000;
 if(force){lf(0,0,240,24,RGB(20,20,30));ltx(8,15,FSN[DL[0].f],C_BLUE);ltx(233-slen(FSN[DL[1].f])*6,15,FSN[DL[1].f],C_RED);}
 for(int i=0;i<2;i++){const Duelist*d=&DL[i];int x=i?132:8;
  lf(x,4,100,6,RGB(60,20,20));lf(x+(i?100-d->hp:0),4,d->hp,6,d->hp>30?C_GREEN:C_RED);lf(x,12,100,3,RGB(30,30,40));lf(x+(i?100-d->st:0),12,d->st,3,C_YEL);
  for(int k=0;k<2;k++)lf(i?126-k*6:110+k*6,4,4,4,k<d->wins?C_YEL:RGB(50,50,60));}
 lf(112,14,18,9,RGB(20,20,30));ltx(tleft>=10?114:117,15,itoa(tleft),C_GRAY);
 lfb=0;}
static int dl_round(int round,int lvl){Duelist*A=&DL[0],*B=&DL[1];
 A->x=70<<8;B->x=170<<8;A->hp=B->hp=100;A->st=FTR[A->f].stam>100?100:FTR[A->f].stam;B->st=FTR[B->f].stam>100?100:FTR[B->f].stam;
 A->s=B->s=DS_IDLE;A->t=B->t=0;A->inv=B->inv=0;A->pw=B->pw=99;int gy=136,T=60*60,res=-1,endT=0;
 const char*shown=0;dpreset();cv_hide();
 lmsg(round==3?"FINAL ROUND - FIGHT!":cat3("ROUND ",itoa(round)," - FIGHT!"),90);vsync();REG_DISPCNT=3|(1<<10);dma32(dl_bg,(void*)0x06000000,240*160/2);dl_hud(round,60,1);REG_DISPCNT=3|(1<<6)|(1<<10)|(1<<12);
 for(int fr=0;;fr++){BEAT->screen=SCR_DUEL;BEAT->phase=round;BEAT->hp=A->hp;BEAT->scoreA=A->wins;BEAT->scoreB=B->wins;BEAT->x=A->x>>8;BEAT->y=B->x>>8;BEAT->kills=B->hp;BEAT->extra=A->s|(B->s<<4)|(A->f<<8)|(B->f<<12);BEAT->timer=T;
  if(res<0&&fr>40){
   A->face=A->x<B->x?1:-1;B->face=-A->face;
   int bp=(pressed&K_B)!=0;dl_act(0,(keys&K_LEFT)!=0,(keys&K_RIGHT)!=0,(keys&K_UP)!=0,(keys&K_DOWN)!=0,(pressed&K_A)!=0,(keys&K_B)!=0,bp,(pressed&(K_L|K_R))!=0);
   int l,r,u,d,a,b,p,g;dl_cpu(lvl,&l,&r,&u,&d,&a,&b,&p,&g);dl_act(1,l,r,u,d,a,b,p,g);
   for(int i=0;i<2;i++)DL[i].x=clampi(DL[i].x,16<<8,224<<8);
   if(iabs((A->x-B->x)>>8)<20){if(A->x<B->x){A->x-=1<<8;B->x+=1<<8;}else{A->x+=1<<8;B->x-=1<<8;}}
   if(--T<=0||A->s==DS_KO||B->s==DS_KO){res=A->hp>B->hp?0:B->hp>A->hp?1:2;endT=fr;if(res<2){DL[res].wins++;DL[res].s=DS_WIN;}
    lmsg(res==2?"DRAW!":A->s==DS_KO||B->s==DS_KO?(res==0?"K.O. - YOU WIN THE ROUND":"K.O. - CPU TAKES THE ROUND"):(res==0?"TIME - YOU WIN THE ROUND":"TIME - CPU TAKES THE ROUND"),120);sfx_boom();}}
  if(res>=0&&fr-endT>130){cv_hide();return res;}
  // render both fighters into canvases, then upload in vblank
  for(int i=0;i<2;i++){cv_begin(i,DL[i].x>>8,gy);dl_draw(&DL[i],gy);}
  next_frame();for(int i=0;i<2;i++){cv_upload(i);cv_oam(i,cvx0(i),gy-CH+2,1);}
  dl_hud(round,(T+59)/60,0);
  if(lMsgT>0){if(lMsg!=shown){dl_msgclr();lfb=(u16*)0x06000000;ltc(42,lMsg,C_YEL);lfb=0;shown=lMsg;}if(--lMsgT==0){dl_msgclr();shown=0;}}}}
static void dl_match(int pf,int cf,int ar){int lvl=SET.bot;DL[0].f=pf;DL[1].f=cf;DL[0].wins=DL[1].wins=0;dl_arena(ar);bgm_t(TR_DUEL);
 int round=1;while(DL[0].wins<2&&DL[1].wins<2&&round<=5){dl_round(round,lvl);round++;}
 int win=DL[0].wins>DL[1].wins;if(win)music_victory();
 for(int fr=0;;fr++){BEAT->screen=SCR_DUEL;BEAT->phase=9;BEAT->scoreA=DL[0].wins;BEAT->scoreB=DL[1].wins;
  if(fr==0){m3_title(1);m3_fill(20,40,200,80,C_PANEL);m3_box(20,40,200,80,2);m3_ctext(52,win?"VICTORY!":"DEFEAT",win?C_YEL:C_RED,2);
   m3_ctext(76,cat3(itoa(DL[0].wins)," - ",itoa(DL[1].wins)),C_WHITE,1);m3_ctext(88,cat3(FTR[pf].n," VS ",FTR[cf].n),C_GRAY,1);m3_ctext(104,"A: CONTINUE",C_GRAY,1);m3_present();}
  next_frame();if(fr>20&&(pressed&(K_A|K_B|K_START))){sfx_ok();return;}}}
static void sword_fight(void){static int pf=0,cf=1,ar=0;int row=0,redraw=1;bgm_t(TR_DUEL);
 for(;;){BEAT->screen=SCR_DSEL;BEAT->menu=row;BEAT->extra=pf|(cf<<4)|(ar<<8);
  if(redraw){redraw=0;dl_arena(ar);dma32(dl_bg,fb,240*160/2);lf(0,0,240,18,RGB(20,20,30));m3_ctext(5,"1V1 SWORD FIGHT - BEST OF 3",C_YEL,1);
   Duelist a={70<<8,1,100,100,DS_IDLE,0,0,1,0,0,pf,0,0,0,0,0},b={170<<8,-1,100,100,DS_BLOCK,0,0,1,0,0,cf,0,0,0,0,0};dpreset();cv_begin(0,70,124);dl_draw(&a,124);cv_blit(0);cv_begin(1,170,124);dl_draw(&b,124);cv_blit(1);
   lf(0,120,240,40,RGB(20,20,30));static const char*const RL[3]={"YOU","CPU","ARENA"};
   for(int k=0;k<3;k++){int y=122+k*9;ltx(6,y,k==row?">":" ",C_YEL);ltx(14,y,RL[k],k==row?C_YEL:C_GRAY);ltx(50,y,k==0?FTR[pf].n:k==1?FTR[cf].n:ARN[ar],k==row?C_WHITE:C_GRAY);}
   {const Fighter*F=&FTR[row==1?cf:pf];lf(0,19,240,22,RGB(20,20,30));if(row<2){ltc(21,F->w,C_BLUE);ltc(31,F->d,C_WHITE);}else{ltc(21,"L/R: CHANGE   A: FIGHT   B: BACK",C_WHITE);ltc(31,cat("CPU SKILL: ",(const char*[]){"RECRUIT","REGULAR","VETERAN","ELITE"}[SET.bot&3]),C_GRAY);}}
   ltc(151,"U+A HI  A MID  D+A LO  B GUARD  L/R DODGE",C_GRAY);
   m3_present();}
  next_frame();
  if(pressed&K_UP){row=(row+2)%3;redraw=1;sfx_menu();}if(pressed&K_DOWN){row=(row+1)%3;redraw=1;sfx_menu();}
  int d=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;
  if(d){if(row==0)pf=(pf+NFTR+d)%NFTR;else if(row==1)cf=(cf+NFTR+d)%NFTR;else ar=(ar+4+d)%4;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();dl_match(pf,cf,ar);bgm_t(TR_DUEL);redraw=1;}
  if(pressed&K_B){sfx_menu();return;}}}
