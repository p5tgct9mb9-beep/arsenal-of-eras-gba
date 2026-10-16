// ===================================================================== v1.3.2 CREDITS - scrolls over Earth from orbit, menu music
#define SCR_CRED 97
typedef struct{const char*s;u8 kind;}CrLine; // kind: 0 heading(2x gold) 1 white 2 gray 3 cyan 4 KC portrait 5 gap 6 card break
static const CrLine CRL[]={
 {"ARSENAL OF ERAS",0},{"FIELD EDITION V1.3.2",3},{"",6},
 {"CREATED BY",2},{"LT VASQUEZ",0},{"GAME DIRECTOR",1},{"DESIGNER - IDEAS",1},{"",6},
 {"BUILT WITH",2},{"KC, AI ASSISTANT",0},{"(GROK BOT)",3},{"PROGRAMMING - ART - MUSIC",1},{"",6},
 {"TUTORIAL GUIDE",2},{"KC THE DOG",0},{"",4},{"BLACK SHEPHERD MIX, BIG EARS,",1},{"GREY MUZZLE, WHITE CHEST.",1},{"GOOD GIRL. BEST SERGEANT.",3},{"",6},
 {"SPECIAL THANKS",2},{"DAD",0},{"",6},
 {"THANK YOU ELON MUSK",0},{"FOR BEING THE BIGGEST",1},{"INSPIRATION FOR ME.",1},{"- LT",3},{"",6},
 {"MORSE TIP FROM THE RADIO ROOM:",2},{"... --- ...",3},{"",6},
 {"THANKS FOR PLAYING!",0}};
#define CR_N (int)(sizeof CRL/sizeof CRL[0])
static int cr_h(int k){switch(k){case 0:return 20;case 4:return 54;case 5:return 10;case 6:return 34;default:return 11;}}
static void cr_dim(int x0,int y0,int w,int h){if(y0<0){h+=y0;y0=0;}if(y0+h>160)h=160-y0;for(int y=y0;y<y0+h;y++){u16*p=&fb[y*240+x0];for(int x=0;x<w;x++){u16 c=p[x];p[x]=((c>>1)&0x3DEF);}}}
static void cr_kc(int x0,int y0){const u16*p=kc_portrait;for(int y=0;y<48;y++){int yy=y0+y;if(yy<0||yy>=160)continue;for(int x=0;x<48;x++){u16 c=p[y*48+x];if(!(c&0x8000))fb[yy*240+x0+x]=c;}}}
static void credits_roll(int after){bgm(1,0);
 for(int y=0;y<160;y++){kl_poll();m3_fill(0,y,240,1,RGB(2+y/40,3+y/30,10+y/12));}
 for(int i=0;i<90;i++){int x=(i*97+13)%240,y=(i*53+7)%150+2;fb[y*240+x]=i%3?RGB(140,150,180):RGB(250,250,255);}
 fr_earth_bg(40,0);fr_save_bg();
 int tot=0;for(int i=0;i<CR_N;i++)tot+=cr_h(CRL[i].kind);
 s32 off=160<<4;int endY=-(tot-80);int hold=0; // final line ("THANKS FOR PLAYING!") stops at the centre
 for(int t=0;;t++){BEAT->screen=SCR_CRED;BEAT->frame=frame;BEAT->phase=after;BEAT->x=off>>4;BEAT->menu=hold;
  dma32(bigbuf,fb,240*160/2);int y=off>>4;
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
