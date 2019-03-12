#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <map>
#include <fstream>
#include "BOBHASH32.h"
#include "params.h"
#include "Counter_Tree.h"
#include <ctime>
using namespace std;
map <string ,int> B,C,Rank;
struct node {string x;int y;} p[10000005];
ifstream fin("u1",ios::in|ios::binary);
char a[105];
string Read()
{
    fin.read(a,13);
    a[13]='\0';
    string tmp=a;
    return tmp;
}
string s[10000005];
int cmp(node i,node j) {return i.y>j.y;}
int main(int argv, char **argc)
{
    srand((unsigned)time(NULL));
    //freopen("D:\\pkuѧϰ\\sliding\\stack-new.txt","r",stdin);
    int MEM,K;
    //sscanf(argc[1],"%d",&MEM);
    //sscanf(argc[2],"%d",&K);
    MEM=50; K=100;
    //fin.close();
    //fin.open(argc[3],ios::in|ios::binary);
    //char f[1000];
    //sprintf(f,"Other_MEM=%d_K=%d_%c%c.txt",MEM,K,argc[3][1],argc[3][2]);
    //freopen(f,"w",stdout);
   // K=100;
    int m=2000000;  // the number of flows in stream
    // prepare for sketch plus heap
    Counter_Tree *SH;
    // Insertion
    for (int i=1; i<=m; i++)
    {
		char aa[105];
		//scanf("%s",aa);
		//s[i]=aa;
		//scanf("%s",aa);
        s[i]=Read();
        B[s[i]]++;
    }

    cout<<"preparing true flow"<<endl;
	// prepare for real top k
	int cnt=0;
    for (map <string,int>::iterator sit=B.begin(); sit!=B.end(); sit++)
    {
        p[++cnt].x=sit->first;
        p[cnt].y=sit->second;
    }
    sort(p+1,p+cnt+1,cmp);
    for (int i=1; i<=K; i++) {Rank[p[i].x]=i; C[p[i].x]=p[i].y;}


    int sh_M;

    {
       // for (sh_M=1; 432*K+sh_M*4*16<=MEM*1024*8; sh_M++);
        SH=new Counter_Tree(MEM,K);
        cout<<sh_M<<endl;
        for (int i=1; i<=m; i++)
        {
            if (i%100000==0) cout<<i<<endl;
            SH->Insert(s[i]);
        }
        int SH_sum=0,SH_AAE=0; double SH_ARE=0;
        string SH_string; int SH_num; double MAP=0;
        for (int i=0; i<K; i++)
        {
            SH_string=(SH->Query(i)).first; SH_num=(SH->Query(i)).second;
            SH_AAE+=abs(B[SH_string]-SH_num); SH_ARE+=abs(B[SH_string]-SH_num)/(B[SH_string]+0.0);
            if (C[SH_string]) SH_sum++;// else cout<<B[SH_string]<<' '<<SH_num<<endl;

            int summ=0;
            for (int j=0; j<=i; j++)
                if (Rank[SH_string]>0 && Rank[SH_string]<=i+1) summ++;
            MAP+=(double)summ/(i+1);
        }
        MAP/=K;

        printf("%dKB   top %d:\nAccepted: %d/%d  %.10f\nARE: %.10f\nAAE: %.10f\nMAP: %.10f\n\n",MEM,K,SH_sum,K,(SH_sum/(K+0.0)),SH_ARE/K,SH_AAE/(K+0.0),MAP);
    }
    return 0;
}
