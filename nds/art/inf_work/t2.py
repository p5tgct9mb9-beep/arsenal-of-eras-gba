from weap import *
from gbalib import up
im=Image.new('RGB',(240,160),(88,96,60))
names=['THOMPSON','M1911','GARAND','BAR','BAZOOKA','BINO']
for i,n in enumerate(names): draw_item(im,ITEMS[n],(2+(i%2)*118,2+(i//2)*30,2+(i%2)*118+114,2+(i//2)*30+28))
draw_item(im,lambda:m4(('RDS','SUP','GRIP','LASER','EXT')),(2,92,160,130))
for i,n in enumerate(['RDS','X4','SNIPE','SUP','GRIP','LASER','BIPOD','EXT']): draw_item(im,ITEMS[n],(2+i*29,132,29+i*29,158))
up(im,4).save('t2.png')
