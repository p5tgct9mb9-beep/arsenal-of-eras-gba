// ---------------------------------------------------------------- CHARACTER CREATOR (saved to SRAM, used as the player sprite on foot)
static const char*const CHS_SKIN[CH_NSKIN]={"TONE 1 (LIGHT)","TONE 2","TONE 3","TONE 4","TONE 5","TONE 6 (DEEP)"};
static const char*const CHS_HAIR[CH_NHAIR]={"BROWN","BLACK","BLOND","RED","GREY"};
static const char*const CHS_HEAD[CH_NHEAD]={"CLEAN SHAVEN","STUBBLE / SHAVED","LONG HAIR / TIED"};
static const char*const CHS_HAT[CH_NHAT]={"HELMET","BERET","PATROL CAP","BOONIE HAT","NONE"};
static const char*const CHS_UNI[CH_NUNI]={"OLIVE DRAB","KHAKI","FIELD GREY","NAVY","BLACK","DESERT TAN","SNOW WHITE","URBAN GREY"};
static const char*const CHS_CAMO[CH_NCAMO]={"SOLID","WOODLAND","SPLINTER","DIGITAL","TIGER STRIPE"};
static const char*const CHS_VEST[CH_NVEST]={"NONE","WEBBING","PLATE CARRIER","RADIO PACK"};
static const char*const CHS_BODY[2]={"STANDARD","SLIM"};
static void ch_preview(int px,int py,int f,int sc){volatile u16*pal=PAL_OBJ+6*16;m3_fill(px-2,py-2,16*sc+4,16*sc+4,RGB(60,70,50));m3_fill(px,py,16*sc,16*sc,RGB(84,98,66));
 for(int y=0;y<16;y++)for(int x=0;x<16;x++){int c=chpx[f][y][x];if(c)m3_fill(px+x*sc,py+y*sc,sc,sc,pal[c]);}}
static void name_entry(void){static const char K[4][11]={"ABCDEFGHIJ","KLMNOPQRST","UVWXYZ0123","456789-. '"};int cx=0,cy=0,redraw=1;char nb[11];int n=slen(CH.name);memcpy(nb,CH.name,11);
 for(;;){BEAT->screen=SCR_LOADOUT;BEAT->menu=100+cy*10+cx;
  if(redraw){m3_fill(4,4,232,152,RGB(14,18,36));m3_box(4,4,232,152,2);m3_ctext(10,"ENTER NAME (MAX 10)",C_YEL,1);
   m3_fill(60,26,120,14,RGB(30,36,60));m3_ctext(29,n?nb:"_",C_WHITE,1);
   for(int r=0;r<4;r++)for(int c=0;c<10;c++){int x=30+c*18,y=52+r*16;char s[2]={K[r][c],0};if(s[0]==' ')s[0]='_';if(r==cy&&c==cx)m3_fill(x-3,y-3,12,13,RGB(80,80,20));m3_text(x,y,s,(r==cy&&c==cx)?C_YEL:C_WHITE,1);}
   {int y=120;m3_text(60,y,cy==4&&cx<5?"> DEL":"  DEL",cy==4&&cx<5?C_YEL:C_WHITE,1);m3_text(140,y,cy==4&&cx>=5?"> DONE":"  DONE",cy==4&&cx>=5?C_YEL:C_WHITE,1);}
   m3_ctext(146,"A: TYPE  B: DELETE  START: DONE",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&K_LEFT){cx=(cx+9)%10;redraw=1;}if(pressed&K_RIGHT){cx=(cx+1)%10;redraw=1;}
  if(pressed&K_UP){cy=(cy+4)%5;redraw=1;}if(pressed&K_DOWN){cy=(cy+1)%5;redraw=1;}
  if(pressed&K_A){if(cy==4){if(cx<5){if(n>0)nb[--n]=0;}else break;}else if(n<10){nb[n++]=K[cy][cx];nb[n]=0;}sfx_menu();redraw=1;}
  if(pressed&K_B){if(n>0)nb[--n]=0;sfx_menu();redraw=1;}
  if(pressed&K_START)break;}
 while(n>0&&nb[n-1]==' ')nb[--n]=0;if(n==0){memcpy(nb,"SOLDIER",8);}memcpy(CH.name,nb,11);sfx_ok();}
static void char_creator(void){enum{C_NAME,C_SKIN,C_HAIR,C_HEAD,C_HAT,C_UNI,C_CAMO,C_VEST,C_BODY,C_USE,C_DONE,C_N};
 static const char*const LB[C_N]={"NAME","SKIN","HAIR","FACE","HEADGEAR","UNIFORM","CAMO","GEAR","BUILD","IN GAME",""};
 int sel=0,redraw=2,f=0;
 for(int t=0;;t++){BEAT->screen=SCR_LOADOUT;BEAT->menu=200+sel;BEAT->extra=CH.skin|(CH.hat<<4)|(CH.uni<<8)|(CH.camo<<12)|(CH.vest<<16);
  if(t%24==0){f^=1;if(!redraw)redraw=3;}
  if(redraw){if(redraw!=3){ch_build();ch_palette(6);
    if(redraw==2)m3_title(1);m3_fill(4,4,232,152,RGB(14,18,36));m3_box(4,4,232,152,2);m3_ctext(8,"CHARACTER CREATOR",C_YEL,1);
    for(int k=0;k<C_N;k++){int y=22+k*11;u16 c=k==sel?C_YEL:C_WHITE;
     if(k==C_DONE){m3_text(10,y+2,k==sel?"> SAVE & BACK":"  SAVE & BACK",c,1);continue;}
     const char*v=k==C_NAME?CH.name:k==C_SKIN?CHS_SKIN[CH.skin]:k==C_HAIR?CHS_HAIR[CH.hair]:k==C_HEAD?CHS_HEAD[CH.head]:k==C_HAT?CHS_HAT[CH.hat]:k==C_UNI?CHS_UNI[CH.uni]:k==C_CAMO?CHS_CAMO[CH.camo]:k==C_VEST?CHS_VEST[CH.vest]:k==C_BODY?CHS_BODY[CH.body]:CH.use?"ON":"OFF (ERA SOLDIER)";
     m3_text(7,y,LB[k],k==sel?C_YEL:C_GRAY,1);m3_text(63,y,v,c,1);if(k==sel&&k!=C_NAME){m3_text(57,y,"<",C_YEL,1);m3_text(63+slen(v)*6+1,y,">",C_YEL,1);}}
    m3_text(162,100,"LIVE PREVIEW",C_GRAY,1);m3_text(162,110,trunc12(CH.name),C_BLUE,1);
    m3_ctext(146,sel==C_NAME?"A: EDIT NAME (ON-SCREEN KEYBOARD)":"LEFT/RIGHT: CHANGE   B: SAVE & BACK",C_GRAY,1);}
   ch_preview(166,26,f,4);m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){sel=(sel+C_N-1)%C_N;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%C_N;redraw=1;sfx_menu();}
  int d=0;if(pressed&K_LEFT)d=-1;if(pressed&K_RIGHT)d=1;if((pressed&K_A)&&sel!=C_NAME&&sel!=C_DONE)d=1;
  if(d){switch(sel){case C_SKIN:CH.skin=(CH.skin+CH_NSKIN+d)%CH_NSKIN;break;case C_HAIR:CH.hair=(CH.hair+CH_NHAIR+d)%CH_NHAIR;break;case C_HEAD:CH.head=(CH.head+CH_NHEAD+d)%CH_NHEAD;break;
    case C_HAT:CH.hat=(CH.hat+CH_NHAT+d)%CH_NHAT;break;case C_UNI:CH.uni=(CH.uni+CH_NUNI+d)%CH_NUNI;break;case C_CAMO:CH.camo=(CH.camo+CH_NCAMO+d)%CH_NCAMO;break;
    case C_VEST:CH.vest=(CH.vest+CH_NVEST+d)%CH_NVEST;break;case C_BODY:CH.body^=1;break;case C_USE:CH.use^=1;break;}if(sel!=C_NAME&&sel!=C_DONE){redraw=1;sfx_menu();}}
  if((pressed&K_A)&&sel==C_NAME){name_entry();redraw=2;}
  if(((pressed&(K_A|K_START))&&sel==C_DONE)||(pressed&K_B)){save_write();sfx_ok();return;}}}
static void loadout_hub(int*era){static int sel=0;static const char*const IT[4]={"CAMPAIGN KITS (BY ERA)","6V6 ARSENAL (ALL ERAS)","CHARACTER CREATOR","BACK"};
 static const char*const DS[4]={"WEAPONS + VEHICLES PER ERA FOR MISSIONS","ONE POOL: ANY WEAPON, TANK OR AIRCRAFT","BUILD YOUR SOLDIER - NAME, LOOK, GEAR",""};int redraw=1;
 for(;;){BEAT->screen=SCR_LOADOUT;BEAT->menu=300+sel;
  if(redraw){m3_title(1);m3_fill(20,30,200,100,RGB(14,18,36));m3_box(20,30,200,100,2);m3_ctext(38,"LOADOUT",C_YEL,2);
   for(int k=0;k<4;k++)m3_text(40,64+k*12,cat(k==sel?"> ":"  ",IT[k]),k==sel?C_YEL:C_WHITE,1);m3_ctext(118,DS[sel],C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){sel=(sel+3)%4;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%4;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();if(sel==0)loadout_menu(0,*era);else if(sel==1){static int m=1;arsenal_menu(1);}else if(sel==2)char_creator();else return;redraw=1;continue;}
  if(pressed&K_B){sfx_menu();return;}}}
