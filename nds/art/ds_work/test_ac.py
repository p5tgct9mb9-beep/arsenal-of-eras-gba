from ac_ds import *
import sys
items=[dict(name=n,era='',country='',fact='TEST',fn=f) for n,f in [('P-51D',p51d),('SPIT',spit9),('BF',bf109),('ZERO',zero),('B17',b17g),('F86',f86),('MIG21',mig21),('F4',f4),('SR71',sr71),('F22',f22),('F35',f35a),('F16',f16c),('SU57',su57),('A10',a10c),('AH64',ah64e),('MQ9',mq9)]]
sheet('TEST',[('A','',items[:5]),('B','',items[5:10]),('C','',items[10:15]),('D','',items[15:])],'/workspace/ds_work/t_ac1.png')
