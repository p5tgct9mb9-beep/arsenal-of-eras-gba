from m1hd import *
def felt(W,H):
    im=Image.new('RGB',(W,H)); P=im.load()
    FR=[(26,24,18),(34,32,24),(42,40,30),(50,48,36),(58,56,42)]
    for y in range(H):
        for x in range(W):
            t=0.75-0.5*abs(y/H-0.45)+0.12*(noise(x*0.8,y*0.8,3)-0.5)
            P[x,y]=dith(t,x,y,FR)
    return im
def draw(im,fn,box_,align='c'):
    P,L,D=fn(); T=im.crop(box_); xf,mask=fit_mask(T,P,L,D,(1,1,T.width-1,T.height-1),align=align)
    im.paste(T,box_[:2]); return Xf(xf.ox+box_[0],xf.oy+box_[1],xf.sc)
if __name__=='__main__':
    im=felt(256,80); xf=draw(im,garand,(6,8,250,72)); up(im,4).save('show/m1_t5.png'); print(xf.sc)
    im=felt(256,60)
    for i,f in enumerate([helmet,belt,grenade,canteen,knife_r]): draw(im,f,(4+i*50,4,4+i*50+46,56))
    up(im,4).save('show/kit_t3.png')
    im=felt(256,30)
    for i,f in enumerate([bayonet,m7gl,m84]): draw(im,f,(4+i*84,4,4+i*84+78,26))
    up(im,4).save('show/att_t3.png')
