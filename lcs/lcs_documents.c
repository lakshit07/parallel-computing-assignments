// Anirudh Kumar Bansal   2014A7PS095P
// Lakshit Bhutani   2014A7PS095P

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"
#define MAX_LINES 150
#define MAX_LINELEN 550
#define MAX_N 100

unsigned long all_hash[MAX_N][MAX_LINES];
char all_lines[MAX_N][MAX_LINES][MAX_LINELEN+1];
int lcslen[MAX_LINES+1][MAX_LINES+1];
int istopk[MAX_N];
int topk[MAX_N];
int solution[MAX_N][MAX_N];
#pragma omp threadprivate(lcslen)

/*
sdbm
----
This algorithm was created for sdbm (a public-domain reimplementation of ndbm) database library. 
It was found to do well in scrambling bits, causing better distribution of the keys and fewer splits.
It also happens to be a good general hashing function with good distribution. 
The actual function is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below is the faster version used in gawk. 
The magic constant 65599 was picked out of thin air while experimenting with different constants, and turns out to be a prime. 
This is one of the algorithms used in berkeley db. 
*/

unsigned long hash(unsigned char* str)
{
  unsigned long hash = 0;
    int c;

    while (c = *str++)
       hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

typedef struct
{
	int x , y , len;
}lcs_pair_node;

lcs_pair_node lcs_pair[MAX_N*MAX_N];

int cmp(const void* a , const void* b)
{
	return (((lcs_pair_node*)b)->len) - (((lcs_pair_node*)a)->len);
}

int max(int a,int b){return (a>b)?a:b;}

void getlcs(int file1,int lines1,int file2,int lines2)
{
	int i,j;
	int pos = 0;
  int chunk = 1;
  int length;
	
  for(i = 0 ; i <= lines1 ; i++)
	{
		for(j = 0 ; j <= lines2 ; j++)
		{
			if(i == 0 || j == 0)
				lcslen[i][j] = 0;
			else if(all_hash[file1][i-1] == all_hash[file2][j-1])
			{
				lcslen[i][j] = lcslen[i-1][j-1] + 1;
			}
			else
				lcslen[i][j] = max(lcslen[i-1][j],lcslen[i][j-1]);
		}
	}
	
  solution[file1][file2] = lcslen[lines1][lines2];
}

int readfile(FILE* fp,int fileno)
{
	int num_lines = 0;
  char str[MAX_LINELEN+1];

	while(fgets(str,MAX_LINELEN+1,fp) != NULL)
	{
		size_t len = strlen(str);
        if (len > 0 && str[len-1] == '\n')
        str[--len] = '\0';

		all_hash[fileno][num_lines] = hash(str);
		strcpy(all_lines[fileno][num_lines],str);
    num_lines++;
	}

	fclose(fp);
	return num_lines;
}


int main(int argc, char const *argv[])
{
  int N , k;
  printf("N = ");
  scanf("%d" , &N);
  printf("K = ");
  scanf("%d" , &k);
  double start = omp_get_wtime();
  int lines[N + 2];
  FILE* fp[N + 2];
  int i,j,l;  

  #pragma omp parallel for private(i)
  for(i = 1 ; i <= N ; i++)
  {
    char temp[10];
    char fname[10];
    strcpy(fname , "text");
    sprintf(temp , "%d" , i);
    strcat(fname , temp);
    fp[i] = fopen(fname , "r");
    lines[i-1] = readfile(fp[i] , i-1);
  }

  int chunk = 1;
  int m;

   #pragma omp parallel for default(shared) private(l,m)
    for(m = 0 ; m < N-1 ; m++)
    {
    	for(l = m+1 ; l < N ; l++)
    		getlcs(m,lines[m],l,lines[l]);
    }

  int z = 0;

  for(i = 0 ; i < N ; i++)
  {
  	for(j = i+1 ; j < N ; j++)
  	{
  	    lcs_pair[z].x = i;
    		lcs_pair[z].y = j;
    		lcs_pair[z++].len = solution[i][j];        
  	}	
  }

  FILE *fw = fopen("out_lcs","w");

  qsort(lcs_pair , MAX_N*MAX_N , sizeof(lcs_pair_node) , cmp);

  for(i = 0 ; i < MAX_N ; i++)
  	istopk[i] = 0;

  int unique = 0;

  //#pragma omp parellel for private(i)
  for(i = 0 ; i < k ; i++)
  {
  	int n1 = lcs_pair[i].x;

  	if(!istopk[n1])
  	{
  		topk[unique] = n1;
      unique++;
  		istopk[n1] = 1;
  	}

  	int n2 = lcs_pair[i].y;

  	if(!istopk[n2])
  	{
  		topk[unique] = n2;
      unique++; 
  		istopk[n2] = 1;
  	}
  }  

  int minline = MAX_LINES+1;
  int minpos;

  for(i = 0 ; i < unique ; i++)
  {
  	if(lines[topk[i]] < minline)
  	{
  		minline = lines[topk[i]];
  		minpos = i;
  	}
  }

  char intersection[MAX_LINES][MAX_LINELEN+1];
  int in_count = 0;
  int minindex = topk[minpos];
  
  #pragma omp parallel default(shared) private(i,j,k)
  {
    #pragma omp for schedule(dynamic,chunk) 
    for(i = 0 ; i < minline ; i++)
    {
  	   int flag = 1;
  	   
       for(j = 0 ; j < unique ; j++)
  	   {
    		int ind = topk[j];
    		int num_lines = lines[ind];
    		int occur = 0;

    		if(j == minpos) 
          continue;
  		
        for(k = 0 ; k < num_lines ; k++)
  		  {
  		  	if(all_hash[minindex][i] == all_hash[ind][k])
  			 {
  			   	occur = 1;
  				  break;
  			 }
  		  }

  		  if(!occur)
  		  {
  			    flag = 0;
  	        break;
  	    }
  	   }
    
  	if(flag)
  		strcpy(intersection[in_count++],all_lines[minindex][i]);
    }
  }

  for(i = 0 ; i < in_count ; i++)
    fprintf(fw , "%s\n" , intersection[i]);

  fclose(fw);

  double duration = omp_get_wtime() - start;
  printf("Time taken : %lf sec\n" , duration);
  
  return 0;
}
