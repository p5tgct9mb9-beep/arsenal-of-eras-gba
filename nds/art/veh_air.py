#!/usr/bin/env python3
"""veh_air.py - ds-v0.3.0 top-down aircraft sprites, rebuilt from real dimensions (plane_spr, 158 x 32x32).
Each real aircraft has a spec: length and span (m), wing planform (taper / elliptical / gull / swept / delta /
cranked delta / swing wing / biplane / triplane / flying wing), wing position, root and tip chord, sweep,
engine layout (nose prop, twin or four wing props, pusher, single or twin jets, wing-pod jets), tail type
(single fin, twin fins, twin booms, canted fins, T-tail, tailless, canards), canopy position, rotor for
helicopters.  It is drawn with veh_hd.Model (4x height field, front-left light) and reduced to 32x32 with the
team palette: camo 2<3<4, metal 12<13<14<11, glass 10, prop blur 11/7, afterburner 15, team marking 9.
Scale: the larger of span / length keeps the size of the GBA sprite, the other follows the real ratio;
fuselages and nacelles are drawn 40% fatter than true scale so they read at 32 px.
Nose points up (y = 0), as in the GBA art."""
import math, numpy as np
import veh_hd as H
from veh_hd import Model, CAMO, METAL, DARK, TEAM, GLASS, GOLD
def A(L, B, wing='t', wp=.33, cr=None, ct=None, sw=0, eng='p1', tail='s', tb=None, can=.3, fw=1.0, **k):
    d = dict(L=L, B=B, wing=wing, wp=wp, cr=cr or L * .24, ct=ct or (cr or L * .24) * .5, sw=sw, eng=eng, tail=tail,
             tb=tb or B * .34, can=can, fw=fw); d.update(k); return d
F = lambda L, B, **k: A(L, B, **k)                                      # WWII single-engine fighter defaults
SPEC = {
 'P-51D MUSTANG': F(9.83, 11.28, wp=.36, cr=2.6, ct=1.2, scoop=1),
 'SPITFIRE IX': F(9.47, 11.23, wing='e', wp=.30, cr=2.5, ct=1.0),
 'F6F HELLCAT': F(10.24, 13.06, wp=.30, cr=2.9, ct=1.5, fw=1.25),
 'P-47 THUNDERBOLT': F(11.0, 12.42, wing='e', wp=.32, cr=3.0, ct=1.3, fw=1.3),
 'F4U CORSAIR': F(10.2, 12.5, wing='g', wp=.36, cr=2.8, ct=1.4, can=.42),
 'YAK-3': F(8.5, 9.2, wp=.34, cr=2.2, ct=1.0),
 'HURRICANE IIC': F(9.84, 12.19, wp=.32, cr=2.6, ct=1.3, guns=4),
 'P-38 LIGHTNING': A(11.53, 15.85, wp=.38, cr=2.6, ct=1.1, eng='p2', tail='boom', tb=4.9, ne=.27, pod=1),
 'BF 109 G': F(9.02, 9.92, wp=.30, cr=2.1, ct=1.0, sq=1),
 'FW 190 A': F(8.95, 10.5, wp=.28, cr=2.3, ct=1.2, fw=1.2, radial=1),
 'ME 262': A(10.6, 12.6, wing='sw', sw=18, wp=.36, cr=3.0, ct=1.4, eng='jw2', ne=.25),
 'A6M ZERO': F(9.06, 12.0, wp=.30, cr=2.6, ct=1.3, radial=1, rt=1),
 'JU 87 STUKA': F(11.1, 13.8, wing='g', wp=.30, cr=2.9, ct=1.4, gear=1),
 'SBD DAUNTLESS': F(10.09, 12.66, wp=.32, cr=3.0, ct=1.3, radial=1, can=.36),
 'B-17 FLYING FORT.': A(22.66, 31.62, wp=.32, cr=5.6, ct=2.0, eng='p4', tb=13.2, ne=(.18, .34), can=.12, fw=1.1, radial=1),
 'B-25 MITCHELL': A(16.13, 20.6, wp=.33, cr=3.8, ct=1.6, eng='p2', tail='tw', tb=6.6, ne=.2, can=.18),
 'P-40 WARHAWK': F(9.66, 11.38, wp=.30, cr=2.6, ct=1.3),
 'P-39 AIRACOBRA': F(9.2, 10.4, wp=.36, cr=2.5, ct=1.2, can=.28),
 'F4F WILDCAT': F(8.76, 11.58, wp=.32, cr=2.4, ct=1.3, sq=1, radial=1, fw=1.3),
 'TBF AVENGER': F(12.48, 16.51, wp=.30, cr=3.4, ct=1.6, radial=1, can=.30, fw=1.3),
 'MOSQUITO': A(13.57, 16.52, wp=.30, cr=3.3, ct=1.3, eng='p2', ne=.2, can=.16),
 'TYPHOON IB': F(9.73, 12.67, wp=.30, cr=2.7, ct=1.5, fw=1.25),
 'LANCASTER': A(21.11, 31.09, wp=.30, cr=5.0, ct=1.7, eng='p4', tail='tw', tb=10.0, ne=(.15, .33), can=.14),
 'FW 190 D-9': F(10.2, 10.5, wp=.30, cr=2.3, ct=1.2),
 'BF 110': A(12.3, 16.25, wp=.30, cr=2.9, ct=1.3, eng='p2', tail='tw', tb=5.0, ne=.2, can=.30),
 'JU 88': A(14.36, 20.08, wp=.32, cr=3.4, ct=1.4, eng='p2', ne=.2, can=.1),
 'HE 111': A(16.4, 22.6, wing='e', wp=.30, cr=4.2, ct=1.6, eng='p2', ne=.2, can=.05),
 'ME 163 KOMET': A(5.98, 9.33, wing='sw', sw=23, wp=.35, cr=2.8, ct=1.2, eng='r', tail='n', fin=1),
 'IL-2 STURMOVIK': F(11.6, 14.6, wp=.33, cr=3.0, ct=1.4, can=.33),
 'LA-5FN': F(8.67, 9.8, wp=.30, cr=2.4, ct=1.1, radial=1),
 'LA-7': F(8.6, 9.8, wp=.30, cr=2.4, ct=1.1, radial=1),
 'KI-43 OSCAR': F(8.92, 10.84, wp=.30, cr=2.4, ct=1.1, radial=1, rt=1),
 'KI-84 HAYATE': F(9.92, 11.24, wp=.30, cr=2.4, ct=1.2, radial=1),
 'N1K2 GEORGE': F(9.35, 11.99, wp=.30, cr=2.6, ct=1.2, radial=1),
 'B5N KATE': F(10.3, 15.52, wp=.30, cr=3.2, ct=1.4, radial=1, can=.32, lc=1),
 'D3A VAL': F(10.2, 14.37, wing='e', wp=.30, cr=3.0, ct=1.2, radial=1, can=.30, gear=1),
 'P-61 BLACK WIDOW': A(15.11, 20.12, wp=.33, cr=3.6, ct=1.4, eng='p2', tail='boom', tb=5.6, ne=.22, pod=1, radial=1),
 'P-63 KINGCOBRA': F(9.96, 11.68, wp=.36, cr=2.5, ct=1.2, can=.28),
 'F2A BUFFALO': F(8.03, 10.67, wp=.32, cr=2.4, ct=1.2, radial=1, fw=1.4),
 'F8F BEARCAT': F(8.61, 10.92, wp=.30, cr=2.6, ct=1.2, radial=1, fw=1.25, bub=1),
 'P-80 SHOOTING STAR': A(10.49, 11.81, wp=.38, cr=2.6, ct=1.3, eng='j1', tip=1, can=.24),
 'B-24 LIBERATOR': A(20.6, 33.5, wp=.30, cr=4.0, ct=1.4, eng='p4', tail='tw', tb=8.0, ne=(.14, .30), can=.1, radial=1),
 'B-26 MARAUDER': A(17.8, 21.65, wp=.30, cr=3.6, ct=1.6, eng='p2', ne=.2, can=.12, radial=1),
 'B-29 SUPERFORT.': A(30.18, 43.05, wp=.36, cr=5.6, ct=1.8, eng='p4', tb=10.0, ne=(.14, .30), can=.04, radial=1),
 'A-20 HAVOC': A(14.63, 18.69, wp=.30, cr=3.4, ct=1.4, eng='p2', ne=.2, can=.18, radial=1),
 'PBY CATALINA': A(19.47, 31.7, wp=.30, cr=3.2, ct=2.6, eng='p2', ne=.12, can=.18, para=1, floats=1),
 'SB2C HELLDIVER': F(11.18, 15.17, wp=.30, cr=3.2, ct=1.5, radial=1, can=.32),
 'BEAUFIGHTER': A(12.6, 17.63, wp=.30, cr=3.0, ct=1.4, eng='p2', ne=.18, can=.15, radial=1),
 'TEMPEST V': F(10.26, 12.5, wing='e', wp=.32, cr=2.6, ct=1.2, fw=1.25),
 'HALIFAX': A(21.82, 31.6, wp=.30, cr=4.6, ct=1.8, eng='p4', tail='tw', tb=9.0, ne=(.14, .32), can=.12),
 'WELLINGTON': A(19.69, 26.26, wp=.30, cr=4.6, ct=1.8, eng='p2', ne=.2, can=.12, radial=1),
 'SWORDFISH': A(10.87, 13.87, wing='bi', wp=.24, cr=1.8, ct=1.8, eng='p1', radial=1, can=.36),
 'FAIREY FIREFLY': F(11.46, 13.56, wp=.30, cr=2.9, ct=1.4, can=.34),
 'GLOSTER METEOR': A(13.59, 13.11, wp=.40, cr=3.2, ct=1.6, eng='jn2', ne=.22, can=.18, tb=4.7),
 'BF 109 K-4': F(9.02, 9.92, wp=.30, cr=2.1, ct=1.0, sq=1),
 'TA 152 H': F(10.82, 14.44, wp=.30, cr=2.4, ct=1.0),
 'ME 410': A(12.48, 16.35, wp=.30, cr=3.0, ct=1.3, eng='p2', ne=.2, can=.18),
 'DO 335 PFEIL': A(13.85, 13.8, wp=.33, cr=3.0, ct=1.4, eng='pp', tail='x', can=.28),
 'AR 234 BLITZ': A(12.64, 14.41, wp=.40, cr=2.6, ct=1.4, eng='jw2', ne=.24, can=.03, tb=4.8),
 'HE 162 SALAMANDER': A(9.05, 7.2, wp=.42, cr=2.0, ct=1.0, eng='jd', tail='tw', tb=3.0, can=.16),
 'JU 87 G KANONEN': F(11.5, 15.0, wing='g', wp=.30, cr=2.9, ct=1.4, gear=1, cannon=1),
 'YAK-9': F(8.55, 9.74, wp=.33, cr=2.2, ct=1.0),
 'YAK-1': F(8.48, 10.0, wp=.33, cr=2.2, ct=1.0),
 'MIG-3': F(8.25, 10.2, wp=.34, cr=2.3, ct=1.0),
 'LAGG-3': F(8.81, 9.8, wp=.33, cr=2.4, ct=1.0),
 'PE-2': A(12.66, 17.16, wp=.33, cr=3.0, ct=1.3, eng='p2', tail='tw', tb=5.6, ne=.2, can=.2),
 'TU-2': A(13.8, 18.86, wp=.32, cr=3.4, ct=1.3, eng='p2', tail='tw', tb=6.0, ne=.2, can=.2, radial=1),
 'IL-10': F(11.12, 13.4, wp=.33, cr=2.8, ct=1.3, can=.32),
 'P-39 (VVS)': F(9.2, 10.4, wp=.36, cr=2.5, ct=1.2, can=.28),
 'KI-61 HIEN': F(8.94, 12.0, wp=.32, cr=2.4, ct=1.0),
 'KI-100': F(8.82, 12.0, wp=.32, cr=2.4, ct=1.0, radial=1),
 'J2M RAIDEN': F(9.95, 10.8, wp=.32, cr=2.6, ct=1.2, radial=1, fw=1.35),
 'A6M5 ZERO': F(9.12, 11.0, wp=.30, cr=2.6, ct=1.3, radial=1, rt=1),
 'G4M BETTY': A(20.0, 25.0, wp=.30, cr=4.4, ct=1.6, eng='p2', ne=.18, can=.06, radial=1, fw=1.3),
 'KI-21 SALLY': A(16.0, 22.5, wp=.33, cr=4.0, ct=1.6, eng='p2', ne=.2, can=.12, radial=1),
 'H8K EMILY': A(28.13, 38.0, wp=.28, cr=5.0, ct=2.0, eng='p4', ne=(.12, .26), can=.12, floats=1, radial=1),
 'MC.202 FOLGORE': F(8.85, 10.58, wp=.32, cr=2.4, ct=1.1),
 'MC.205 VELTRO': F(8.85, 10.58, wp=.32, cr=2.4, ct=1.1),
 'FIAT G.55': F(9.37, 11.85, wp=.32, cr=2.5, ct=1.1),
 'RE.2005': F(8.73, 11.0, wing='e', wp=.32, cr=2.5, ct=1.0),
 'HO 229': A(7.47, 16.76, wing='fw', sw=32, wp=.02, cr=6.5, ct=1.0, eng='j2', tail='n', can=.18),
 'ME 262 HG III': A(10.9, 12.6, wing='sw', sw=45, wp=.30, cr=3.4, ct=1.4, eng='jr2', can=.3),
 'TA 183 HUCKEBEIN': A(9.2, 10.0, wing='sw', sw=40, wp=.30, cr=2.6, ct=1.2, eng='j1', tail='T', tb=3.2, can=.18),
 'LIPPISCH P.13A': A(6.7, 6.0, wing='d', sw=60, wp=.02, cr=6.5, ct=.3, eng='r', tail='n', fin=1, can=.2),
 'BA 349 NATTER': A(6.0, 4.0, wp=.45, cr=1.2, ct=1.2, eng='r', tail='x', can=.2, fw=1.6),
 'AR 234 C': A(12.64, 14.41, wp=.40, cr=2.6, ct=1.4, eng='jw4', ne=(.2, .3), can=.03, tb=4.8),
 'JU 287': A(18.3, 20.11, wing='fs', sw=-25, wp=.36, cr=3.6, ct=1.6, eng='jw4', ne=(.0, .28), can=.04),
 'ME 264 AMERIKA': A(20.9, 43.0, wp=.30, cr=5.0, ct=1.6, eng='p4', tail='tw', tb=8.0, ne=(.13, .27), can=.06),
 'XP-55 ASCENDER': A(9.0, 12.37, wing='sw', sw=25, wp=.50, cr=2.8, ct=1.2, eng='pu', tail='c', can=.32),
 'NORTHROP XP-79': A(4.27, 11.58, wing='fw', sw=35, wp=.02, cr=4.2, ct=1.0, eng='j2', tail='n', can=.15),
 'MIG-21': A(14.1, 7.15, wing='d', sw=57, wp=.45, cr=6.2, ct=.4, eng='j1', tb=3.7, can=.26, probe=1),
 'F-4 PHANTOM': A(19.2, 11.7, wing='sw', sw=45, wp=.38, cr=6.0, ct=1.6, eng='j2', tb=5.5, can=.22, intake=1, dogtooth=1),
 'YAK-38': A(16.37, 7.32, wing='sw', sw=45, wp=.42, cr=3.6, ct=1.4, eng='j1', tb=3.8, can=.18),
 'F-100 SUPER SABRE': A(15.2, 11.81, wing='sw', sw=45, wp=.40, cr=4.6, ct=1.4, eng='j1', tb=5.4, can=.2, nosei=1),
 'F-104 STARFIGHTER': A(16.66, 6.36, wing='t', sw=10, wp=.48, cr=2.9, ct=1.4, eng='j1', tail='T', tb=3.6, can=.22, tip=1),
 'F-105 THUNDERCHIEF': A(19.63, 10.65, wing='sw', sw=45, wp=.44, cr=5.0, ct=1.4, eng='j1', tb=5.2, can=.18),
 'F-111 AARDVARK': A(22.4, 19.2, wing='sw', sw=26, wp=.38, cr=4.4, ct=1.4, eng='j2', tb=8.8, can=.16, sbs=1),
 'A-4 SKYHAWK': A(12.22, 8.38, wing='d', sw=33, wp=.40, cr=4.6, ct=1.0, eng='j1', tb=3.4, can=.22, intake=1),
 'F-8 CRUSADER': A(16.53, 10.87, wing='sw', sw=42, wp=.38, cr=4.4, ct=1.6, eng='j1', tb=5.4, can=.18, nosei=1),
 'MIRAGE IIIC': A(15.03, 8.22, wing='d', sw=60, wp=.45, cr=7.4, ct=.4, eng='j1', tail='n', fin=1, can=.24),
 'AH-1G COBRA': A(13.6, 13.4, eng='h', can=.2, rotor=13.4, stub=3.3),
 'MIG-17 FRESCO': A(11.26, 9.63, wing='sw', sw=45, wp=.40, cr=3.4, ct=1.4, eng='j1', tb=3.1, can=.26, nosei=1),
 'MIG-19 FARMER': A(12.54, 9.0, wing='sw', sw=55, wp=.42, cr=3.6, ct=1.4, eng='j2', tb=3.5, can=.24, nosei=1),
 'MIG-23 FLOGGER': A(16.7, 13.97, wing='sw', sw=45, wp=.40, cr=4.0, ct=1.4, eng='j1', tb=5.6, can=.2, intake=1),
 'MIG-25 FOXBAT': A(19.75, 14.01, wing='sw', sw=40, wp=.48, cr=5.0, ct=1.4, eng='j2', tail='tw', tb=7.6, can=.12, box=1),
 'SU-7 FITTER': A(16.8, 8.93, wing='sw', sw=60, wp=.42, cr=4.4, ct=1.4, eng='j1', tb=4.0, can=.24, nosei=1),
 'MI-24 HIND': A(17.5, 17.3, eng='h', can=.14, rotor=17.3, stub=6.6, fw=1.2),
 'F-35A': A(15.7, 10.7, wing='d', sw=38, wp=.44, cr=6.0, ct=1.6, eng='j1', tail='cv', tb=6.6, can=.2, intake=1),
 'SU-57': A(20.1, 14.1, wing='cd', sw=48, wp=.34, cr=8.0, ct=1.4, eng='j2', tail='cv', tb=7.5, can=.14, box=1),
 'F/A-18E HORNET': A(18.31, 13.62, wing='cd', sw=28, wp=.40, cr=5.0, ct=1.8, eng='j2', tail='cv', tb=6.6, can=.16, lex=1),
 'J-15': A(21.9, 14.7, wing='cd', sw=42, wp=.38, cr=6.6, ct=1.4, eng='j2', tail='tw', tb=9.9, can=.12, canard=1),
 'F-16C VIPER': A(15.06, 9.96, wing='cd', sw=40, wp=.40, cr=5.0, ct=1.2, eng='j1', tb=5.6, can=.16, lex=1),
 'F-15E STRIKE EAGLE': A(19.43, 13.05, wing='sw', sw=45, wp=.40, cr=5.8, ct=1.6, eng='j2', tail='tw', tb=8.6, can=.14, box=1, cft=1),
 'F-15EX EAGLE II': A(19.43, 13.05, wing='sw', sw=45, wp=.40, cr=5.8, ct=1.6, eng='j2', tail='tw', tb=8.6, can=.14, box=1, cft=1),
 'EA-18G GROWLER': A(18.31, 13.62, wing='cd', sw=28, wp=.40, cr=5.0, ct=1.8, eng='j2', tail='cv', tb=6.6, can=.16, lex=1, pods=1),
 'AV-8B HARRIER II': A(14.12, 9.25, wing='sw', sw=37, wp=.40, cr=4.0, ct=1.4, eng='j1v', tb=4.2, can=.18, intake=1),
 'A-10C WARTHOG': A(16.26, 17.53, wp=.42, cr=3.0, ct=1.8, eng='jp2', tail='tw', tb=5.7, can=.12),
 'B-1B LANCER': A(44.5, 24.0, wing='sw', sw=67, wp=.36, cr=10.0, ct=2.0, eng='jb4', tb=13.7, can=.06, blend=1),
 'B-2 SPIRIT': A(21.0, 52.4, wing='fw', sw=33, wp=.0, cr=21.0, ct=1.0, eng='j4', tail='n', can=.12, saw=1),
 'AC-130J GHOSTRIDER': A(29.8, 40.4, wp=.36, cr=4.2, ct=2.6, eng='p4', tb=16.0, ne=(.14, .29), can=.05, fw=1.0),
 'GRIPEN E': A(15.2, 8.6, wing='d', sw=50, wp=.50, cr=5.6, ct=1.0, eng='j1', tail='n', fin=1, can=.2, canard=1),
 'MIRAGE 2000-5': A(14.36, 9.13, wing='d', sw=58, wp=.42, cr=7.6, ct=.5, eng='j1', tail='n', fin=1, can=.22),
 'KF-21 BORAMAE': A(16.9, 11.2, wing='d', sw=40, wp=.42, cr=6.4, ct=1.6, eng='j2', tail='cv', tb=6.4, can=.16),
 'TEJAS MK1A': A(13.2, 8.2, wing='cd', sw=55, wp=.38, cr=7.0, ct=.6, eng='j1', tail='n', fin=1, can=.2),
 'AH-64E APACHE': A(17.7, 14.6, eng='h', can=.14, rotor=14.6, stub=5.2, fw=1.3),
 'SU-30SM': A(21.9, 14.7, wing='cd', sw=42, wp=.38, cr=6.6, ct=1.4, eng='j2', tail='tw', tb=9.9, can=.12, canard=1),
 'SU-34 FULLBACK': A(23.3, 14.7, wing='cd', sw=42, wp=.40, cr=6.6, ct=1.4, eng='j2', tail='tw', tb=9.9, can=.10, canard=1, sbs=1),
 'SU-35 FLANKER-E': A(21.9, 15.3, wing='cd', sw=42, wp=.38, cr=6.6, ct=1.4, eng='j2', tail='tw', tb=9.9, can=.12),
 'MIG-31 FOXHOUND': A(22.69, 13.46, wing='sw', sw=41, wp=.48, cr=5.6, ct=1.4, eng='j2', tail='tw', tb=8.8, can=.12, box=1),
 'TU-160 BLACKJACK': A(54.1, 35.6, wing='sw', sw=65, wp=.36, cr=11.0, ct=2.0, eng='jb4', tb=13.3, can=.05, blend=1),
 'J-10C': A(16.9, 9.75, wing='d', sw=54, wp=.46, cr=6.4, ct=.8, eng='j1', tail='n', fin=1, can=.18, canard=1),
 'J-35': A(17.3, 11.5, wing='d', sw=40, wp=.42, cr=6.4, ct=1.6, eng='j2', tail='cv', tb=6.6, can=.16),
 'JF-17 THUNDER': A(14.93, 9.45, wing='cd', sw=42, wp=.40, cr=4.8, ct=1.2, eng='j1', tb=5.0, can=.18, lex=1),
 'KA-52 ALLIGATOR': A(16.0, 14.5, eng='h', can=.14, rotor=14.5, stub=4.6, coax=1, fw=1.3),
 'SU-25 FROGFOOT': A(15.33, 14.36, wing='sw', sw=20, wp=.42, cr=3.6, ct=1.4, eng='jn2', ne=.08, tb=4.6, can=.16),
 'MIG-29 FULCRUM': A(17.32, 11.36, wing='cd', sw=42, wp=.40, cr=5.6, ct=1.4, eng='j2', tail='tw', tb=7.8, can=.16, lex=1),
 'SU-27 FLANKER': A(21.9, 14.7, wing='cd', sw=42, wp=.38, cr=6.6, ct=1.4, eng='j2', tail='tw', tb=9.9, can=.12),
 'F-15C EAGLE': A(19.43, 13.05, wing='sw', sw=45, wp=.40, cr=5.8, ct=1.6, eng='j2', tail='tw', tb=8.6, can=.14, box=1),
 'SOPWITH CAMEL': A(5.72, 8.53, wing='bi', wp=.22, cr=1.4, ct=1.4, radial=1, can=.36, tb=2.6),
 'FOKKER DR.I': A(5.77, 7.19, wing='tri', wp=.20, cr=1.0, ct=1.0, radial=1, can=.40, tb=2.0),
 'SPAD XIII': A(6.25, 8.25, wing='bi', wp=.20, cr=1.4, ct=1.4, can=.36, tb=2.6),
 'ALBATROS D.III': A(7.33, 9.05, wing='bi', wp=.20, cr=1.6, ct=1.2, can=.38, tb=2.8),
 'SE5A': A(6.38, 8.11, wing='bi', wp=.22, cr=1.5, ct=1.5, can=.38, tb=2.6),
 'FOKKER D.VII': A(6.95, 8.9, wing='bi', wp=.20, cr=1.6, ct=1.3, can=.38, tb=2.6),
 'NIEUPORT 17': A(5.8, 8.16, wing='bi', wp=.20, cr=1.2, ct=1.0, radial=1, can=.38, tb=2.4),
 'GOTHA G.IV': A(12.36, 23.7, wing='bi', wp=.24, cr=2.3, ct=2.3, eng='pu2', ne=.2, can=.12, tb=6.0),
 'HANDLEY PAGE O/400': A(19.16, 30.48, wing='bi', wp=.20, cr=3.0, ct=3.0, eng='p2', ne=.16, can=.08, tail='box', tb=8.0),
}
SPEC['P-39 (VVS)'] = dict(SPEC['P-39 AIRACOBRA'])
def names():
    import re, os
    s = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'src', 'vehdb.inc')).read()
    return {int(nums.split(',')[12]): n for n, nums in re.findall(r'\{"([^"]+)","[^"]*",((?:-?\d+,){15}-?\d+)\}', s)}
def _wing(m, sp, sc, cx, ywr, half, tone=0.58, dz=None, mat=CAMO):
    """one planform (both halves): root LE at ywr, half span `half` px"""
    cr, ct, sw, w = sp['cr'] * sc, sp['ct'] * sc, sp['sw'], sp['wing']
    tan = math.tan(math.radians(sw)); ylt = ywr + half * tan
    out = []
    for s in (-1, 1):
        if w == 'e':                                                                # elliptical (Spitfire, He 111, Tempest)
            pts = []
            for k in range(13):
                t = k / 12; x = cx + s * half * t; c = cr * math.sqrt(max(0.02, 1 - t ** 2))
                pts.append((x, ywr + cr * .25 - c * .25))
            for k in range(12, -1, -1):
                t = k / 12; x = cx + s * half * t; c = cr * math.sqrt(max(0.02, 1 - t ** 2))
                pts.append((x, ywr + cr * .25 + c * .75))
        elif w in ('d', 'fw', 'cd'):
            if w == 'cd':                                                           # cranked delta / LERX
                pts = [(cx, ywr - cr * .28), (cx + s * half * .22, ywr + cr * .18), (cx + s * half, ylt + cr * .1), (cx + s * half, ylt + cr * .1 + ct), (cx + s * half * .3, ywr + cr), (cx, ywr + cr)]
            else:
                pts = [(cx, ywr), (cx + s * half, ylt), (cx + s * half, ylt + ct), (cx, ywr + cr)]
                if w == 'fw' and sp.get('saw'): pts = [(cx, ywr), (cx + s * half, ylt), (cx + s * half, ylt + ct), (cx + s * half * .55, ywr + cr * .62), (cx + s * half * .3, ywr + cr * .8), (cx, ywr + cr * .62)]
        else:                                                                       # taper / swept / gull / forward-swept / biplane
            pts = [(cx, ywr), (cx + s * half, ylt), (cx + s * half, ylt + ct), (cx, ywr + cr)]
            if sp.get('sq'): pts[1] = (cx + s * half * .97, ylt)                    # squared tips
            if sp.get('rt') or w == 'g': pts = [(cx, ywr), (cx + s * half * .9, ylt), (cx + s * half, ylt + ct * .4), (cx + s * half * .92, ylt + ct), (cx, ywr + cr)]
        m.poly(pts, mat, tone, bevel=0.15, dz=dz if dz is not None else H.S * 1.0); out.append(pts)
        if w == 'g':                                                                # gull bend: fold line
            gx = cx + s * half * .32; m.rect(min(gx, gx + s * .6), ywr + .5, max(gx, gx + s * .6), ywr + cr * .9, mat, tone - .18)
        # flap / aileron line one pixel in front of the trailing edge, lit leading edge
        if half > 4 and w not in ('d', 'fw'):
            m.poly([(cx + s * 2, ywr + cr - 1.1 + (half * .0)), (cx + s * half * .9, ylt + ct - 1.0), (cx + s * half * .9, ylt + ct - .55), (cx + s * 2, ywr + cr - .6)], mat, tone - .14)
        m.poly([(cx + s * 1.5, ywr + .1), (cx + s * half * .95, ylt + .1), (cx + s * half * .95, ylt + .7), (cx + s * 1.5, ywr + .7)], mat, tone + .2)
    return ylt
def plane_model(sp, gspan=30, glen=29):
    sc = min(gspan / sp['B'], glen / sp['L']); m = Model(); cx = 16.0
    L = sp['L'] * sc; B = sp['B'] * sc; y0 = 16 - L / 2; y1 = y0 + L
    if sp['eng'] == 'h': return heli_model(sp, sc, m)
    fw = max(2.6, min(7.5, (1.25 if L > 20 else 1.0) * sp['fw'] * L * .11 * 1.4))   # fuselage width
    jet = sp['eng'].startswith('j') or sp['eng'] == 'r'
    ywr = y0 + sp['wp'] * L; half = B / 2
    # wings (biplanes: lower wing slightly behind, upper wing over it)
    if sp['wing'] in ('bi', 'tri'):
        _wing(m, dict(sp, wing='t'), sc, cx, ywr + sp['cr'] * sc * .35, half * .92, tone=.48)
        if sp['wing'] == 'tri': _wing(m, dict(sp, wing='t'), sc, cx, ywr + sp['cr'] * sc * .7, half * .82, tone=.44)
    elif sp['wing'] == 'fs':
        pass
    if sp['wing'] not in ('bi', 'tri'):
        if sp['wing'] == 'fs':
            for s in (-1, 1): m.poly([(cx, ywr + 1.5), (cx + s * half, ywr - half * .45), (cx + s * half, ywr - half * .45 + sp['ct'] * sc), (cx, ywr + sp['cr'] * sc)], CAMO, .58, bevel=.15, dz=H.S)
        else: _wing(m, sp, sc, cx, ywr, half)
    # tailplanes
    tb = sp['tb'] * sc / 2; tcr = max(1.6, sp['cr'] * sc * .45); tail = sp['tail']
    if tail not in ('n', 'c', 'x', 'box'):
        tys = y1 - tcr - (0 if tail != 'boom' else 0.5)
        tsw = math.tan(math.radians(max(0, sp['sw'] - 5)))
        for s in (-1, 1): m.poly([(cx, tys), (cx + s * tb, tys + tb * tsw), (cx + s * tb, tys + tb * tsw + tcr * .6), (cx, tys + tcr)], CAMO, .56, bevel=.15, dz=H.S * .8)
    if tail == 'x' or tail == 'box':
        for s in (-1, 1): m.poly([(cx, y1 - 2.4), (cx + s * max(2.0, tb), y1 - 1.6), (cx + s * max(2.0, tb), y1 - .4), (cx, y1)], CAMO, .56, bevel=.15, dz=H.S * .8)
    if tail == 'c':                                                                     # canard pusher
        for s in (-1, 1): m.poly([(cx, y0 + 2), (cx + s * 2.8, y0 + 3.2), (cx + s * 2.8, y0 + 4.2), (cx, y0 + 4)], CAMO, .56, bevel=.15, dz=H.S * .8)
    if sp.get('canard'):
        for s in (-1, 1): m.poly([(cx + s * fw * .4, ywr - 2.2), (cx + s * (fw * .4 + 2.6), ywr - .9), (cx + s * (fw * .4 + 2.6), ywr - .2), (cx + s * fw * .4, ywr - .6)], CAMO, .62, bevel=.15, dz=H.S * 1.4)
    # engines on the wing
    eng = sp['eng']; ne = sp.get('ne', .25)
    nac = []
    if eng in ('p2', 'jw2', 'jn2', 'pu2', 'jp2'): nac = [ne * B] if not isinstance(ne, tuple) else list(ne)
    if eng in ('p4', 'jw4'): nac = [ne[0] * B, ne[1] * B] if isinstance(ne, tuple) else [B * .16, B * .32]
    if eng == 'jb4': nac = [fw * .5 + 1.2]
    for nx in nac:
        for s in (-1, 1):
            x = cx + s * nx; nw = max(1.8, min(3.6, fw * (.55 if eng.startswith('p') else .5)))
            if eng in ('jp2',):                                                         # A-10 rear pods
                m.rrect(x - nw / 2, y0 + L * .62, x + nw / 2, y0 + L * .82, nw / 2, METAL, .55, bevel=.2, dz=H.S * 2.5)
                m.rect(x - nw / 2 + .3, y0 + L * .62, x + nw / 2 - .3, y0 + L * .62 + .7, DARK, .2); continue
            if eng == 'jb4':
                m.rect(x - 1.6 * (1 if s < 0 else 0) - (0 if s < 0 else -0.0), y0 + L * .58, x + 1.6 * (1 if s > 0 else 0), y0 + L * .80, METAL, .45, bevel=.2, dz=H.S * 2)
                continue
            front = ywr - (2.6 if eng.startswith('p') else 1.6); back = ywr + sp['cr'] * sc * (.95 if eng.startswith('p') else 1.05)
            if eng.startswith('j'): front = ywr - 1.0 - nw * .4
            m.rrect(x - nw / 2, front, x + nw / 2, back, nw / 2, CAMO, .62, bevel=.2, dz=H.S * 2.2)
            if eng.startswith('p'):
                m.rect(x - .5, front - .4, x + .5, front + .5, METAL, .2)                   # spinner
                pr = max(2.2, nw * 1.25)
                fy = round(front - 1.0); m.rect(x - pr, fy, x + pr, fy + 1, GLASS, .5, tag=PROP, dz=0)     # prop disc
            else:
                m.rect(x - nw / 2 + .3, front, x + nw / 2 - .3, front + .7, DARK, .2)       # intake
                m.rect(x - nw / 2 + .4, back - .7, x + nw / 2 - .4, back, DARK, .4)         # nozzle
    # tail booms (P-38, P-61)
    if tail == 'boom':
        bx = nac[0] if nac else B * .25
        for s in (-1, 1):
            m.rrect(cx + s * bx - 1.0, ywr, cx + s * bx + 1.0, y1 - .6, 1.0, CAMO, .62, bevel=.2, dz=H.S * 2.0)
        m.rect(cx - bx, y1 - tcr, cx + bx, y1 - .8, CAMO, .58, bevel=.15, dz=H.S * 2.2)
    # fuselage (cylinder, tapered nose and tail)
    if not (tail == 'boom' and sp.get('pod')) or True:
        fl = L if tail != 'boom' else L * .62
        fy0 = y0; fy1 = y0 + fl
        nose = fl * (.10 if not jet else .18); tailtap = fl * (.30 if not jet else .12)
        pts = [(cx - fw * .25, fy0), (cx + fw * .25, fy0), (cx + fw / 2, fy0 + nose), (cx + fw / 2, fy1 - tailtap), (cx + fw * .18, fy1), (cx - fw * .18, fy1), (cx - fw / 2, fy1 - tailtap), (cx - fw / 2, fy0 + nose)]
        if sp.get('box'): pts = [(cx - fw * .2, fy0), (cx + fw * .2, fy0), (cx + fw * .45, fy0 + nose), (cx + fw * .95, fy0 + fl * .42), (cx + fw * .95, fy1 - .4), (cx - fw * .95, fy1 - .4), (cx - fw * .95, fy0 + fl * .42), (cx - fw * .45, fy0 + nose)]
        if sp.get('blend'): pts = [(cx - fw * .2, fy0), (cx + fw * .2, fy0), (cx + fw * .5, fy0 + nose), (cx + fw * 1.1, fy0 + fl * .45), (cx + fw * .7, fy1 - tailtap), (cx + fw * .2, fy1), (cx - fw * .2, fy1), (cx - fw * .7, fy1 - tailtap), (cx - fw * 1.1, fy0 + fl * .45), (cx - fw * .5, fy0 + nose)]
        if sp['wing'] == 'fw': pts = [(cx - fw * .3, fy0 + 1), (cx + fw * .3, fy0 + 1), (cx + fw * .55, fy0 + fl * .3), (cx + fw * .5, fy0 + fl * .8), (cx - fw * .5, fy0 + fl * .8), (cx - fw * .55, fy0 + fl * .3)]
        m.poly(pts, CAMO, .66, dome=.85, dz=H.S * 3.2)
        if sp.get('lex'):
            for s in (-1, 1): m.poly([(cx + s * fw * .4, fy0 + fl * .2), (cx + s * fw * 1.0, ywr + 1), (cx + s * fw * .5, ywr + 1)], CAMO, .62, bevel=.1, dz=H.S * .6)
        if sp.get('cft'):
            for s in (-1, 1): m.rect(cx + s * fw * .95 - (1.2 if s > 0 else 0), fy0 + fl * .42, cx + s * fw * .95 + (1.2 if s < 0 else 0), fy0 + fl * .72, CAMO, .5, bevel=.2)
        if sp.get('intake'):
            for s in (-1, 1): m.rect(cx + s * fw * .5 - (.9 if s > 0 else 0), fy0 + fl * .26, cx + s * fw * .5 + (.9 if s < 0 else 0), fy0 + fl * .48, CAMO, .5, bevel=.2)
        if sp.get('probe'): m.rect(cx - .3, fy0 - 2, cx + .3, fy0 + .5, METAL, .7)
        if sp.get('radial'): m.ell(cx - fw / 2 - .2, fy0 - .2, cx + fw / 2 + .2, fy0 + 2.2, METAL, .36, bevel=.3, dz=H.S * 3.4)   # radial cowling
        if sp.get('scoop'): m.rrect(cx - fw * .3, fy0 + fl * .5, cx + fw * .3, fy0 + fl * .62, 1, CAMO, .42)                         # belly scoop
        # canopy
        cy = fy0 + sp['can'] * fl; cl = max(2.2, fl * (.16 if not sp.get('bub') else .2)); cw = max(1.4, fw * .55)
        if sp.get('sbs'): cw = max(2.2, fw * .75)
        if sp.get('lc'): cl = fl * .3
        m.ell(cx - cw / 2, cy, cx + cw / 2, cy + cl, DARK, .6, dome=.6, dz=H.S * 1.2)                                # tinted glass
        m.ell(cx - cw / 2 + .1, cy + cl * .12, cx + .1, cy + cl * .12 + max(1.0, cl * .3), GLASS, .5, dz=0)             # sky glint
        if cl > 3.5: m.rect(cx - cw / 2 + .2, round(cy + cl * .62), cx + cw / 2 - .2, round(cy + cl * .62) + 1, METAL, .45, dz=0)   # canopy frame
        # fins (top-down: a thin lit/dark ridge along the tail)
        if tail in ('s', 'boom', 'T') or sp.get('fin'):
            fx = [cx] if tail != 'boom' else [cx - (nac[0] if nac else 4), cx + (nac[0] if nac else 4)]
            for x in fx: m.rect(x - .45, y1 - max(3.0, tcr * 1.4), x + .45, y1 - .2, CAMO, .82, dz=H.S * 2)
        if tail in ('tw', 'cv'):
            off = fw * (.75 if tail == 'tw' else .6)
            for s in (-1, 1): m.rect(cx + s * off - .45, y1 - max(3.2, tcr * 1.5), cx + s * off + .45, y1 - .4, CAMO, .82, dz=H.S * 2)
        if tail == 'x': m.rect(cx - .45, y1 - 3, cx + .45, y1, CAMO, .82, dz=H.S * 2)
        # engines in the fuselage
        if eng in ('p1', 'pp'):
            pr = max(3.0, min(half * .8, fw * 1.3))
            m.rect(cx - .6, fy0 - .6, cx + .6, fy0 + .5, METAL, .2)
            fy = round(fy0 - 1.2); m.rect(cx - pr, fy, cx + pr, fy + 1, GLASS, .5, tag=PROP, dz=0)
        if eng in ('pp', 'pu', 'pu2'):
            pr = max(3.0, fw * 1.2); m.rect(cx - pr, fy1 + .1, cx + pr, fy1 + .9, GLASS, .5, tag=PROP, dz=0)
        if eng in ('j1', 'j1v', 'j2', 'jr2', 'r', 'jd', 'j4'):
            n = 2 if eng in ('j2', 'jr2') else 1
            for k in range(n):
                ex = cx + (k - (n - 1) / 2) * fw * .42; ew = max(1.2, fw * (.34 if n == 2 else .45))
                m.rect(ex - ew / 2, fy1 - .9, ex + ew / 2, fy1 + .1, GOLD if eng != 'r' else DARK, .5, dz=0)      # nozzle glow
        if eng == 'jd': m.rrect(cx - fw * .4, fy0 + fl * .35, cx + fw * .4, fy0 + fl * .75, 1, METAL, .55, bevel=.2, dz=H.S * 4)   # He 162 dorsal jet
        if eng == 'j1v':
            for s in (-1, 1): m.rect(cx + s * fw * .5 - (.8 if s > 0 else 0), ywr + 1, cx + s * fw * .5 + (.8 if s < 0 else 0), ywr + 3, DARK, .3)
    if sp.get('tip'):
        for s in (-1, 1): m.rrect(cx + s * half - .9, ywr + (half * math.tan(math.radians(sp['sw']))) - 1.4, cx + s * half + .9, ywr + half * math.tan(math.radians(sp['sw'])) + sp['ct'] * sc + .4, .9, CAMO, .62, bevel=.2, dz=H.S * 2)
    if sp.get('floats'):
        for s in (-1, 1): m.rect(cx + s * half * .9 - .6, ywr + .4, cx + s * half * .9 + .6, ywr + 2.2, CAMO, .6, bevel=.2, dz=H.S * 1.6)
    if sp.get('pods') or sp.get('cannon'):
        for s in (-1, 1): m.rect(cx + s * half * .45 - .5, ywr - 2.2, cx + s * half * .45 + .5, ywr + 1.6, METAL, .6, dz=H.S)
    # national marking (generic disc, team colour) on each wing
    if half > 5:
        for s in (-1, 1):
            mx = cx + s * half * .62; my = ywr + half * .62 * math.tan(math.radians(sp['sw'] if sp['wing'] != 'fs' else -25)) + sp['cr'] * sc * .38
            m.rect(mx - .6, my - .5, mx + .6, my + .6, TEAM, .5, dz=0)
    return m
PROP, ROTOR = 3, 4
def heli_model(sp, sc, m):
    cx = 16.0; L = sp['L'] * sc; y0 = 16 - L * .42; y1 = y0 + L; fw = max(3.4, sp['fw'] * L * .14)
    sl = sp.get('stub', 0) * sc / 2
    if sl: m.rect(cx - sl, y0 + L * .34, cx + sl, y0 + L * .42, CAMO, .55, bevel=.2, dz=H.S * 1.2)
    for s in (-1, 1):
        if sl: m.rect(cx + s * sl * .8 - .7, y0 + L * .30, cx + s * sl * .8 + .7, y0 + L * .48, METAL, .5, bevel=.2, dz=H.S * 1.6)   # rocket pods
    m.poly([(cx - fw * .3, y0), (cx + fw * .3, y0), (cx + fw / 2, y0 + L * .15), (cx + fw / 2, y0 + L * .5), (cx + fw * .22, y0 + L * .62), (cx - fw * .22, y0 + L * .62), (cx - fw / 2, y0 + L * .5), (cx - fw / 2, y0 + L * .15)], CAMO, .64, dome=.8, dz=H.S * 3)
    m.rect(cx - .7, y0 + L * .6, cx + .7, y1 - 1.0, CAMO, .6, dome=.5, dz=H.S * 2)                        # tail boom
    m.rect(cx - 2.2, y1 - 2.4, cx + 2.2, y1 - 1.4, CAMO, .55, bevel=.2, dz=H.S * 1.4)                    # stabiliser
    m.rect(cx + .7, y1 - 2.6, cx + 1.1, y1 - .2, GLASS, .5, tag=PROP, dz=0)                              # tail rotor
    m.ell(cx - fw * .3, y0 + .5, cx + fw * .3, y0 + L * (.14 if not sp.get('coax') else .2), DARK, .6, dome=.6, dz=H.S * 1.2)
    m.ell(cx - fw * .3, y0 + L * .16, cx + fw * .3, y0 + L * .3, DARK, .6, dome=.6, dz=H.S * 1.2)
    m.rect(cx - fw * .3 + .3, y0 + 1.0, cx - fw * .3 + 1.3, y0 + 2.0, GLASS, .5, dz=0)
    m.ell(cx - 1.0, y0 + L * .36, cx + 1.0, y0 + L * .44, METAL, .55, bevel=.3, dz=H.S * 3)             # rotor head
    r = sp['rotor'] * sc / 2; ry = y0 + L * .40
    for a in ((0, 1), (1, 0)) if not sp.get('coax') else ((0, 1), (1, 0), (.7, .7), (.7, -.7)):
        dx, dy = a[0] * r, a[1] * r
        m.poly([(cx - dx - dy * .05, ry - dy + dx * .05), (cx + dx - dy * .05, ry + dy + dx * .05), (cx + dx + dy * .05, ry + dy - dx * .05), (cx - dx + dy * .05, ry - dy - dx * .05)], METAL, .3, dz=H.S * 4, tag=ROTOR)
    return m
def _post_prop(g, tg):
    """propeller discs: alternating blur / clear pixels so the terrain shows through"""
    for y in range(32):
        for x in range(32):
            if tg[y, x] == PROP: g[y, x] = 11 if (x + y) % 2 == 0 else (13 if (x + y) % 4 == 1 else 0)
            elif tg[y, x] == ROTOR and g[y, x] in (12, 13, 14, 11): g[y, x] = 13 if (x + y) % 3 else 0
def plane(sp, gspan=30, glen=29):
    m = plane_model(sp, gspan, glen)
    saved = H._post
    try:
        def post(g, tg): saved(g, tg); _post_prop(g, tg)
        H._post = post; return m.reduce()
    finally: H._post = saved
def _gba_dims(g):
    ys, xs = np.nonzero(g > 1)
    if not len(ys): return 28, 28
    return int(xs.max() - xs.min() + 1), int(ys.max() - ys.min() + 1)
def patch_planes(words, nm):
    import veh_ds
    out = list(words)
    for i in range(158):
        g = veh_ds.grids(words, 1, 128, i * 128)[0]; sp = SPEC.get(nm.get(i))
        if sp is None: out[i * 128:i * 128 + 128] = veh_ds.pack(veh_ds.plane(g, i)); continue
        gs, gl = _gba_dims(g)
        out[i * 128:i * 128 + 128] = veh_ds.pack(plane(sp, min(30, gs), min(30, gl)))
    return out
