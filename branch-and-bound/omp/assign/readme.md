* To compile the program - 
``` bash
make
```
* To executing the program -  

``` bash
set OMP_NUM_THREADS = x 
./assign < inputfile 
```
**(where x is the number of threads : 1 , 2 , 4 or 8 and inputfile is the name of the input file)**  

To specify the input file, go to the *assign.c* file and change the first parameter of the *fopen()* function in main() function in line number **128**.

* Input file format : 

```		
N   
c11 c12 c13 .... c1N
c21 c22 c23 .... c2N
c31 c32 c33 .... c3N
....
cN1 cN2 cN3 .... cNN
```
*where N = number of agents/tasks and cij represents the cost associated with assigning the jth task to agent i.*

* Sample test cases (as mentioned in the assignment pdf on which testing was done):

1. inp8 : A 8X8 cost matrix with 8 agents and 8 tasks.
2. inp9 : A 9X9 cost matrix with 9 agents and 9 tasks.

* Other sample test cases provided :
1. inp4 : A 4X4 cost matrix with 4 agents and 4 tasks.
2. inp5 : A 5X5 cost matrix with 5 agents and 5 tasks.
3. inp6 : A 6X6 cost matrix with 6 agents and 6 tasks. 
