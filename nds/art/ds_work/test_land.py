from land_ds import *
fs=[('SHERMAN',sherman),('T34',t34),('TIGER',tiger),('PZ4',pz4),('JEEP',jeep),('M60',m60),('T72',t72),('M113',m113),('M1A2',m1a2),('LEO',leo2a6),('CH2',chally2),('T90',t90),('BRAD',bradley),('HIMARS',himars)]
items=[dict(name=n,era='',country='',fact='TEST',fn=f) for n,f in fs]
sheet('TEST',[('A','',items[:5]),('B','',items[5:10]),('C','',items[10:15])],'/workspace/ds_work/t_ld1.png',land=True)
