#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#define AP
#define ismax 0
#define MAX_SOL 1000
#define MAX_COST INT_MAX
int cost[1000][1000];
struct solution
{
	int cost;
	int num;
	int job[1000];
};
typedef struct solution *solnptr;
solnptr naive_solve();
solnptr init_solve();
int sol_bound(solnptr s);
int sol_extend(solnptr sol,solnptr new_sol[MAX_SOL]);
bool sol_done(solnptr sol);
void print_sol(solnptr s);
void encode_sol(solnptr s,char* temp);
solnptr decode_sol(char* buf,int* len);
