#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"
#include <pthread.h>

#define NUM_QUERIES 100000
#define NUM_OF_THREADS 4

typedef enum {vacant, filled, deleted} status;
int counter = 0;
int c;
double l;
pthread_mutex_t mutex[2*NUM_QUERIES];
pthread_mutex_t mutex_f , mutex_c;
pthread_mutex_t mutex_h;
pthread_mutex_t active_mod;
int active = 0;

struct ele
{
	int key;
	int value;
};

typedef struct ele* element;

struct HashTable
{
	int size;
	status s;
	element data;
};

typedef struct HashTable* head;

FILE *f;
FILE *out;
FILE *outq;
int query[NUM_QUERIES][3];
head h,n;
int q = 0;

void rehash();

int hash(int k)
{
	return k % (h->size);
}

void init(int c)
{
	h = (head)malloc(c * sizeof(struct HashTable));

	int i;

	for(i = 0 ; i < c ; i++)
	{
		h[i].s = vacant;
		h[i].data = (element)malloc(sizeof(struct ele));
		h[i].data->key = -1;
	}

	h->size = c;
}


void add(int k, int val, head h,int tq)
{
	element e = (element)malloc(sizeof(struct ele));
	e->key = k;
	e->value = val;

	int j = 0;
	int siz = h->size;
	int flag = 0;
	int pos;

	while(j < siz)
	{
		pos = (hash(k) + (j*j+j)/2) % siz;

		pthread_mutex_lock(&mutex[pos]);
		
		if(h[pos].s == vacant || h[pos].s == deleted)
		{			
			//pthread_mutex_lock(&mutex_h);
			h[pos].s = filled;
			h[pos].data = e;
			//pthread_mutex_unlock(&mutex_h);		

			pthread_mutex_lock(&mutex_c);
			counter++;
			pthread_mutex_unlock(&mutex_c);
			flag = 1;
			pthread_mutex_lock(&mutex_f);
		    fprintf(out , "Key %d added at position %d\n",k,pos);
		    fprintf(outq, "1 %d %d\n",k,val);
		    pthread_mutex_unlock(&mutex_f);	
		    pthread_mutex_unlock(&mutex[pos]);
			break;
		}

		pthread_mutex_unlock(&mutex[pos]);
		
		j++;
	}
	pthread_mutex_unlock(&mutex[pos]);
	double lf = (double)counter / siz;

	if(lf > l)
	{
		pthread_mutex_lock(&mutex_c);
		counter=0;
		pthread_mutex_unlock(&mutex_c);
		
		rehash(tq);
	}
	/*pthread_mutex_lock(&active_mod);
	active--;
	pthread_mutex_unlock(&active_mod);*/
	if(j == siz)
    {
		pthread_mutex_lock(&mutex_f);
		fprintf(out , "Key %d cannot be added\n",k);
		fprintf(outq, "1 %d %d\n",k,val);
		pthread_mutex_unlock(&mutex_f);
	}	

}

int find(int k,int tq)
{
	int j = 0;
	int siz = h->size;
	int flag = 0;
	int pos;

	while(j < siz)
	{
		pos = (hash(k) + (j*j+j)/2) % siz;

		pthread_mutex_lock(&mutex[pos]);
		if(h[pos].data->key == k && h[pos].s == filled)
		{
			/*pthread_mutex_lock(&active_mod);
			active--;
			pthread_mutex_unlock(&active_mod);*/
			pthread_mutex_lock(&mutex_f);
			fprintf(out , "Key %d found at %d\n",k,pos);
			fprintf(outq , "0 %d\n",k);
			pthread_mutex_unlock(&mutex_f);
			pthread_mutex_unlock(&mutex[pos]);
			return pos;
		}
		else if(h[pos].s == vacant || h[pos].s == deleted)
		{
			flag = 1;
			/*pthread_mutex_lock(&active_mod);
			active--;
			pthread_mutex_unlock(&active_mod);*/
			pthread_mutex_lock(&mutex_f);
			fprintf(out , "Key %d not found\n",k);
			fprintf(outq, "0 %d\n",k);
			pthread_mutex_unlock(&mutex_f);
			pthread_mutex_unlock(&mutex[pos]);
			return -1;	
		}
		pthread_mutex_unlock(&mutex[pos]);
		
		j++;
	}
 
 if(!flag)
 {
 	/*pthread_mutex_lock(&active_mod);
	active--;
	pthread_mutex_unlock(&active_mod);*/
 }
pthread_mutex_lock(&mutex_f);
fprintf(out , "Key %d not found\n",k);
fprintf(outq, "0 %d\n",k);
pthread_mutex_unlock(&mutex_f);

	return -1;
}

void delete(int k,int tq)
{	
	int j = 0;
	int siz = h->size;
	int flag = 0;
	int pos;

	while(j < siz)
	{
		pos = (hash(k) + (j*j+j)/2) % siz;
		pthread_mutex_lock(&mutex[pos]);

		if(h[pos].data->key == k && h[pos].s == filled)
		{
			h[pos].data->key = -1;
			h[pos].s = deleted;
			/*pthread_mutex_lock(&active_mod);
			active--;
			pthread_mutex_unlock(&active_mod);*/
			pthread_mutex_lock(&mutex_f);
			fprintf(out , "Key %d deleted from %d\n",k,pos);
			fprintf(outq , "2 %d\n",k);
			pthread_mutex_unlock(&mutex_f);
			pthread_mutex_unlock(&mutex[pos]);
			return;
		}
		else if(h[pos].s == vacant || h[pos].s == deleted)
		{
			flag = 1;
			/*pthread_mutex_lock(&active_mod);
			active--;
			pthread_mutex_unlock(&active_mod);*/
			pthread_mutex_unlock(&mutex[pos]);
			break;
	    }
	    pthread_mutex_unlock(&mutex[pos]);	
		
		j++;
	}
		
    if(!flag)
    {
		/*pthread_mutex_lock(&active_mod);
		active--;
		pthread_mutex_unlock(&active_mod);*/
	}
	else
	{
		pthread_mutex_lock(&mutex_f);
		fprintf(out , "Key %d to be deleted not found\n",k);
		fprintf(outq, "2 %d\n",k);
		pthread_mutex_unlock(&mutex_f);
	}
}

pthread_mutex_t assign,queries;

void* start(void *id)
{
	int tid = (int)id;
	int type , key , value;
	int tq;

	while(q < NUM_QUERIES)
	{
		pthread_mutex_lock(&queries);
		pthread_mutex_lock(&assign);
		if(q >= NUM_QUERIES)
		{
			pthread_mutex_unlock(&assign);
			pthread_mutex_unlock(&queries);
			return;
		}
	    type = query[q][0];
		key = query[q][1];
		value = query[q][2];
		//printf("%d executing query : %d\n" , tid, q);
		tq = q;
		q++;
		active++;
		pthread_mutex_unlock(&assign);
		pthread_mutex_unlock(&queries);

		if(type == 0)
		{
			int pos = find(key,tq);
		}
		else if(type == 1)
		{
			add(key , value, h,tq);
		}
		else
		{
			delete(key,tq);
		}
        pthread_mutex_lock(&active_mod);
        active--;
        pthread_mutex_unlock(&active_mod);

	}	

}

int main(int argc, char const *argv[])
{
	printf("C = ");
	scanf("%lf" , &c);
	printf("L = ");
	scanf("%d" , &l);
	init(c);
    f = fopen("query","r");
    out = fopen("query_out_parallel","w");
    outq = fopen("query_actual","w");
    int i = 0;

    while(i < NUM_QUERIES)
    {
    	fscanf(f,"%d %d",&query[i][0],&query[i][1]);
    	if(query[i][0] == 1)
    		fscanf(f,"%d",&query[i][2]);
    	else
    		query[i][2] = -1;
    	i++;
    }

    double start_time = omp_get_wtime();	

	pthread_t thread[NUM_OF_THREADS];

	for(i = 0 ; i < NUM_OF_THREADS ; i++)
		pthread_create(&thread[i] , NULL, start ,(void*)i);

	for(i = 0 ; i < NUM_OF_THREADS ; i++)
		pthread_join(thread[i] , NULL);	

	printf("Time taken : %lf sec\n" , omp_get_wtime() - start_time);

	fclose(f);
	fclose(out);
	fclose(outq);

	return 0;
}

void rehash(int tq)
{
	int original = h->size;
	n = (head)malloc((2*original)*sizeof(struct HashTable));
	n->size = 2*original;
	int i;

	for(i = 0 ; i < 2*original ; i++)
	{
		n[i].s = vacant;
		n[i].data = (element)malloc(sizeof(struct ele));
		n[i].data->key = -1;
	}
	while(active > 1);
	pthread_mutex_lock(&queries);
	pthread_mutex_lock(&mutex_f);
	fprintf(out,"Rehash called\n");
	pthread_mutex_unlock(&mutex_f);

	for(i = 0 ; i < original ; i++)
	{
		if(h[i].s == filled)
		{
			add(h[i].data->key, h[i].data->value,n,tq);
		}
	}
		
	h = n;
	pthread_mutex_lock(&mutex_f);
	fprintf(out,"Rehash done\n");
	pthread_mutex_unlock(&mutex_f);

	pthread_mutex_unlock(&queries);

}
