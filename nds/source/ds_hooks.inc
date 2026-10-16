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
