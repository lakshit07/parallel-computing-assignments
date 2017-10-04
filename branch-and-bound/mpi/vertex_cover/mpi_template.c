#include "mpi_template.h"
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <mpi.h>
#include <string.h>
int min(int a,int b) { return (a<b)?a:b;}

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

solnptr evaluate1()
{
	head = tail = NULL;
	qsiz = 0;
	solnptr first = init_solve();
	solnptr best = naive_solve();
	push(first);

	while(true)
	{
		int numthreads = 1;
		if(numthreads > qsiz)
			numthreads = qsiz;
		if(!numthreads) break;
		solnptr arr[numthreads];
		for(int i = 0; i < numthreads ; i++)
			arr[i] = pop();
		
		for (int i = 0; i < numthreads ; ++i)
		{
			solnptr sol = arr[i];
			if(sol_done(sol))
			{
				if(ismax)
				{
					if(sol->cost > best->cost)
						best = sol;
				}
				else
				{
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
						
						for(int j = 0; j < num_sol ; j++)
						{
							push(new_sol[j]);
						}
					}
				}
				else
				{
					if(sol_bound(sol) < best->cost)
					{
						solnptr new_sol[MAX_SOL];
						int num_sol = sol_extend(sol,new_sol);
						
						for(int j = 0; j < num_sol ; j++)
						{
							push(new_sol[j]);
						}
		
					}
				}
			}
		}

	}
	return best;

}

solnptr evaluate()
{
	MPI_Init(0, NULL);
	int pid,num_proc;
	MPI_Comm_rank(MPI_COMM_WORLD , &pid);
	MPI_Comm_size(MPI_COMM_WORLD , &num_proc);
	int opt;
	bool idle[num_proc];
	int idle_no;
	qsiz = 0;
    
    if(num_proc == 1)
    	return evaluate1();

	if(pid == 0)
	{
		solnptr first = init_solve();
		solnptr best = naive_solve();
		idle[0] = false;
		for(int i = 1; i < num_proc ; i++)
			idle[i] = true;
		idle_no = num_proc - 1;
		strcpy(master,"P ");
		char temp[100];
		encode_sol(first,temp);
		strcat(master,temp);
		sprintf(temp,"%d ",best->cost);
		strcat(master,temp);
		MPI_Send(master,strlen(master)+1,MPI_CHAR,1,0,MPI_COMM_WORLD);
		idle[1] = false;
		idle_no--;
		while(idle_no < num_proc - 1)
		{
			MPI_Status s;
			int number_amount;
			MPI_Recv(master,BUF_SIZ,MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&s);
			MPI_Get_count(&s, MPI_CHAR, &number_amount);

            if(number_amount == 0)
            	return NULL;

			int sno = s.MPI_SOURCE;
			char op = master[0];
			if(op == 'S')
			{
				int needed;
				sscanf(master+2,"%d",&needed);
				int given = min(needed,idle_no);
				sprintf(master,"%d ",given);
				sprintf(temp,"%d ",best->cost);
				strcat(master,temp);

				for(int i = 1; (given > 0) && (i < num_proc) ; i++)
				{
					if(idle[i])
					{
						sprintf(temp,"%d ",i);
						strcat(master,temp);
						idle[i] = false;
						idle_no--;
						given--;
					}
				}
				
				MPI_Send(master,strlen(master)+1,MPI_CHAR,sno,0,MPI_COMM_WORLD);

			}
			else if(op == 'I')
			{
				idle_no++;
				idle[sno] = true;
			}
			else if(op == 'D')
			{
				int l;
				solnptr new_sol = decode_sol(master+2,&l);
				if((ismax && (new_sol->cost > best->cost)) || (!ismax && (new_sol->cost < best->cost)))
				{
					best = new_sol;
					opt = best->cost;
				}
			}

		}
		for(int i = 1; i < num_proc ; i++)
		{
			master[0] = 'F';
			master[1] = '\0';
			MPI_Send(master,2,MPI_CHAR,i,0,MPI_COMM_WORLD);
		}
		return best;

	}
	else
	{
		while(1)
		{
			MPI_Status s;
			int number_amount;
			MPI_Recv(slave,BUF_SIZ,MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&s);
			MPI_Get_count(&s, MPI_CHAR, &number_amount);

            if(number_amount == 0)
            	return NULL;

			char op = slave[0];
			char temp[100];
			if(op == 'P')
			{
				int len_sol;
				solnptr sol = decode_sol(slave+2,&len_sol);
				push(sol);
				sscanf(slave+2+len_sol,"%d",&opt);
				while(qsiz > 0)
				{
					solnptr sol = pop();
					if(ismax && (sol_bound(sol) < opt)) continue;
					if(!ismax && (sol_bound(sol) > opt)) continue;
					if(sol_done(sol))
					{
						strcpy(slave,"D ");
						encode_sol(sol,temp);
						strcat(slave,temp);
						MPI_Send(slave,strlen(slave)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
						continue;
					}
					solnptr new_sol[MAX_SOL];
					int num_sol = sol_extend(sol,new_sol);
					for(int i = 0; i < num_sol ; i++)
						push(new_sol[i]);
					if(!num_sol && !qsiz) break;
					sprintf(slave,"S %d",qsiz);
					MPI_Send(slave,strlen(slave)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
					MPI_Recv(slave,BUF_SIZ,MPI_CHAR,0,0,MPI_COMM_WORLD,&s);

            		 if(number_amount == 0)
           			 	return NULL;
            
					int avail;
					int pos,len;
					sscanf(slave,"%d%n",&avail,&len);
					pos = len;
					sscanf(slave+pos,"%d%n",&opt,&len);
					pos += len;
	
					for(int i = 0; i < avail ; i++)
					{
						int sno;
						sscanf(slave+pos,"%d%n",&sno,&len);
						pos += len;
						strcpy(reply,"P ");
						solnptr sol = pop();
						encode_sol(sol,temp);
						strcat(reply,temp);
						sprintf(temp,"%d ",opt);
						strcat(reply,temp);
						MPI_Send(reply,strlen(reply)+1,MPI_CHAR,sno,0,MPI_COMM_WORLD);
					}
				}
				strcpy(slave,"I ");
				MPI_Send(slave,strlen(slave)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);

			}
			else if(op == 'F')
			{
				break;
			}
		}
	}
	MPI_Finalize();
}