#ifndef _CSS_H
#define _CSS_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "BOBHASH32.h"
#include "params.h"
#include "BOBHASH64.h"
#define rep(i,a,n) for(int i=a;i<=n;i++)
using namespace std;
class CSS
{
    private:
        int head[M+10],Next[M+10],M2,K;
        struct node {int wz; string ID;} ID_index[M+10];
        int R[MAX_MEM+10],Counter_Array[MAX_MEM+10],m,Last;
        int Value_Index[MAX_MEM+10];
        BOBHash32 * bobhash;
    public:
        CSS(int M2,int K):M2(M2),K(K) {bobhash=new BOBHash32(1001);}
        void clear()
        {
            Value_Index[0]=M2-1; ID_index[0].wz=-1;
            for (int i=0; i<M2; i++) R[i]=i,head[i]=i+1,ID_index[i+1].wz=i;
        }
        int Hash(string ST)
        {
            return (bobhash->run(ST.c_str(),ST.size()))%M2;
        }
        int Find(string x)
        {
            int X=Hash(x);
            int now=head[X];
            while (now && ID_index[now].ID!=x) now=Next[now];
            if (!now) return -1; else
            return ID_index[now].wz;
        }
        void Change(int x,int y,int z)
        {
            int now=head[x];
            while (now && ID_index[now].wz!=y) now=Next[now];
            ID_index[now].wz=z;
        }
        void Change(int x,int y,int z,string a)
        {
            int now=head[x];
            while (now && (ID_index[now].wz!=y || ID_index[now].ID!=a)) now=Next[now];
            ID_index[now].wz=z;
        }
        void Insert_Hash(int x,string y,int z)
        {
            ID_index[Last].ID=y; ID_index[Last].wz=z;
            Next[Last]=head[x];
            head[x]=Last;
        }
        void Delete(int x,int y)
        {
            if (ID_index[head[x]].wz==y) {Last=head[x]; head[x]=Next[head[x]]; return;}
            int now=head[x];
            while (now && ID_index[Next[now]].wz!=y) now=Next[now];
            Last=Next[now];
            Next[now]=Next[Next[now]];
        }
        void Insert(string x)
        {
            int p=Find(x);
            if (p!=-1)
            {
                int Q=Counter_Array[p]+1;
                int z=Value_Index[Counter_Array[p]];
                if (z!=p)
                {
                    Change(R[z],z,p);
                    R[p]=R[z];
                    Value_Index[Counter_Array[p]]=z-1;
                }
                 else
                   if (z && Counter_Array[z-1]==Counter_Array[z]) Value_Index[Counter_Array[z]]=z-1; else Value_Index[Counter_Array[z]]=0;
                Value_Index[Q]=max(Value_Index[Q],z);
                R[z]=Hash(x);
                Counter_Array[z]=Q;
                Change(R[z],p,z,x);
            }
            else
            {
                Delete(R[0],0); int Q=Counter_Array[0]+1;
                int z=Value_Index[Counter_Array[0]];
                if (z)
                {
                    Change(R[z],z,0);
                    R[0]=R[z];
                    Value_Index[Counter_Array[0]]=z-1;
                } else
                  Value_Index[Counter_Array[0]]=0;
                Value_Index[Q]=max(Value_Index[Q],z);
                R[z]=Hash(x);
                Counter_Array[z]=Q;
                Insert_Hash(R[z],x,z);
            }
        }
        struct Node {string x; int y;} q[MAX_MEM+10];
        static int cmp(Node i,Node j) {return i.y>j.y;}
        void work()
        {
           int CNT=0;
            for (int i=M2-1; i>=M2-K; i--)
            {
                int now=head[R[i]];
                while (1) {if (ID_index[now].wz==i) {q[CNT].x=ID_index[now].ID;q[CNT].y=Counter_Array[i]; CNT++; break;} now=Next[now]; }
            }
           sort(q,q+K,cmp);
        }
        pair<string ,int> Query(int k)
        {
            return make_pair(q[k].x,q[k].y);
        }
};
#endif
