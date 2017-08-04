#ifndef _LoosyCounting_H
#define _LossyCounting_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "BOBHASH32.h"
#include "params.h"
#include "ssummary.h"
#include "BOBHASH64.h"
#define rep(i,a,n) for(int i=a;i<=n;i++)
using namespace std;
class LossyCounting
{
    private:
        ssummary *ss;
        int K;
    public:
        LossyCounting(int K):K(K) {ss=new ssummary(0); ss->clear();}
        void Insert(string x,int c)
        {
            bool mon=false;
            int p=ss->find(x);
            if (p) mon=true;
            if (!mon)
            {
                int q=c+1;
                int i=ss->getid();
                ss->add2(ss->location(x),i);
                ss->str[i]=x;
                ss->sum[i]=q;
                ss->link(i,0);
            } else
            {
                int tmp=ss->Left[ss->sum[p]];
                ss->cut(p);
                if(ss->head[ss->sum[p]])tmp=ss->sum[p];
                ss->sum[p]++;
                ss->link(p,tmp);
            }
        }
        void clear(int c)
        {
            while (ss->getmin()<c)
            {
                int t=ss->Right[0];
                int tmp=ss->head[t];
                ss->cut(ss->head[t]);
                ss->recycling(tmp);
            }
        }
        struct Node {string x; int y;} q[MAX_MEM+10];
        static int cmp(Node i,Node j) {return i.y>j.y;}
        void work()
        {
            int CNT=0;
            for(int i=N;i;i=ss->Left[i])
                for(int j=ss->head[i];j;j=ss->Next[j]) {q[CNT].x=ss->str[j]; q[CNT].y=ss->sum[j]; CNT++; }
            sort(q,q+CNT,cmp);
        }
        pair<string ,int> Query(int k)
        {
            return make_pair(q[k].x,q[k].y);
        }
};
#endif
