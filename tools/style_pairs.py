#!/usr/bin/env python3
# v1.3 before/after composites -> shots/v13/v13_style_<name>.png (BEFORE = pre-art-pass ROM, AFTER = release ROM)
import os
from PIL import Image,ImageDraw
P=[('campaign_brief','build/B/v13_cm_21/v13_cm21_brief.png','build/F/v13_cm_21/v13_cm21_brief.png'),
   ('campaign_play','build/B/v11_camp/v11_camp_bunker_approach.png','build/F/v11_camp/v11_camp_bunker_approach.png'),
   ('bradley','build/B/v13_brad/v13_bradley_1.png','build/F/v13_brad/v13_bradley_1.png'),
   ('6v6_era','build/B/v13_era_rev_inf/v13_era_rev_inf_play2.png','build/F/v13_era_rev_inf/v13_era_rev_inf_play2.png'),
   ('duel','build/B/v13_duel/v13_sword_clash.png','build/A/sty_duel/v13_style_duel_1d.png'),
   ('duel_slash','build/B/v13_duel/v13_sword_fight.png','build/A/sty_duel/v13_style_duel_2c.png'),
   ('fps','build/B/v13_fps_0/f010.png','build/A/sty_fps0/f010.png'),
   ('specops_ww1','build/B/v13_so_ww1_nomans/v13_so_ww1_nomans_2.png','build/F/v13_so_ww1_nomans/v13_so_ww1_nomans_2.png'),
   ('specops_wmap','build/B/v13_so_ww1_nomans/v13_so_wmap_w11.png','build/F/v13_so_ww1_nomans/v13_so_wmap_w11.png'),
   ('specops_ua','build/B/v13_ua/v13_ua_arty_call.png','build/F/v13_ua/v13_ua_arty_call.png')]
os.makedirs('shots/v13',exist_ok=True)
for n,b,a in P:
    if not(os.path.exists(b) and os.path.exists(a)):print('skip',n);continue
    m=Image.new('RGB',(960+12,320+18),(16,16,24));d=ImageDraw.Draw(m)
    for k,(f,t) in enumerate(((b,'BEFORE'),(a,'AFTER (v1.3 art pass)'))):
        m.paste(Image.open(f).convert('RGB').resize((240,160)).resize((480,320),Image.NEAREST),(k*492,18));d.text((k*492+4,3),t,fill=(255,230,120))
    m.save('shots/v13/v13_style_%s.png'%n);print('ok',n)
