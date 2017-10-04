#include "openmp_template.h"
#include <omp.h>
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

int main(int argc, char const *argv[])
{
	scanf("%d %d" , &n , &m);
	int i;
	for(i = 0 ; i < m ; i++)
	{
		scanf("%d %d" , &a , &b);
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