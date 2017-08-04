#ifndef _ssummary_H
#define _ssummary_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "BOBHASH32.h"
#include "params.h"
#define len2 9973
#define rep(i,a,n) for(int i=a;i<=n;i++)
using namespace std;
class ssummary
{
    public:
        int tot;
        int sum[M+10],K,last[M+10],Next[M+10],ID[M+10];
        int head[N+10],Left[N+10],Right[N+10],num;
        string str[M+10];
        int head2[len2+10],Next2[M+10];
        BOBHash32 * bobhash;

        ssummary(int K):K(K) {bobhash=new BOBHash32(1000);}
        void clear()
        {
            memset(sum,0,sizeof(sum));
            memset(last,0,sizeof(Next));
            memset(Next2,0,sizeof(Next2));
            rep(i,0,N)head[i]=Left[i]=Right[i]=0;
            rep(i,0,len2-1)head2[0]=0;
            tot=0;
            rep(i,1,M+2)ID[i]=i;
            num=M+2;
            Right[0]=N;
            Left[N]=0;
        }
        int getid()
        {
            int i=ID[num--];
            last[i]=Next[i]=sum[i]=Next2[i]=0;
            return i;
        }
        int location(string ST)
        {
            return (bobhash->run(ST.c_str(),ST.size()))%len2;
        }
        void add2(int x,int y)
        {
            Next2[y]=head2[x];
            head2[x]=y;
        }
        int find(string s)
        {
            for(int i=head2[location(s)];i;i=Next2[i])
              if(str[i]==s)return i;
            return 0;
        }
        void linkhead(int i,int j)
        {
            Left[i]=j;
            Right[i]=Right[j];
            Right[j]=i;
            Left[Right[i]]=i;
        }
        void cuthead(int i)
        {
            int t1=Left[i],t2=Right[i];
            Right[t1]=t2;
            Left[t2]=t1;
        }
        int getmin()
        {
            if (tot<K) return 0;
            if(Right[0]==N)return 1;
            return Right[0];
        }
        void link(int i,int ww)
        {
            ++tot;
            bool flag=(head[sum[i]]==0);
            Next[i]=head[sum[i]];
            if(Next[i])last[Next[i]]=i;
            last[i]=0;
            head[sum[i]]=i;
            if(flag)
            {
                for(int j=sum[i]-1;j>0 && j>sum[i]-10;j--)
                if(head[j]){linkhead(sum[i],j);return;}
                linkhead(sum[i],ww);
            }
        }
        void cut(int i)
        {
            --tot;
            if(head[sum[i]]==i)head[sum[i]]=Next[i];
            if(head[sum[i]]==0)cuthead(sum[i]);
            int t1=last[i],t2=Next[i];
            if(t1)Next[t1]=t2;
            if(t2)last[t2]=t1;
        }
        void recycling(int i)
        {
            int w=location(str[i]);
            if (head2[w]==i)
              head2[w]=Next2[i];
              else
              {
                  for(int j=head2[w];j;j=Next2[j])
                  if(Next2[j]==i)
                  {
                        Next2[j]=Next2[i];
                        break;
                  }
              }
            ID[++num]=i;
        }
};
#endif
