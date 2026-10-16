// ===================================================================== v1.3.2 CREDITS - scrolls over Earth from orbit, menu music
#define SCR_CRED 97
typedef struct{const char*s;u8 kind;}CrLine; // kind: 0 heading(2x gold) 1 white 2 gray 3 cyan 4 KC portrait 5 gap 6 card break
static const CrLine CRL[]={
 {"ARSENAL OF ERAS",0},{"FIELD EDITION V1.3.4",3},{"",6},
 {"CREATED BY",2},{"LT VASQUEZ",0},{"GAME DIRECTOR",1},{"DESIGNER - IDEAS",1},{"",6},
 {"BUILT WITH",2},{"KC, AI ASSISTANT",0},{"(GROK BOT)",3},{"PROGRAMMING - ART - MUSIC",1},{"",6},
 {"TUTORIAL GUIDE",2},{"KC THE DOG",0},{"",4},{"BLACK SHEPHERD MIX, BIG EARS,",1},{"GREY MUZZLE, WHITE CHEST.",1},{"GOOD GIRL. BEST SERGEANT.",3},{"",6},
 {"SPECIAL THANKS",2},{"DAD",0},{"DEDICATED TO MY DAD,",0},{"A REAL HERO. - LT",0},{"",6},
 {"THANK YOU ELON MUSK",0},{"FOR BEING THE BIGGEST",1},{"INSPIRATION FOR ME.",1},{"- LT",3},{"",6},
 {"MORSE TIP FROM THE RADIO ROOM:",2},{"... --- ...",3},{"",6},
 {"THANK YOU",0},{"TO ALL ACTIVE-DUTY AND VETERAN",1},{"MEMBERS OF THE ARMED FORCES.",1},{"",6},
 {"THANK YOU",0},{"TO THE U.S. ARMY, U.S. NAVY,",1},{"U.S. AIR FORCE, U.S. MARINE CORPS",1},{"AND U.S. SPACE FORCE.",1},{"",6},
 {"THANK YOU",0},{"SPACEX, TESLA AND ANDURIL INDUSTRIES",1},{"FOR THE INSPIRATION.",1},{"",6},{"",7},
 {"THANKS FOR PLAYING!",0},{"",5},
 {"ARSENAL OF ERAS IS A FAN-MADE PERSONAL",2},{"PROJECT AND IS NOT AFFILIATED WITH OR",2},{"ENDORSED BY ANY COMPANY OR MILITARY",2},{"BRANCH MENTIONED.",2}};
#define CR_N (int)(sizeof CRL/sizeof CRL[0])
static int cr_h(int k){switch(k){case 7:return 0;case 0:return 20;case 4:return 54;case 5:return 10;case 6:return 34;default:return 11;}}
static void cr_dim(int x0,int y0,int w,int h){if(y0<0){h+=y0;y0=0;}if(y0+h>160)h=160-y0;for(int y=y0;y<y0+h;y++){u16*p=&fb[y*240+x0];for(int x=0;x<w;x++){u16 c=p[x];p[x]=((c>>1)&0x3DEF);}}}
static void cr_kc(int x0,int y0){const u16*p=kc_portrait;for(int y=0;y<48;y++){int yy=y0+y;if(yy<0||yy>=160)continue;for(int x=0;x<48;x++){u16 c=p[y*48+x];if(!(c&0x8000))fb[yy*240+x0+x]=c;}}}
// quiet tribute screen: Tribute in Light beams over a dark skyline, slow fade in/out
static void cr_fade(int id){for(int t=0;;t++){BEAT->screen=SCR_CRED;BEAT->menu=id+t/60;REG_BLDY=t<96?16-t/6:0;next_frame();
  if((t>120&&(pressed&(K_A|K_B|K_START)))||t>480||(FR_AUTO&&t>300))break;}
 for(int t=0;t<64;t++){REG_BLDY=t/4;next_frame();}pressed=0;}
static void cr_tribute(void){music_stop();REG_BLDCNT=(1<<2)|(3<<6);REG_BLDY=16;
 for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(4+y/10,4+y/10,12+y/5));for(int i=0;i<40;i++)m3_fill((i*89+7)%240,(i*41+3)%70,1,1,RGB(90,90,110));
 for(int b=0;b<2;b++){int cx=104+b*32;for(int y=0;y<128;y++){int w=3+y/40;for(int x=-w;x<=w;x++){int ax=iabs(x);int a=ax<=1?22-y/16:hmx(0,14-ax*14/(w+1)-y/20);u16 c=fb[y*240+cx+x];int cr=c&31,cg=(c>>5)&31,cb=(c>>10)&31;cr+=a;cg+=a;cb+=a+3;fb[y*240+cx+x]=hmn(31,cr)|(hmn(31,cg)<<5)|(hmn(31,cb)<<10);}}}
 for(int x=0;x<240;x++){int h=6+((x*7)%13)+((x/12)%3)*5;if(x>=96&&x<146)h=4;m3_fill(x,128+18-h,1,h+14,RGB(6,6,10));}
 m3_fill(0,146,240,14,RGB(6,6,10));
 m3_ctext(24,"THIS GAME HONORS THE BRAVERY OF",C_WHITE,1);m3_ctext(36,"FIREFIGHTERS, MILITARY MEN AND WOMEN,",C_WHITE,1);
 m3_ctext(48,"AND THE PEOPLE WHO LOST THEIR LIVES",C_WHITE,1);m3_ctext(60,"IN THE 9/11 ATTACKS.",C_WHITE,1);m3_present();
 cr_fade(900);
 // second memorial card: Benghazi, September 11-12, 2012 - a folded flag
 REG_BLDY=16;for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(2,2,5+y/24));
 for(int j=0;j<36;j++){int w=72-j*2;int x0=84+j;for(int i=0;i<w;i++){int x=x0+i,y=118-j;fb[y*240+x]=RGB(14,22,58);}}
 for(int j=0;j<36;j++){int w=72-j*2;for(int i=0;i<w;i+=7){if((j%6)==3)m3_fill(84+j+i+((j/6)&1)*3,118-j,1,1,RGB(31,31,31)*0+RGB(250,250,255));}}
 m3_fill(80,119,80,3,RGB(90,70,40));
 m3_ctext(18,"AND IN HONOR OF THE FALLEN HEROES",C_WHITE,1);m3_ctext(30,"OF BENGHAZI, SEPTEMBER 11-12, 2012:",C_WHITE,1);
 m3_ctext(130,"AMBASSADOR J. CHRISTOPHER STEVENS,",C_WHITE,1);m3_ctext(141,"SEAN SMITH, GLEN DOHERTY",C_WHITE,1);m3_ctext(152,"AND TYRONE WOODS.",C_WHITE,1);m3_present();
 cr_fade(920);pressed=0;bgm(1,0);}
static void credits_roll(int after){bgm(1,0);
 for(int y=0;y<160;y++){kl_poll();m3_fill(0,y,240,1,RGB(2+y/40,3+y/30,10+y/12));}
 for(int i=0;i<90;i++){int x=(i*97+13)%240,y=(i*53+7)%150+2;fb[y*240+x]=i%3?RGB(140,150,180):RGB(250,250,255);}
 fr_earth_bg(40,0);fr_save_bg();
 int tot=0;for(int i=0;i<CR_N;i++)tot+=cr_h(CRL[i].kind);
 s32 off=160<<4;int endY=-(tot-80);int hold=0,trib=0; // final line ("THANKS FOR PLAYING!") stops at the centre
 for(int t=0;;t++){BEAT->screen=SCR_CRED;BEAT->frame=frame;BEAT->phase=after;BEAT->x=off>>4;BEAT->menu=hold;
  dma32(bigbuf,fb,240*160/2);int y=off>>4;
  {int yy=y;for(int i=0;i<CR_N;i++){if(CRL[i].kind==7&&yy<112&&!trib){trib=1;cr_tribute();REG_BLDY=16;dma32(bigbuf,fb,240*160/2);m3_present();for(int k=0;k<16;k++){REG_BLDY=15-k;next_frame();}REG_BLDCNT=0;REG_BLDY=0;}yy+=cr_h(CRL[i].kind);}}
  // dark card behind each block
  {int cy=y,st=y;for(int i=0;i<CR_N;i++){int k=CRL[i].kind;if(k==6||i==CR_N-1){int e=cy+(i==CR_N-1?cr_h(k):0);if(e>0&&st<160)cr_dim(20,st-4,200,e-st+6);st=cy+cr_h(k);}cy+=cr_h(k);}}
  for(int i=0;i<CR_N;i++){int k=CRL[i].kind;int h=cr_h(k);if(y>-60&&y<160){
    if(k==0){if(y>=-1&&y<144)m3_ctext(y+2,CRL[i].s,C_YEL,2);}else if(y<0||y>152);else if(k==1)m3_ctext(y,CRL[i].s,C_WHITE,1);else if(k==2)m3_ctext(y,CRL[i].s,C_GRAY,1);else if(k==3)m3_ctext(y,CRL[i].s,RGB(110,200,230),1);
    else if(k==4)cr_kc(96,y+2);}
   y+=h;}
  if(hold>60)m3_ctext(150,"A / B: BACK",C_GRAY,1);
  m3_present();next_frame();
  int sp=(keys&K_A)?128:(keys&K_UP)?-32:44;if((off>>4)>endY){off-=sp;if(off>(200<<4))off=200<<4;}else{off=endY<<4;hold++;}
  if(hold>60&&(pressed&(K_A|K_B|K_START)))break;if(pressed&K_B&&hold==0)break;if(FR_AUTO&&hold>150)break;}
 bgCached=-1;pressed=0;}
