#include "assign.h"
#include <stdlib.h>
#define BUF_SIZ 1000
char master[BUF_SIZ];
char slave[BUF_SIZ];
char reply[BUF_SIZ];
void push(solnptr s);
solnptr pop();
solnptr evaluate();
int min(int a,int b);