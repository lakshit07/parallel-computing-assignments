#include <stdio.h>
#include <stdbool.h>
#include "omp.h"
#include <malloc.h>
#include "openmp_template.h"

struct node
{
	solnptr s;
	struct node* next;
};

typedef struct node* nodeptr;
nodeptr head , tail;
int qsiz;

void push(solnptr s)
{
	if(head == NULL)
	{
		nodeptr nd = (nodeptr)malloc(sizeof(struct node));
		nd->s = s;
		nd->next = NULL;
		head = tail = nd;
	}
	else
	{
		nodeptr nd = (nodeptr)malloc(sizeof(struct node));
		nd->s = s;
		nd->next = NULL;
		tail->next = nd;
		tail = nd;
	}
	qsiz++;
}

solnptr pop()
{
	qsiz--;
	if(head == tail)
		return head->s;

	nodeptr n = head;
	head = head->next;
	solnptr sl = head->s;
	free(n);
	return sl;
}

solnptr evaluate()
{
	head = tail = NULL;
	qsiz = 0;
	solnptr first = init_solve();
	solnptr best = naive_solve();
	push(first);

	while(true)
	{
		int numthreads = omp_get_num_threads();
		if(numthreads > qsiz)
			numthreads = qsiz;
		if(!numthreads) break;
		solnptr arr[numthreads];
		for(int i = 0; i < numthreads ; i++)
			arr[i] = pop();
		
		#pragma omp parallel for 
		for (int i = 0; i < numthreads ; ++i)
		{
			solnptr sol = arr[i];
			if(sol_done(sol))
			{
				if(ismax)
				{
					#pragma omp critical
					if(sol->cost > best->cost)
						best = sol;
				}
				else
				{
					#pragma omp critical
					if(sol->cost < best->cost)
						best = sol;
				}
			}
			else
			{
				if(ismax)
				{
					if(sol_bound(sol) > best->cost)
					{
						solnptr new_sol[MAX_SOL];
						int num_sol = sol_extend(sol,new_sol);
						#pragma omp critical
						{
							for(int j = 0; j < num_sol ; j++)
							{
								push(new_sol[j]);
							}
						}
					}
				}
				else
				{
					if(sol_bound(sol) < best->cost)
					{
						solnptr new_sol[MAX_SOL];
						int num_sol = sol_extend(sol,new_sol);
						#pragma omp critical
						{
							for(int j = 0; j < num_sol ; j++)
							{
								push(new_sol[j]);
							}
						}
					}
				}
			}
		}


	}
	return best;

}  