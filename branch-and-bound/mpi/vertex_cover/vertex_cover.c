#include "mpi_template.h"
#include <mpi.h>
#include <omp.h>
#include <string.h>
int n , m , a , b;
int degree[100];
int max(int a,int b) { return (a>b)?a:b; }

solnptr init_solve()
{
	solnptr sptr = (solnptr)malloc(sizeof(struct solution));
	sptr->num_vertices = 0;
	sptr->num_edges = 0;
	sptr->cost = 0;
	return sptr;
}

solnptr naive_solve()
{
	solnptr sptr = (solnptr)malloc(sizeof(struct solution));
	sptr->num_vertices = n;
	sptr->num_edges = m;
	sptr->cost = n;
	for(int i = 0; i < n ; i++)
		sptr->V[i] = i;
	for(int i = 0 ; i < m ; i++)
		sptr->E[i] = edges[i];
	return sptr;
}

int sol_bound(solnptr s)
{
	bool rem[100];
	bool remedge[10000];

	for(int i = 0 ; i < n ; i++)
		rem[i] = true;

	for(int i = 0 ; i < s->num_vertices ; i++)
		rem[s->V[i]] = false;

	for(int i = 0 ; i < m ; i++)
		remedge[i] = true;

	for(int i = 0; i < s->num_edges ; i++)
		remedge[s->E[i]->index] = false;

	int maxd = 0;
	
	for(int i = 0 ; i < n ; i++)
	{
		if(rem[i])
			maxd = max(maxd , degree[i]);
	}	

	int reme = 0;				

	for(int i = 0 ; i < m ; i++)
	{
		if(!remedge[i]) continue;
		int u = edges[i]->u;
		int v = edges[i]->v;
		bool included = false;

		for(int j = 0; j < s->num_vertices ; j++)
		{
			if((u == s->V[j]) || (v == s->V[j]))
			{
				included = true;
				break;
			}

			if(included) break;
		}

		if(!included)
			reme++;
	}

	return reme/maxd + s->cost;	
}

int sol_extend(solnptr sol,solnptr new_sol[MAX_SOL])
{
	int cnt = 0;
	bool remedge[10000];
	for(int i = 0 ; i < m ; i++)
		remedge[i] = true;

	for(int i = 0; i < sol->num_edges ; i++)
		remedge[sol->E[i]->index] = false;

	for(int i = 0 ; i < m ; i++)
	{
		int u = edges[i]->u;
		int v = edges[i]->v;
		int ind = edges[i]->index;
		if(!remedge[ind]) continue;
		bool included = false;

		for(int j = 0; j < sol->num_vertices ; j++)
		{
			if((u == sol->V[j]) || (v == sol->V[j]))
			{
				included = true;
				break;
			}
			if(included) 
				break;
		}

		solnptr add_sol = (solnptr)malloc(sizeof(struct solution));
		add_sol->num_vertices = sol->num_vertices;
		add_sol->num_edges = sol->num_edges + 1;

		for(int j = 0; j < sol->num_vertices ; j++)
			add_sol->V[j] = sol->V[j];
		
		for(int j = 0; j < sol->num_edges ; j++)
			add_sol->E[j] = sol->E[j];
		
		add_sol->E[sol->num_edges] = edges[i];
		
		if(!included)
		{
			add_sol->num_vertices++;
			add_sol->V[sol->num_vertices] = u;
		}
		
		add_sol->cost = add_sol->num_vertices;
		new_sol[cnt++] = add_sol;
	}

	return cnt;
}

bool sol_done(solnptr sol)
{
	return(sol->num_edges == m);
}

void print_sol(solnptr s)
{
	if(s == NULL)
		return;

	printf("The number of vertices in a minimum vertex cover is %d . These vertices are as follows:\n",s->num_vertices);
	for(int i = 0; i < s->num_vertices ; i++)
		printf("%d ",s->V[i] + 1);
	printf("\n");
}

void encode_sol(solnptr s,char temp[])
{
	int pos = 0;
	pos += sprintf(temp,"%d %d %d ",s->cost,s->num_vertices,s->num_edges);
	for(int i = 0; i < s->num_vertices; i++)
		pos += sprintf(temp+pos,"%d ",s->V[i]);
	for(int i = 0; i < s->num_edges; i++)
		pos += sprintf(temp+pos,"%d ",s->E[i]->index);
}

solnptr decode_sol(char* buf,int* len)
{
	int pos = 0;
	int num_char;
	solnptr s = (solnptr)malloc(sizeof(struct solution));
	sscanf(buf,"%d %d %d%n",&(s->cost),&(s->num_vertices),&(s->num_edges),&num_char);
	pos += num_char;
	for(int i = 0; i < s->num_vertices ; i++)
	{
		int v;
		sscanf(buf+pos,"%d%n",&v,&num_char);
		pos += num_char;
		s->V[i] = v;
	}
	for(int i = 0; i < s->num_edges ; i++)
	{
		int e;
		sscanf(buf+pos,"%d%n",&e,&num_char);
		pos += num_char;
		s->E[i] = edges[e];
	}
	*len = pos;
	return s;
}
int main(int argc, char *argv[])
{
	FILE *f;
	f = fopen("inp_k4" , "r");

	fscanf(f, "%d %d" , &n , &m);
	int i;
	for(i = 0 ; i < m ; i++)
	{
		fscanf(f, "%d %d" , &a , &b);
		a--;b--;
		degree[a]++;
		degree[b]++;
		edges[i] = (edgeptr)malloc(sizeof(struct edge));
		edges[i]->index = i;
		edges[i]->u = a;
		edges[i]->v = b;
	}

	double start = omp_get_wtime();
	solnptr s = evaluate();
	double duration = omp_get_wtime() - start;
	printf("Time taken (in seconds) : %lf\n",duration);
	print_sol(s);

	return 0;
}