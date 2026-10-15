// ===================================================================== v1.3 SPEC OPS LOADOUT screen
#define SCR_SOLO 50
static const char*so_pname(int u,int slot,int idx){if(!idx)return slot?"NONE":cat("DEFAULT: ",WPN_N[PF_UW[u]]);int e,i;if(slot){e=fdec(NSEC,idx-1,&i);return SEC[e][i].n;}e=fdec(NPRIM,idx-1,&i);return PRIM[e][i].n;}
static const char*so_ptag(int slot,int idx){if(!idx)return "";int e,i;e=fdec(slot?NSEC:NPRIM,idx-1,&i);return ERA_TAG[e];}
static void so_menu(int u){int row=0,redraw=1;SoLo bak=SOL[u];
 static const char*const RN[11]={"PRIMARY","SECONDARY","MELEE","EQUIP 1 (L)","EQUIP 2 (DN+L)","OPTIC","MUZZLE","GRIP","MAGAZINE","RESET DEFAULT","DONE"};
 for(;;){BEAT->screen=SCR_SOLO;BEAT->menu=row;SoLo*l=&SOL[u];BEAT->extra=l->prim|(l->sec<<8)|(l->mel<<16)|(l->eq0<<20)|(l->eq1<<24);
  if(redraw){redraw=0;m3_title(1);m3_fill(2,2,236,156,C_PANEL);m3_box(2,2,236,156,2);
   m3_fill(6,6,40,40,RGB(20,30,70));m3_unit(u,10,10,2,0);flag_tag(33,35,unit_nat(u));
   m3_text(50,7,"SPEC OPS LOADOUT",C_YEL,1);m3_text(50,18,PF_US[u],C_WHITE,1);m3_text(50,29,"SAVED PER UNIT",C_GRAY,1);
   const char*vals[11];vals[0]=so_pname(u,0,l->prim);vals[1]=so_pname(u,1,l->sec);vals[2]=MEL[l->mel].n;vals[3]=EQ_N[l->eq0];vals[4]=EQ_N[l->eq1];
   vals[5]=ATTG[ATL[0][2][l->opt]].n;vals[6]=ATTG[ATL[1][2][l->muz]].n;vals[7]=ATTG[ATL[2][2][l->grip]].n;vals[8]=ATTG[ATL[3][2][l->mag]].n;vals[9]="";vals[10]="";
   SoW cur,def;so_calc(u,0,&cur);SoLo sv=*l;SoLo d0;so_defaults(u);d0=SOL[u];SOL[u]=sv;{SoLo t=*l;*l=d0;so_calc(u,0,&def);*l=t;}
   int melU=(PF_UW[u]==11||PF_UW[u]==12)&&!l->prim;
   for(int r=0;r<11;r++){int y=50+r*9;int dis=(melU&&r>=5&&r<=8);u16 c=r==row?C_YEL:dis?RGB(80,80,96):C_WHITE;
    m3_text(6,y,r==row?">":" ",C_YEL,1);m3_text(12,y,RN[r],r==row?C_YEL:C_GRAY,1);m3_text(98,y,vals[r],c,1);
    if(r==0||r==1){const char*t=so_ptag(r,r?l->sec:l->prim);if(*t)m3_text(236-slen(t)*6-2,y,t,C_BLUE,1);}}
   // stat bars + delta vs unit default
   static const char*const SN[4]={"DMG","ROF","RNG","MOB"};int a[4],b[4];so_bars(&cur,a);so_bars(&def,b);
   for(int k=0;k<4;k++){int x=184,y=6+k*10;m3_text(150,y,SN[k],C_GRAY,1);m3_fill(x,y+1,40,5,RGB(40,40,60));int v=a[k]*40/100,w0=b[k]*40/100;m3_fill(x,y+1,v<w0?v:w0,5,C_WHITE);
    int dv=a[k]-b[k];if(dv)m3_text(228,y,dv>0?"+":"-",dv>0?C_GREEN:C_RED,1);
    if(dv>0)m3_fill(x+w0,y+1,v-w0,5,C_GREEN);else if(dv<0)m3_fill(x+v,y+1,w0-v,5,C_RED);}
   const char*hint=row==3?EQ_D[l->eq0]:row==4?EQ_D[l->eq1]:row>=5&&row<=8?ATTG[ATL[row-5][2][((u8*)&l->opt)[row-5]]].d:row==2?"AUTO WHEN AN ENEMY IS CLOSE":row==1?"SELECT IN COURSE: SWAP WEAPON":row==0?"ANY ERA - OR THE UNIT'S OWN":"";
   m3_fill(4,149,232,8,C_PANEL);m3_ctext(149,hint,C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){row=(row+10)%11;redraw=1;sfx_menu();}if(pressed&K_DOWN){row=(row+1)%11;redraw=1;sfx_menu();}
  int d=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;if(keys&K_R)d*=(pressed&(K_RIGHT|K_LEFT))?10:1;
  if(d){SoLo*l=&SOL[u];int np=fsum(NPRIM)+1,ns=fsum(NSEC)+1;
   switch(row){case 0:l->prim=(l->prim+np*10+d)%np;break;case 1:l->sec=(l->sec+ns*10+d)%ns;break;case 2:l->mel=(l->mel+NMEL+d)%NMEL;break;
    case 3:l->eq0=(l->eq0+NEQ+d)%NEQ;break;case 4:l->eq1=(l->eq1+NEQ+d)%NEQ;break;
    case 5:l->opt=(l->opt+NATL[0][2]+d%NATL[0][2])%NATL[0][2];break;case 6:l->muz=(l->muz+NATL[1][2]+d%NATL[1][2])%NATL[1][2];break;
    case 7:l->grip=(l->grip+NATL[2][2]+d%NATL[2][2])%NATL[2][2];break;case 8:l->mag=(l->mag+NATL[3][2]+d%NATL[3][2])%NATL[3][2];break;}
   redraw=1;sfx_menu();}
  if(pressed&K_A){if(row==9){so_defaults(u);redraw=1;sfx_ok();}else if(row==10){so_write();sfx_ok();return;}}
  if(pressed&K_START){so_write();sfx_ok();return;}
  if(pressed&K_B){so_write();sfx_menu();return;}(void)bak;}}
