#define MAX_SOL 1000
#define ismax 0
#include <stdbool.h>
#include <stdio.h>
struct edge
{
	int index;
	int u,v;
};
typedef struct edge* edgeptr;
edgeptr edges[10000];

struct solution
{
	int cost;
	int num_vertices;
	int num_edges;
	int V[100];
	edgeptr E[10000];
}; 
typedef struct solution* solnptr;

solnptr naive_solve();
solnptr init_solve();
int sol_bound(solnptr s);
int sol_extend(solnptr sol,solnptr new_sol[MAX_SOL]);
bool sol_done(solnptr sol);
void print_sol(solnptr s);
