#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <map>
#include <algorithm>

#include "ElasticSketch.h"
using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 10


struct FIVE_TUPLE{	char key[13];	};
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[END_FILE_NO - START_FILE_NO + 1];

void ReadInTraces(const char *trace_prefix)
{
	int datafileCnt = 1;
//	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		char datafileName[100];
		sprintf(datafileName, "%s", trace_prefix);
		FILE *fin = fopen(datafileName, "rb");

		FIVE_TUPLE tmp_five_tuple;
		traces[datafileCnt - 1].clear();
		int tot_num = 0;
		while(fread(&tmp_five_tuple, 1, 13, fin) == 13 && tot_num < 10000000)
		{
			traces[datafileCnt - 1].push_back(tmp_five_tuple);
			tot_num ++;
		}
		fclose(fin);

		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
	printf("\n");
}

      map <string,int> mp;
struct node {string x;int y;} tt[10000005];
int cmp(node i,node j) {return i.y>j.y;}

	int CalculateFP(uint8_t *key, uint32_t &fp)
	{
		fp = *((uint32_t*)key);
	//	return CalculateBucketPos(fp) % bucket_num;
	}
	
	int insert2(uint8_t *key, uint8_t *swap_key, uint32_t &swap_val, uint32_t f = 1)
	{
		uint32_t fp;
		int pos = CalculateFP(key, fp);
		uint32_t key2 = fp;
		
		mp[string((const char*)&key2, 4)] ++;
	}

 void insert(uint8_t *key, int f = 1)
    {
        uint8_t swap_key[KEY_LENGTH_4];
        uint32_t swap_val = 0;
        int result = insert2(key, swap_key, swap_val, f);
    }
    
    
int main()
{
	ReadInTraces("u1");


#define HEAVY_MEM (int(50 * 1024 / 4))
#define BUCKET_NUM (HEAVY_MEM / 64)
#define TOT_MEM_IN_BYTES (50 * 1024)
	ElasticSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> *elastic = NULL;



//	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	int datafileCnt = 1;
	{
		elastic = NULL;

    timespec time1, time2;
		long long resns;
		int packet_cnt = (int)traces[datafileCnt - 1].size();
    
    uint8_t **keys = new uint8_t*[(int)traces[datafileCnt - 1].size()];
		for(int i = 0; i < (int)traces[datafileCnt - 1].size(); ++i)
		{
			keys[i] = new uint8_t[13];
			memcpy(keys[i], traces[datafileCnt - 1][i].key, 13);
		}
    
    clock_gettime(CLOCK_MONOTONIC, &time1);
    int test_cycles = 1;
    for(int t = 0; t < test_cycles; ++t)
    {
      elastic = new ElasticSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();
      for(int i = 0; i < packet_cnt; ++i)
      {
      	  if (i%1000000==0) printf("%d\n",i);
      	  insert(keys[i]);
      	  elastic->insert(keys[i]);
      }
      
      int cnt = 0;
      for (map <string,int>:: iterator sit = (--mp.end()); sit!=mp.begin(); sit--)
      {
      	tt[cnt].x=sit->first;
      	tt[cnt++].y=sit->second;
	  }
	  sort(tt,tt+cnt,cmp);
	  cout<<2<<endl;
	  map<string,int> OK;
	  for (int i = 0 ; i < 100; i++)
	  {
	  	OK[tt[i].x] = 1;
	  }
	  
      vector<pair<string,int>> results;
      elastic->get_heavy_hitters(100, results);
      double AAE=0,ARE=0,accuracy = 0;
      cout<<3<<endl;
	  for (int i = 0; i < results.size(); i++)
	  {
	  	if (OK[results[i].first]) accuracy += 1;
	  	AAE += abs(mp[results[i].first] - results[i].second);
	  	ARE += fabs((mp[results[i].first] - results[i].second) / (mp[results[i].first] + 0.0));
	  }
	  printf("accuracy: %.5f\nAAE: %.5f\nARE: %.5f\n",accuracy/100.0,AAE/100.0,ARE/100.0);
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		double th = (double)1000.0 * test_cycles * packet_cnt / resns;
    
    /* free memory */
		for(int i = 0; i < (int)traces[datafileCnt - 1].size(); ++i)
			delete[] keys[i];
		delete[] keys;
    
    printf("throughput is %lf mbps\n", th);
	}
}
