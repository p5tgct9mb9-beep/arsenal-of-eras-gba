// ===================================================================== v1.3.2 HANGAR - AIRCRAFT BUILDER
// Designs: class + 6 parts + paint + name. Stats are summed from part tables; the 32x32 top-down sprite is
// composited procedurally from the parts and injected into the real 6v6 dogfight engine (hgAir / hgSpr hooks in mp.inc).
static inline int hmn(int a,int b){return a<b?a:b;}static inline int hmx(int a,int b){return a>b?a:b;}
enum{HC_PLANE,HC_DRONE,HC_HELI};
typedef struct{u8 cls,body,wing,eng,wpn,arm,paint,used;char name[12];}HDes;
typedef struct{HDes d[3];u16 flights,photos;u32 kills;}HSave;
static HSave HS EWRAM;static int hs_loaded;
#define HS_OFF 0x2300
typedef struct{const char*n;s16 kmh,agi,arm;s32 kg,pay;s16 fuel;}HPart; // kmh, agility 0-100, armor 0-100, kg, payload kg, endurance minutes
static const char*const HCLS[3]={"PLANE","DRONE","HELICOPTER"};
static const HPart HBODY[3][4]={
 {{"LIGHT FIGHTER",620,58,15,3200,600,40},{"HEAVY FIGHTER",540,38,35,7500,2200,60},{"BOMBER",440,12,42,13000,6500,120},{"STEALTH WING",520,28,22,9000,3200,95}},
 {{"MINI FRAME",70,88,0,2,1,22},{"MEDIUM FRAME",105,66,5,24,6,32},{"HEAVY LIFTER",85,36,12,110,40,28},{"FIXED WING",190,30,10,700,250,900}},
 {{"SCOUT",240,72,12,1300,350,150},{"ATTACK",280,58,48,5000,1400,140},{"TRANSPORT",260,34,30,7000,3500,150},{"HEAVY LIFT",230,18,35,15000,9000,140}}};
static const HPart HWING[3][4]={
 {{"PROP STRAIGHT",0,12,0,300,200,40},{"JET SWEPT",900,6,0,800,300,-10},{"JET DELTA",1350,-4,0,1000,450,-15},{"VTOL FANS",320,16,0,1500,-200,-20}},
 {{"QUAD ROTOR",0,10,0,0,0,0},{"HEX ROTOR",-5,2,2,3,4,-3},{"OCTO ROTOR",-10,-6,6,6,10,-6},{"PUSHER PROP",60,-12,0,2,0,60}},
 {{"SINGLE ROTOR",0,8,0,0,0,0},{"COAXIAL",25,14,0,300,200,-10},{"TANDEM ROTORS",-15,-14,0,900,2500,-10},{0}}};
static const HPart HENG[3][4]={
 {{"STANDARD",0,0,0,0,0,0},{"TURBOFAN",150,0,0,300,0,25},{"AFTERBURNER",520,6,0,600,0,-28},{"ECONOMY",-90,0,0,-200,0,50}},
 {{"STD BATTERY",0,0,0,0,0,0},{"HI-CAP BATT",-5,-5,0,1,0,16},{"SPORT BATTERY",40,10,0,0,0,-9},{"HYBRID GENSET",-10,-10,0,6,0,120}},
 {{"ONE TURBINE",0,0,0,0,0,0},{"TWIN TURBINE",30,6,0,250,600,-5},{"HIGH-OUTPUT",55,10,0,350,300,-25},{0}}};
static const char*const HWPN[5]={"GUNS ONLY","AAM MISSILES","BOMBS","ROCKET PODS","RECON CAMERA"};
static const s8 HWPN_KMH[5]={0,-20,-40,-25,10},HWPN_AGI[5]={0,-3,-8,-5,2},HWPN_KG[5]={3,6,12,8,2};
static const char*const HARM[4]={"NO ARMOR","LIGHT ARMOR","MEDIUM ARMOR","HEAVY ARMOR"};
static const s8 HARM_KMH[4]={0,-10,-25,-50},HARM_AGI[4]={0,-3,-8,-15},HARM_ARM[4]={0,20,40,65},HARM_KG[4]={0,5,10,18};
static const char*const HPAINT[6]={"FACTION CAMO","GHOST GREY","ARCTIC WHITE","NIGHT BLACK","TEAM COLORS","GOLD RACER"};
static const u8 HPAINTC[6][3]={{2,3,4},{12,13,14},{13,14,11},{1,7,12},{7,9,11},{8,15,11}};
static int hg_nw(int c){return c==HC_HELI?3:4;}static int hg_ne(int c){return c==HC_HELI?3:4;}
typedef struct{int kmh,agi,arm,kg,pay,fuel;}HStat;
static HStat hg_stats(const HDes*d){const HPart*b=&HBODY[d->cls][d->body],*w=&HWING[d->cls][d->wing],*e=&HENG[d->cls][d->eng];HStat s;
 s.kmh=b->kmh+w->kmh+e->kmh;s.agi=b->agi+w->agi+e->agi;s.arm=b->arm;s.kg=b->kg+w->kg+e->kg;s.pay=b->pay+w->pay+e->pay;s.fuel=b->fuel+w->fuel+e->fuel;
 int bk=b->kg;s.kg+=bk*HWPN_KG[d->wpn]/100+bk*HARM_KG[d->arm]/100;
 int sc=d->cls==HC_DRONE?1:4;s.kmh+=HWPN_KMH[d->wpn]*sc/4+HARM_KMH[d->arm]*sc/4;if(d->cls==HC_DRONE&&s.kmh<20)s.kmh=20;
 s.agi+=HWPN_AGI[d->wpn]+HARM_AGI[d->arm];s.arm+=HARM_ARM[d->arm];
 s.kmh=clampi(s.kmh,20,2600);s.agi=clampi(s.agi,5,100);s.arm=clampi(s.arm,0,100);if(s.pay<0)s.pay=0;s.fuel=clampi(s.fuel,5,1440);return s;}
static const char*hg_wname(int c){return c==HC_PLANE?"WINGS/ENGINE":"ROTORS";}
// ---- AirS for the dogfight engine
static void hg_make_air(const HDes*d){HStat s=hg_stats(d);AirS a;memset(&a,0,sizeof a);a.n=d->name;a.mn=HWPN[d->wpn];
 int k=s.kmh;a.spd=k<700?180+k*33/100:411+(k-700)/10;if(a.spd>650)a.spd=650;if(d->cls==HC_DRONE&&a.spd<200)a.spd=200;
 a.turn=300+s.agi*5;a.rate=d->wpn==0?3:5;a.dmg=d->wpn==0?13:10;if(d->cls==HC_DRONE)a.dmg-=3;a.bspd=1250;
 int pm=d->cls==HC_PLANE?6500:d->cls==HC_HELI?9000:40;int am=2+s.pay*12/pm;am=clampi(am,1,16);
 a.sec=d->wpn==2?2:d->wpn==1?1:0;a.mturn=d->wpn==1?350:0;a.ammo=d->wpn==3?am*2:d->wpn==4||d->wpn==0?0:am;
 a.hp=50+s.arm*3/2+(d->cls==HC_PLANE&&d->body==2?40:0);
 int fs=60+s.fuel*3/2;a.fuel=fs>250?0:fs; // long endurance (>~2.5h) = unlimited for a match
 a.cls=d->cls==HC_HELI?12:d->cls==HC_DRONE?(d->body==3?11:13):(d->body==2?10:8);a.gs=255;a.side=0;
 hgAir=a;hgCam=d->wpn==4;}
// ---- procedural 32x32 top-down sprite (pointing up), palette indices of the obj team banks
static u8 hgG[32*32] EWRAM;
static void gp(int x,int y,int c){if((unsigned)x<32&&(unsigned)y<32)hgG[y*32+x]=c;}
static void grect(int x,int y,int w,int h,int c){for(int j=0;j<h;j++)for(int i=0;i<w;i++)gp(x+i,y+j,c);}
static void gline(int x0,int y0,int x1,int y1,int c){int dx=iabs(x1-x0),dy=-iabs(y1-y0),sx=x0<x1?1:-1,sy=y0<y1?1:-1,e=dx+dy;for(;;){gp(x0,y0,c);if(x0==x1&&y0==y1)break;int e2=2*e;if(e2>=dy){e+=dy;x0+=sx;}if(e2<=dx){e+=dx;y0+=sy;}}}
static void gdisc(int cx,int cy,int r,int c){for(int y=-r;y<=r;y++)for(int x=-r;x<=r;x++)if(x*x+y*y<=r*r+r/2)gp(cx+x,cy+y,c);}
static void gring(int cx,int cy,int r,int c){for(int y=-r;y<=r;y++)for(int x=-r;x<=r;x++){int q=x*x+y*y;if(q<=r*r+r&&q>=(r-1)*(r-1))gp(cx+x,cy+y,c);}}
static void gtri(int ax,int ay,int bx,int by,int cx,int cy,int c){int x0=hmn(ax,hmn(bx,cx)),x1=hmx(ax,hmx(bx,cx)),y0=hmn(ay,hmn(by,cy)),y1=hmx(ay,hmx(by,cy));
 for(int y=y0;y<=y1;y++)for(int x=x0;x<=x1;x++){int d1=(bx-ax)*(y-ay)-(by-ay)*(x-ax),d2=(cx-bx)*(y-by)-(cy-by)*(x-bx),d3=(ax-cx)*(y-cy)-(ay-cy)*(x-cx);
  if((d1>=0&&d2>=0&&d3>=0)||(d1<=0&&d2<=0&&d3<=0))gp(x,y,c);}}
static void gmir(void){for(int y=0;y<32;y++)for(int x=0;x<16;x++){u8 a=hgG[y*32+x],b=hgG[y*32+31-x];if(a&&!b)hgG[y*32+31-x]=a;}} // symmetric: copy left half to right
static void goutline(void){static u8 t[32*32] EWRAM;memcpy(t,hgG,sizeof t);for(int y=0;y<32;y++)for(int x=0;x<32;x++)if(!t[y*32+x]){int n=0;
  if(x>0&&t[y*32+x-1]>1)n++;if(x<31&&t[y*32+x+1]>1)n++;if(y>0&&t[(y-1)*32+x]>1)n++;if(y<31&&t[(y+1)*32+x]>1)n++;if(n)hgG[y*32+x]=1;}}
static void hg_draw(const HDes*d){memset(hgG,0,sizeof hgG);const u8*P=HPAINTC[d->paint];int D=P[0],M=P[1],L=P[2];
 if(d->cls==HC_PLANE){int fw=d->body==0?2:d->body==1?3:4;
  if(d->body==3){gtri(15,3,1,22,15,22,M);gtri(15,18,6,26,15,22,D);gline(15,4,15,21,L);}
  else{ // wings
   if(d->wing==0){grect(2,11,14,5,M);grect(2,11,14,1,L);grect(15-fw,1,1,1,13);gline(10,1,20,1,12);}
   else if(d->wing==1){gtri(15,10,2,20,15,16,M);gline(15,11,4,19,L);}
   else if(d->wing==2){gtri(15,6,2,23,15,23,M);gline(15,7,3,22,L);}
   else{grect(5,13,11,4,M);gring(8,9,3,12);gdisc(8,9,2,13);}
   grect(16-fw,2,fw,26,M);grect(16-fw,2,1,26,L);grect(16-fw,4,fw,4,11);grect(15,5,1,2,14); // fuselage + canopy
   gtri(15,24,9,29,15,28,D); // tailplane
   if(d->body==2){grect(6,13,2,5,D);grect(10,12,2,5,D);} // bomber engines
  }
  if(d->eng==2){grect(14,28,2,3,10);gp(14,31,15);} else if(d->wing>0){gp(15,29,12);}
 }else if(d->cls==HC_HELI){int bw=d->body==0?3:d->body==1?2:d->body==2?4:5;
  grect(16-bw,8,bw,13,M);grect(16-bw,8,1,13,L);grect(14,21,2,9,D);grect(11,28,5,2,D); // body, boom, stabiliser
  grect(16-bw+1,9,bw-1,3,14);if(d->body==1){grect(14,5,2,3,M);}
  if(d->wing==2){grect(16-bw,5,bw,22,M);gline(15,8,4,1,13);gline(15,8,8,15,13);gline(15,23,4,16,13);gline(15,23,8,30,13);gdisc(15,8,1,12);gdisc(15,23,1,12);}
  else{gline(15,13,2,0,13);gline(15,13,2,26,13);gdisc(15,13,1,12);if(d->wing==1){gline(15,13,4,2,14);gline(15,13,6,26,14);}else{gp(12,29,13);gp(12,27,13);}}
 }else{ // drone
  if(d->body==3){grect(1,13,15,3,M);grect(1,13,15,1,L);grect(14,6,2,20,M);grect(9,25,7,2,D);grect(14,5,2,2,14);gline(13,29,15,29,13);}
  else{int r=d->body==0?2:d->body==1?3:4;int arm=d->body==0?8:d->body==1?10:12;int na=d->wing==0?4:d->wing==1?6:8;
   for(int k=0;k<na;k++){int an=(k*256/na+(na==4?32:0))&255;int ex=15+(arm*SIN(an)>>8),ey=15-(arm*COS(an)>>8);gline(15,15,ex,ey,D);gring(ex,ey,r,13);gp(ex,ey,12);}
   grect(16-r-1,16-r-1,2*r+1,2*r+1,M);grect(16-r-1,16-r-1,2*r+1,1,L);gp(15,12-r,14);}
  if(d->wing==3&&d->body!=3){gring(15,27,3,13);}
 }
 // hardpoints
 int hy=d->cls==HC_PLANE?16:d->cls==HC_HELI?15:18,hx=d->cls==HC_DRONE?13:8;
 if(d->wpn==1){grect(hx,hy-3,1,5,11);gp(hx,hy-4,10);}else if(d->wpn==2){grect(hx,hy,2,3,12);}else if(d->wpn==3){grect(hx,hy-1,2,4,12);gp(hx,hy-2,10);}else if(d->wpn==4){gp(15,hy+2,14);gp(15,hy+3,12);}
 if(d->arm>=2){gp(15,hy+6,12);gp(14,hy+6,12);}
 gmir();if(d->wpn==4){gp(16,hy+2,14);gp(16,hy+3,12);}goutline();
 for(int t=0;t<16;t++){int tx=(t&3)*8,ty=(t>>2)*8;for(int y=0;y<8;y++){u32 w=0;for(int x=0;x<8;x++)w|=(u32)(hgG[(ty+y)*32+tx+x]&15)<<(x*4);hgSpr[t*8+y]=w;}}}
static void hg_pal(u16*P){for(int k=0;k<16;k++)P[k]=obj_team_pal[(2*3+2)*16+k];const Fac*F=&FAC[g6.fac0];P[2]=fac_c(F->m,3,4);P[3]=fac_c(F->m,1,1);P[4]=fac_c(F->m,5,4);P[9]=RGB(255,220,40);}
static void hg_preview(int x0,int y0,int sc,const HDes*d){hg_draw(d);u16 P[16];hg_pal(P);for(int y=0;y<32;y++)for(int x=0;x<32;x++){int v=hgG[y*32+x];if(v)m3_fill(x0+x*sc,y0+y*sc,sc,sc,P[v]);}}
// ---- save
static void hs_save(void){sv_put(HS_OFF,'H',1,&HS,sizeof HS);}
static void hs_load(void){if(hs_loaded)return;hs_loaded=1;if(!sv_get(HS_OFF,'H',1,&HS,sizeof HS))memset(&HS,0,sizeof HS);}
static void hg_default(HDes*d,int cls,int slot){memset(d,0,sizeof *d);d->cls=cls;d->used=1;const char*n=cls==HC_PLANE?"FALCON ":cls==HC_DRONE?"HORNET ":"KESTREL ";int k=0;while(n[k]&&k<10){d->name[k]=n[k];k++;}d->name[k++]='1'+slot;d->name[k]=0;
 if(cls==HC_PLANE){d->wing=1;d->wpn=1;}else if(cls==HC_HELI){d->body=1;d->wpn=3;d->arm=1;}else{d->body=1;d->wpn=4;}}
// ---- name editor
static void hg_name(char*nm){static const char HCH[]=" ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";int nc=sizeof HCH-1;int cur=0;int redraw=1;
 for(;;){BEAT->screen=SCR_HNAME;BEAT->menu=cur;
  if(redraw){redraw=0;m3_fill(30,50,180,60,RGB(10,12,24));m3_box(30,50,180,60,2);m3_ctext(56,"NAME YOUR AIRCRAFT",C_YEL,1);
   for(int i=0;i<10;i++){char s[2]={nm[i]?nm[i]:' ',0};m3_fill(50+i*14,72,12,14,i==cur?RGB(40,80,140):RGB(24,28,44));m3_text(53+i*14,75,s,C_WHITE,1);}
   m3_ctext(94,"UP/DOWN LETTER  L/R MOVE  A DONE",C_GRAY,1);m3_present();}
  next_frame();int len=0;while(nm[len])len++;
  if(pressed&(K_UP|K_DOWN)){if(cur>=len){for(int i=len;i<=cur;i++)nm[i]=' ';nm[cur+1]=0;}int k=0;for(int i=0;i<nc;i++)if(HCH[i]==nm[cur])k=i;k=(k+(pressed&K_UP?1:nc-1))%nc;nm[cur]=HCH[k];redraw=1;sfx_menu();}
  if(pressed&K_RIGHT){if(cur<9)cur++;redraw=1;}if(pressed&K_LEFT){if(cur>0)cur--;redraw=1;}
  if(pressed&(K_A|K_START|K_B)){int e=10;while(e>0&&(nm[e-1]==' '||nm[e-1]==0))e--;nm[e]=0;if(!e){nm[0]='X';nm[1]=0;}sfx_ok();pressed=0;return;}}}
// ---- builder screen
static void hg_bg(void){grad(0,160,RGB(30,40,60),RGB(64,72,90));for(int x=0;x<240;x+=30)m3_fill(x,0,2,160,RGB(44,54,70));for(int x=0;x<240;x+=16)m3_fill(x,152,8,2,RGB(240,200,40));}
static const char*hg_val(const HDes*d,int r){switch(r){case 0:return HCLS[d->cls];case 1:return HBODY[d->cls][d->body].n;case 2:return HWING[d->cls][d->wing].n;case 3:return HENG[d->cls][d->eng].n;
 case 4:return HWPN[d->wpn];case 5:return HARM[d->arm];case 6:return HPAINT[d->paint];case 7:return d->name;}return "";}
static int hg_build(HDes*d){static const char*const RL[10]={"CLASS","BODY","","","WEAPONS","ARMOR","PAINT","NAME","SAVE DESIGN","CANCEL"};int r=0,redraw=1;
 for(;;){HStat s=hg_stats(d);BEAT->screen=SCR_HBUILD;BEAT->menu=r;BEAT->mode=d->cls;BEAT->x=s.kmh;BEAT->y=s.agi;BEAT->hp=s.arm;BEAT->extra=s.kg>32767?32767:s.kg;BEAT->scoreA=s.pay>32767?32767:s.pay;BEAT->scoreB=s.fuel;
  if(redraw){redraw=0;hg_bg();m3_fill(0,0,240,18,RGB(10,12,24));m3_ctext(4,"HANGAR - AIRCRAFT BUILDER",C_YEL,1);
   m3_fill(4,22,100,100,RGB(14,18,30));m3_box(4,22,100,100,0);for(int y=26;y<118;y+=8)m3_fill(8,y,92,1,RGB(22,30,46));hg_preview(6,24,3,d);
   m3_fill(108,22,128,112,RGB(10,12,24));m3_box(108,22,128,112,0);
   for(int i=0;i<10;i++){int y=26+i*10+(i>=8?4:0);const char*lab=i==2?(d->cls==HC_PLANE?"WINGS":"ROTORS"):i==3?(d->cls==HC_PLANE?"ENGINE":d->cls==HC_DRONE?"POWER":"ENGINE"):RL[i];u16 c=i==r?C_YEL:C_WHITE;
    if(i<8){m3_text(112,y,lab,i==r?C_YEL:C_GRAY,1);const char*v=hg_val(d,i);char t[20];int k=0;while(v[k]&&k<13){t[k]=v[k];k++;}t[k]=0;m3_text(157,y,t,c,1);}
    else m3_text(118,y,i==r?cat(">",RL[i]):RL[i],i==8?(i==r?C_GREEN:RGB(120,200,120)):(i==r?C_RED:C_GRAY),1);}
   m3_fill(4,124,100,32,RGB(10,12,24));m3_box(4,124,100,32,0);
   m3_text(8,127,cat3(itoa(s.kmh)," KM/H",""),C_WHITE,1);m3_text(8,136,s.kg>=10000?cat3(itoa(s.kg/1000)," T",""):cat3(itoa(s.kg)," KG",""),C_GRAY,1);
   m3_text(8,145,cat3(d->cls==HC_DRONE&&d->body!=3?"BATT ":"FUEL ",s.fuel>=120?itoa(s.fuel/60):itoa(s.fuel),s.fuel>=120?" H":" MIN"),C_GRAY,1);
   m3_fill(108,136,128,20,RGB(10,12,24));m3_box(108,136,128,20,0);
   {int bx=112;const char*L3[3]={"AGI","ARM","PAY"};int pm=d->cls==HC_PLANE?6500:d->cls==HC_HELI?9000:40;int v3[3]={s.agi,s.arm,hmn(100,s.pay*100/pm)};
    for(int k=0;k<3;k++){m3_text(bx+k*42,139,L3[k],C_GRAY,1);m3_fill(bx+k*42,148,38,4,RGB(30,30,40));m3_fill(bx+k*42,148,hmx(1,v3[k]*38/100),4,k==0?C_GREEN:k==1?C_BLUE:C_YEL);}}
   m3_present();}
  next_frame();
  if(pressed&K_UP){r=(r+9)%10;redraw=1;sfx_menu();}if(pressed&K_DOWN){r=(r+1)%10;redraw=1;sfx_menu();}
  int dx=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;
  if(dx&&r<7){u8*f[7]={&d->cls,&d->body,&d->wing,&d->eng,&d->wpn,&d->arm,&d->paint};int n[7]={3,4,hg_nw(d->cls),hg_ne(d->cls),5,4,6};*f[r]=(*f[r]+n[r]+dx)%n[r];
   if(r==0){d->body=0;d->wing=0;d->eng=0;}redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){if(r==7){hg_name(d->name);redraw=1;}else if(r==8){sfx_ok();pressed=0;return 1;}else if(r==9){pressed=0;return 0;}else{r=8;redraw=1;sfx_menu();}}
  if(pressed&K_B){pressed=0;return 0;}}}
// ---- test flight: real dogfight engine; RANGE = enemy aircraft are unarmed targets
static void hg_photo(int i){Unit*u=&U[i];int a=u->ang>>8;for(int k=0;k<NU;k++){Unit*e=&U[k];if(!e->alive||e->team==u->team)continue;int dx=dxw(e->x,u->x),dy=dyw(e->y,u->y);
  if(idist(dx,dy)<240&&iabs(angd(atan2i(dx,dy),a))<64){hgPhotos++;feed(cat("RECON PHOTO LOGGED: ",itoa(hgPhotos)),HB_GREEN);sfx_ok();return;}}feed("NO TARGET IN FRAME",HB_GRAY);}
static void hg_fly(HDes*d,int range){hg_make_air(d);hg_draw(d);hgOn=1;hgRange=range;hgPhotos=0;int ok0=U[0].kills;(void)ok0;
 loading(cat(range?"TARGET RANGE: ":"6V6 DOGFIGHT: ",d->name));mpMixed=1;mp_run(MM_AIR,2,0);mpMixed=0;REG_BLDCNT=0;REG_BLDY=0;
 hs_load();HS.flights++;HS.photos+=hgPhotos;HS.kills+=U[0].kills;hs_save();BEAT->kills=U[0].kills;BEAT->deaths=hgPhotos;
 hgOn=0;hgRange=0;hgCam=0;bgCached=-1;}
static void hangar_mode(void){hs_load();int sel=0;bgm_t(TR_HANGAR);
 for(;;){static char it[5][40] EWRAM;const char*items[5];
  for(int i=0;i<3;i++){HDes*d=&HS.d[i];char*o=sp_put(it[i],"SLOT ");o=sp_int(o,i+1);o=sp_put(o,": ");if(d->used){o=sp_put(o,d->name);o=sp_put(o," (");o=sp_put(o,HCLS[d->cls]);sp_put(o,")");}else sp_put(o,"- EMPTY -");items[i]=it[i];}
  items[3]="FLIGHT LOG";
  int s=fr_menu("HANGAR","BUILD + FLY PLANES, DRONES, HELIS",items,4,0,&sel,SCR_HANGAR);if(s<0)return;
  if(s==3){loading(cat3(cat("FLIGHTS ",itoa(HS.flights)),cat("  KILLS ",itoa(HS.kills)),cat("  PHOTOS ",itoa(HS.photos))));for(int t=0;t<120;t++){next_frame();if(pressed&(K_A|K_B))break;}continue;}
  HDes*d=&HS.d[s];
  if(!d->used){static const char*const CL[3]={"PLANE","DRONE","HELICOPTER"};int c=0;int k=fr_menu("NEW DESIGN","PICK A CLASS",CL,3,0,&c,SCR_HANGAR+100);if(k<0)continue;
   HDes t;hg_default(&t,k,s);if(hg_build(&t)){*d=t;hs_save();sfx_ok();}continue;}
  static const char*const OP[5]={"TEST FLIGHT - TARGET RANGE","FLY IN 6V6 DOGFIGHT","EDIT DESIGN","COPY TO NEXT EMPTY SLOT","SCRAP DESIGN"};int o=0;
  int k=fr_menu(d->name,HCLS[d->cls],OP,5,0,&o,SCR_HANGAR+101);if(k<0)continue;
  if(k==0||k==1){hg_fly(d,k==0);bgm_t(TR_HANGAR);}
  else if(k==2){HDes t=*d;if(hg_build(&t)){*d=t;hs_save();}}
  else if(k==3){for(int i=0;i<3;i++)if(!HS.d[i].used){HS.d[i]=*d;hs_save();break;}}
  else if(k==4){d->used=0;hs_save();}}}
static void rec_hangar(const char**L){hs_load();static char b[5][40] EWRAM;for(int i=0;i<3;i++){HDes*d=&HS.d[i];char*o=sp_put(b[i],"SLOT ");o=sp_int(o,i+1);o=sp_put(o,": ");if(d->used){o=sp_put(o,d->name);o=sp_put(o," ");sp_put(o,HCLS[d->cls]);}else sp_put(o,"EMPTY");L[i]=b[i];}
 {char*o=sp_put(b[3],"FLIGHTS ");o=sp_int(o,HS.flights);o=sp_put(o,"  KILLS ");sp_int(o,HS.kills);L[3]=b[3];}{char*o=sp_put(b[4],"RECON PHOTOS ");sp_int(o,HS.photos);L[4]=b[4];}}
