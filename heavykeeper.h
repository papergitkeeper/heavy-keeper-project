#ifndef _heavykeeper_H
#define _heavykeeper_H

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
#define HK_d 2
#define HK_b 1.08
#define rep(i,a,n) for(int i=a;i<=n;i++)
using namespace std;
class heavykeeper
{
    private:
        ssummary *ss;
        struct node {int C,FP;} HK[HK_d][MAX_MEM+10];
        BOBHash64 * bobhash;
        int K,M2;
    public:
        heavykeeper(int M2,int K):M2(M2),K(K) {ss=new ssummary(K); ss->clear(); bobhash=new BOBHash64(1005);}
        void clear()
        {
            for (int i=0; i<HK_d; i++)
                for (int j=0; j<=M2+5; j++) HK[i][j].C=HK[i][j].FP=0;
        }
        unsigned long long Hash(string ST)
        {
            return (bobhash->run(ST.c_str(),ST.size()));
        }
        void Insert(string x)
        {
            bool mon=false;
            int p=ss->find(x);
            if (p) mon=true;
            int maxv=0;
            unsigned long long H=Hash(x); int FP=(H>>48);
            for (int j=0; j<HK_d; j++)
            {
                int Hsh=H%(M2-(2*HK_d)+2*j+3);
                int c=HK[j][Hsh].C;
                if (HK[j][Hsh].FP==FP)
                {
                    if (mon || c<=ss->getmin())
                      HK[j][Hsh].C++;
                    maxv=max(maxv,HK[j][Hsh].C);
                } else
                {
                    if (!(rand()%int(pow(HK_b,HK[j][Hsh].C))))
                    {
                        HK[j][Hsh].C--;
                        if (HK[j][Hsh].C<=0)
                        {
                            HK[j][Hsh].FP=FP;
                            HK[j][Hsh].C=1;
                            maxv=max(maxv,1);
                        }
                    }
                }
            }
            if (!mon)
            {
                if (maxv-(ss->getmin())==1 || ss->tot<K)
                {
                    int i=ss->getid();
                    ss->add2(ss->location(x),i);
                    ss->str[i]=x;
                    ss->sum[i]=maxv;
                    ss->link(i,0);
                    while(ss->tot>K)
                    {
                        int t=ss->Right[0];
                        int tmp=ss->head[t];
                        ss->cut(ss->head[t]);
                        ss->recycling(tmp);
                    }
                }
            } else
            if (maxv>ss->sum[p])
            {
                int tmp=ss->Left[ss->sum[p]];
                ss->cut(p);
                if(ss->head[ss->sum[p]]) tmp=ss->sum[p];
                ss->sum[p]=maxv;
                ss->link(p,tmp);
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
