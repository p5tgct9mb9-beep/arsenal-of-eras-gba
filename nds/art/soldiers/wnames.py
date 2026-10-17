"""Parse weapon tables (PRIM/SEC/LAUN per era) from src/loadout.inc for the art generators."""
import re,os
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
def tables():
    s=open(os.path.join(ROOT,'src/loadout.inc')).read(); sn=open(os.path.join(ROOT,'src/sniperdb.inc')).read()
    mac=dict(re.findall(r'#define (SNP_E\d) ?(.*)',sn))
    out={}
    for arr in ('PRIM','SEC','LAUN'):
        i=s.index('static const Wpn %s[NERA]'%arr); i=s.index('={',i)+1; d=0; j=i
        while True:
            c=s[j]
            if c=='{': d+=1
            elif c=='}':
                d-=1
                if d==0: break
            j+=1
        body=s[i+1:j]
        for k,v in mac.items(): body=body.replace(k,v)
        eras=[];d=0;cur=''
        for c in body:
            if c=='{':
                d+=1
                if d==1: cur=''; continue
            if c=='}':
                d-=1
                if d==0: eras.append(re.findall(r'\{"([^"]+)",([^}]*)\}',cur)); continue
            if d>=1: cur+=c
        out[arr]=[[ (n,[int(x) for x in st.split(',')]) for n,st in e] for e in eras]
    return out
if __name__=='__main__':
    t=tables()
    for k,v in t.items():
        for e,l in enumerate(v): print(k,e,len(l),[n for n,_ in l])
