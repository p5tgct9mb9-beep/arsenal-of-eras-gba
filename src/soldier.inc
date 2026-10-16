// ---------------------------------------------------------------- v1.3.6 SOLDIERS: 3/4 top-down infantry (32x32 split 32x16), recoloured at runtime
#include "soldierdb.inc"
#define SDT_P 384      // player set (120 tiles)
#define SDT_A 504      // team A look
#define SDT_B 624      // team B look
#define SDT_PLATE 744  // 12 nameplates x 4 tiles
#define SDT_MARK 792   // 3 chevrons (blue, red, yellow)
#define SDB_A1 7
#define SDB_A2 8
#define SDB_B1 11
#define SDB_B2 12
#define SDB_P 10
#define SDB_UI 9
static u8 sdbuf[15*256] EWRAM;static u8 sdOn,sdFire[MAXU];static s16 sdAnim[MAXU];static u8 sdMove[MAXU];
static void sd_unrle(const u8*s,u8*d,int n){int i=0;while(i<n){u8 b=*s++;if(b==0){int k=*s++;while(k--&&i<n)d[i++]=0;}else d[i++]=b;}}
static void sd_upload(int set,int tile,int camo){sd_unrle(sd_rle+sd_off[set],sdbuf,15*256);
 if(camo>0&&camo<5)for(int f=0;f<15;f++)for(int t=0;t<8;t++)for(int y=0;y<8;y++)for(int xb=0;xb<4;xb++){int i=f*256+t*32+y*4+xb;u8 b=sdbuf[i];if(!b)continue;
   int Y=((t>>2)*8+y+(f>=6?16:0))&31,X0=(t&3)*8+xb*2;
   for(int h=0;h<2;h++){int p=(b>>(4*h))&15;if(p==3||p==4){int l=sd_camo_g[(camo-1)*1024+Y*32+X0+h];if(l){p=l==1?5:6;b=(u8)((b&~(15<<(4*h)))|(p<<(4*h)));}}}sdbuf[i]=b;}
 dma32(sdbuf,(u8*)OBJ_TILES+tile*32,15*256/4);}
static void sd_pal(int bank,int uni,int skin,int hair,int beret,int plate){volatile u16*p=PAL_OBJ+bank*16;const u16*U=sd_uni+uni*28;const u16*hl=beret?sd_beret:sd_helm+uni*7;const u16*g=sd_gear+(uni*2+(plate?1:0))*7;
 p[1]=RGB(10,10,8);p[2]=U[1];p[3]=U[3];p[4]=U[5];p[5]=U[7+3];p[6]=U[14+3];p[7]=sd_skin[skin*6+1];p[8]=sd_skin[skin*6+4];p[9]=sd_hair[hair*6+3];
 p[10]=hl[2];p[11]=hl[5];p[12]=g[3];p[13]=sd_boot[uni*7+2];p[14]=RGB(62,63,68);p[15]=RGB(128,82,44);}
static int sd_look(int era,int f){int east=f==2||f==5||f==6||f==10||f==FAC_WP,ax=FAC[f].axis;
 switch(era){case 0:return ax?(f==4?9:1):0;case 1:return east?3:2;case 2:return east?5:4;case 3:case 4:return 6;case 5:return 7;default:return 8;}}
static int sd_combo(int era){int rifle=(era==0||era>=5)?1:0;return ((((CH.hat*3+CH.head)*4+CH.vest)*2+CH.body)*2+rifle);}
// tiny 3x5 font for nameplates
static int sdf_idx(char c){if(c>='0'&&c<='9')return c-'0';if(c>='A'&&c<='Z')return 10+c-'A';if(c=='-')return 37;if(c=='.')return 38;return 36;}
static void sd_plate(int k,const char*name,int team){u32 t[32];memset(t,0,sizeof t);int n=0;while(name[n]&&n<7)n++;int w=n*4+3,x0=(32-w)/2;int bg=team==2?8:team?7:6;
 for(int y=0;y<8;y++)for(int x=x0;x<x0+w;x++){int c=(y==0||y==7||x==x0||x==x0+w-1)?1:bg;t[(x>>3)*8+y]|=(u32)c<<((x&7)*4);}
 for(int i=0;i<n;i++){u16 g=sd_font[sdf_idx(name[i])];for(int r=0;r<5;r++)for(int c=0;c<3;c++)if(g&(1<<(14-r*3-c))){int x=x0+2+i*4+c,y=1+r;t[(x>>3)*8+y]=(t[(x>>3)*8+y]&~(15u<<((x&7)*4)))|(2u<<((x&7)*4));}}
 dma32(t,(u8*)OBJ_TILES+(SDT_PLATE+k*4)*32,32);}
static void sd_marks(void){for(int m=0;m<3;m++){u32 t[8];static const u8 SH[5]={0x7F,0x3E,0x1C,0x08,0};for(int y=0;y<8;y++){u32 w=0;for(int x=0;x<8;x++){int c=0;
   if(y<5&&((SH[y]>>(7-x))&1)){int edge=y==0||!((SH[y]>>(8-x))&1)||!((SH[y]>>(6-x))&1)||(y<4&&!((SH[y+1]>>(7-x))&1));c=edge?1:3+m;}w|=(u32)c<<(x*4);}t[y]=w;}
  dma32(t,(u8*)OBJ_TILES+(SDT_MARK+m)*32,8);}
 volatile u16*p=PAL_OBJ+SDB_UI*16;p[1]=RGB(8,8,10);p[2]=RGB(240,240,228);p[3]=RGB(90,170,255);p[4]=RGB(240,70,60);p[5]=RGB(255,220,60);p[6]=RGB(24,44,84);p[7]=RGB(90,24,22);p[8]=RGB(84,70,10);}
static const char*const SD_NM[24]={"HALE","REYES","BROOKS","DIAZ","KOWAL","OKAFOR","NAKANO","LUND","VOSS","MARSH","CRANE","PIKE",
 "BAKER","FORD","SILVA","GRANT","IVES","KANE","LOPEZ","MOSS","NOLAN","PRICE","QUINN","WARD"};
static int sdEraT[2],sdFacT[2];
static void sd_match_setup(int era0,int era1,int f0,int f1){sdOn=1;sdEraT[0]=era0;sdEraT[1]=era1;sdFacT[0]=f0;sdFacT[1]=f1;
 sd_upload(SD_LOOK0+sd_look(era0,f0),SDT_A,0);sd_upload(SD_LOOK0+sd_look(era1,f1),SDT_B,0);
 int lp0=sd_look(era0,f0),lp1=sd_look(era1,f1);int pl0=lp0==4||lp0==5||lp0==6,pl1=lp1==4||lp1==5||lp1==6;
 sd_pal(SDB_A1,8+f0,1,0,0,pl0);sd_pal(SDB_A2,8+f0,4,1,0,pl0);sd_pal(SDB_B1,8+f1,2,1,0,pl1);sd_pal(SDB_B2,8+f1,0,2,0,pl1);
 if(CH.use){sd_upload(sd_combo(era0),SDT_P,CH.camo);sd_pal(SDB_P,CH.uni,CH.skin,CH.hair,CH.hat==1,CH.vest==2);}
 else{sd_upload(SD_LOOK0+lp0,SDT_P,0);sd_pal(SDB_P,8+f0,2,0,0,pl0);volatile u16*p=PAL_OBJ+SDB_P*16;p[12]=RGB(200,170,60);}
 for(int k=0;k<12;k++)sd_plate(k,k==0?CH.name:SD_NM[(k+(f0*5))%24],k==0?2:k<6?0:1);
 sd_marks();for(int k=0;k<MAXU;k++){sdFire[k]=0;sdAnim[k]=U[k].anim;sdMove[k]=0;}}
static void sd_spr(int x,int y,int tile,int pal,int hf,int prio){if(noam>=128||x<=-32||x>=240||y<=-16||y>=160)return;u16*o=&oam[noam*4];
 o[0]=(y&255)|(1<<14);o[1]=(x&511)|(2<<14)|(hf?1<<12:0);o[2]=tile|(prio<<10)|(pal<<12);noam++;}
static int sd_view(int t){t&=255;if(t<32||t>=224)return 2;if(t<96)return 1;if(t<160)return 0;return 3;}
// draw a soldier whose map position is screen (x,y); k = unit index (team/bank/plate)
static void sd_draw_unit(int k,int x,int y){Unit*u=&U[k];int v=sd_view(u->tur>>8),fl=v==3;if(fl)v=1;
 if(u->anim!=sdAnim[k]){sdAnim[k]=u->anim;sdMove[k]=8;}else if(sdMove[k])sdMove[k]--;
 int tb=k==0?SDT_P:u->team?SDT_B:SDT_A,bank=k==0?SDB_P:u->team?((k&1)?SDB_B2:SDB_B1):((k&1)?SDB_A2:SDB_A1);
 static const u8 WC[4]={1,0,2,0};int lf=sdMove[k]?WC[(u->anim>>2)&3]:0;int fire=sdFire[k]>0;if(sdFire[k])sdFire[k]--;
 int ox=v==1?(fl?-20:-11):-16;
 sd_spr(x+ox,y-22,tb+(v*2+fire)*8,bank,fl,1);sd_spr(x+ox,y-6,tb+(6+v*3+lf)*8,bank,fl,1);}
static void sd_draw_tags(int k,int x,int y){Unit*u=&U[k];Unit*P=&U[0];
 int near=k==0||idist((u->x-P->x)>>8,(u->y-P->y)>>8)<(u->team?64:84);
 if(near&&k<12){spr(x-16,y-34,SDT_PLATE+k*4,1,1,SDB_UI,0);}
 else spr(x-4,y-28,SDT_MARK+(k==0?2:u->team?1:0),0,0,SDB_UI,0);}
