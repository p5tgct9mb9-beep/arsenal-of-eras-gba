// ===================================================================== v1.3.1 WHAT-IF SPACE RACE 1945 (fictional alternate history)
// Paper-project rockets flown as if the war's end had gone differently. Generic markings, no insignia, no leaders.
// The fact cards state plainly what really happened, including the forced labour behind the real A4/V-2.
static int wif_rk(const char*n){for(int i=0;i<NRK;i++){const char*a=RK[i].n,*b=n;while(*a&&*a==*b){a++;b++;}if(!*a&&!*b)return i;}return -1;}
static int wif_launch(const char*n){int ri=wif_rk(n);if(ri<0)return 0;const Rk*r=&RK[ri];lScore=0;lAuto=FR_AUTO?1:0;lIgn=0;lHull=3;lMsgT=0;int ok=0;
 if(r->kind==LK_HOP){ok=l_land(r,ri,LL_HOP)>0;}else{int a=l_ascent(r,ri);ok=(a==1&&r->kind!=LK_SUB)?l_orbit(r,ri):r->kind==LK_SUB?a>=1:a==1;}
 bgCached=-1;return ok?(lScore>100?lScore:100):0;}
static const char*const WFN[9]={"1 A4B: THE WINGED A4","2 RHEINBOTE: FOUR STAGES","3 WASSERFALL: RECON DODGE","4 NATTER: VERTICAL LAUNCH","5 A9/A10: ACROSS THE OCEAN","6 SILBERVOGEL: SKIP-GLIDER","7 ALLIED COUNTER: LANCER X","8 SOVIET COUNTER: ZARYA-45","9 FINALE: THE SUN GUN"};
static const char*const WFR[8]={"A4B WINGED","RHEINBOTE",0,"NATTER BA 349","A9/A10 AMERIKA","SILBERVOGEL","ALLIED LANCER X","SOVIET ZARYA-45"};
static const char*const WFF[9][2]={
 {"A4B","A WINGED A4 WAS LAUNCHED TWICE IN JANUARY 1945 TO TEST LONGER-RANGE GLIDE. THE REAL A4 (V-2) WAS BUILT BY CONCENTRATION-CAMP PRISONERS AT MITTELWERK; THOUSANDS DIED. THIS GAME DEPICTS HISTORY - IT DOES NOT HONOUR IT."},
 {"RHEINBOTE","RHEINBOTE WAS A REAL FOUR-STAGE SOLID ROCKET FIRED AT ANTWERP IN 1944-45. ITS TINY WARHEAD MADE IT MILITARILY USELESS."},
 {"WASSERFALL","WASSERFALL WAS A RADIO-GUIDED ANTI-AIRCRAFT ROCKET DERIVED FROM THE A4. IT WAS TESTED BUT NEVER ENTERED SERVICE."},
 {"NATTER","THE BA 349 NATTER WAS A VERTICAL-LAUNCH ROCKET INTERCEPTOR. ITS ONLY CREWED LAUNCH, ON 1 MARCH 1945, KILLED TEST PILOT LOTHAR SIEBER."},
 {"A9/A10","THE A9/A10 WAS A PAPER STUDY FOR A TWO-STAGE TRANSATLANTIC ROCKET. NOTHING LIKE IT FLEW; THE FIRST REAL ICBM WAS THE SOVIET R-7 IN 1957."},
 {"SILBERVOGEL","SANGER AND BREDT'S SILBERVOGEL WAS A SKIP-GLIDE BOMBER STUDY. POSTWAR ANALYSIS FOUND ITS REENTRY HEATING BADLY UNDERESTIMATED. IT WAS NEVER BUILT."},
 {"OPERATION PAPERCLIP","AFTER THE WAR THE US BROUGHT OVER 1,600 GERMAN SCIENTISTS AND ENGINEERS, INCLUDING VON BRAUN'S TEAM. LANCER X IS FICTIONAL."},
 {"OPERATION OSOAVIAKHIM","IN OCTOBER 1946 THE USSR MOVED THOUSANDS OF GERMAN SPECIALISTS EAST. KOROLEV'S BUREAU SOON SURPASSED THEIR WORK. ZARYA-45 IS FICTIONAL."},
 {"THE SUN GUN","A SPACE MIRROR WEAPON WAS DESCRIBED TO ALLIED INTERROGATORS IN 1945 AS A FAR-FUTURE IDEA. IT NEVER EXISTED BEYOND TALK - HERE IT IS A BOSS."}};
static void whatif_1945(void){v131_load();static int sel=0;
 for(;;){u8 dn[9];for(int i=0;i<9;i++)dn[i]=VS.prog[5]>i;int m=fr_menu("WHAT-IF 1945","FICTIONAL ROCKET RACE - REAL HISTORY CARDS",WFN,9,dn,&sel,SCR_WIF);if(m<0)return;
  if(m>VS.prog[5]&&!FR_AUTO){guide_tip("LOCKED","PLAY THE CHAIN IN ORDER.");continue;}
  if(VS.prog[5]==0&&m==0)guide_card("WHAT-IF 1945","BRIEFING","ALTERNATE HISTORY: THE PAPER ROCKETS OF 1945 FLY, AND THE ALLIES AND SOVIETS RACE TO COUNTER THEM. EVERY CARD AFTER A MISSION TELLS WHAT REALLY HAPPENED.",-1,0,0);
  int sc=0;
  if(m==2){int alt=40,ecm=3;bgm_t(TR_OLDC);int ok=1;for(int k=0;k<2&&ok;k++)ok=ov_threat(TH_WASSER,AC_RECON,&alt,&ecm);sc=ok?3000+ecm*500:0;}
  else if(m==8){bgm_t(TR_BOSS);int r=od_run(3);sc=r>=0?r:0;}
  else{if(m==7)bgm_t(TR_SOVMARCH);sc=wif_launch(WFR[m]);}
  fr_mode_done(5,m,sc,WFN,sc>0,sc>0?"":"THE ATTEMPT FAILED",WFF[m][0],WFF[m][1]);if(FR_AUTO)return;}}
