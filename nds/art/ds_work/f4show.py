from f4hd import *
SKY=[(52,86,138),(70,110,164),(92,136,186),(120,160,204),(150,186,220),(180,206,230)]
def sky(W,H,seed=3):
    im=Image.new('RGB',(W,H)); P=im.load()
    for y in range(H):
        for x in range(W):
            t=1-y/H*0.95+0.06*(noise(x*0.6,y*1.6,9)-0.5)
            c=dith(t,x,y,SKY)
            n=noise(x*0.5+200,y*2.2,14)
            if n>0.72 and y>H*0.45: c=dith(min(1,(n-0.72)*5),x,y,[c,(206,220,234),(232,240,246)])
            P[x,y]=c
    return im
def draw(im,fn,marks,box_):
    P,L,D=fn(); T=im.crop(box_); xf,mask=fit_mask(T,P,L,D,(1,1,T.width-1,T.height-1))
    marks(T,xf) if marks else None; im.paste(T,box_[:2]); return xf
if __name__=='__main__':
    im=sky(256,104); draw(im,side,side_marks,(8,6,250,100)); up(im,4).save('show/f4_side_t4.png')
    im=Image.new('RGB',(240,110),(30,40,50)); draw(im,plan,plan_marks,(4,4,140,106)); draw(im,head,None,(144,20,238,90)); up(im,4).save('show/f4_ph_t4.png')
