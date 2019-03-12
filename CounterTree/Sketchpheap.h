#ifndef _Sketchpheap_H
#define _Sketchpheap_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "params.h"
#include "BOBHASH32.h"
#define Total 10000005
#define Count 4
using namespace std;
class Sketchpheap
{
private:
    BOBHash32 * bobhash[Count+2];
    struct node {int val,idx; string ID;} Heap[Total];
    int tot[Count][MAX_MEM],head[N],next[Total],m,k,WZ,Q,p,cnt,o,MIN,M2,K,NUM;
    struct Node {int wz; string ID;} ID_index[Total];

public:
    Sketchpheap(int NUM,int K):NUM(NUM),K(K)
    {
        M2=1000000;
        for (int i=0; i<=Count; i++) bobhash[i]=new BOBHash32(i+1000);
    }
    int Find(int x,string y)
    {
        int now=head[x];
        while (ID_index[now].ID!=y && now) now=next[now];
        if (ID_index[now].ID==y) return ID_index[now].wz;
        return -1;
    }
    void Delete(int x,string y)
    {
        if (ID_index[head[x]].ID==y) {head[x]=next[head[x]]; return;}
        int now=head[x],Last;
        while (ID_index[now].ID!=y && now) {Last=now; now=next[now];}
        if (!head[x]) return;
        next[Last]=next[next[Last]];
    }
    void Change(int x,int y)
    {
        swap(ID_index[Heap[x].idx].wz,ID_index[Heap[y].idx].wz);
        swap(Heap[x].val,Heap[y].val);
        swap(Heap[x].idx,Heap[y].idx);
        swap(Heap[x].ID,Heap[y].ID);
    }
    void Insert(string A)
    {
        MIN=1000000000;
        for (int i=0; i<Count; i++)
        {
            WZ=(bobhash[i]->run(A.c_str(),A.size()))%NUM;
            tot[i][WZ]++;
            MIN=min(MIN,tot[i][WZ]);
        }
        Q=(bobhash[Count]->run(A.c_str(),A.size()))%M2;
        p=Find(Q,A);
        if (p==-1)
        {
            Heap[++cnt].val=MIN; Heap[cnt].ID=A;
            o++; ID_index[o].ID=A; ID_index[o].wz=cnt; next[o]=head[Q]; head[Q]=o;
            Heap[cnt].idx=o;
            int now=cnt;
            while (now>1 && Heap[now].val<Heap[now/2].val) {Change(now,now/2); now/=2;}
            if (cnt>K)
            {
                Change(1,cnt); Delete((bobhash[Count]->run(Heap[cnt].ID.c_str(),Heap[cnt].ID.size()))%M2,Heap[cnt].ID);
                int now=1; cnt--;
                while (now*2<=cnt && Heap[now].val>Heap[now*2].val || now*2+1<=cnt && Heap[now].val>Heap[now*2+1].val)
                {
                    if (Heap[now*2].val<Heap[now*2+1].val || now*2==cnt) {Change(now,now*2); now*=2;} else
                    {
                        Change(now,now*2+1);
                        now=now*2+1;
                    }
                }
            }
        } else
        {
            Heap[p].val=MIN;
            int now=p;
            while (now>1 && Heap[now].val<Heap[now/2].val) {Change(now,now/2); now/=2;}
            while (now*2<=cnt && Heap[now].val>Heap[now*2].val || now*2+1<=cnt && Heap[now].val>Heap[now*2+1].val)
            {
                if (Heap[now*2].val<Heap[now*2+1].val || now*2==cnt) {Change(now,now*2); now*=2;} else
                {
                    Change(now,now*2+1);
                    now=now*2+1;
                }
            }
        }
    }
    pair<string,int> Query(int k)
    {
        //printf("%s %s\n",Heap[1].ID,Heap[2].ID);
        return make_pair(Heap[k+1].ID,Heap[k+1].val);
    }
};
#endif
