* To compiling the program -
```
make
```
* To execute the program - 
 ```bash
set OMP_NUM_THREADS = x 
./vertex_cover < inputfile
 ```
**(where x is the number of threads : 1 , 2 , 4 or 8 and inputfile is the name of the input file)**

* To specify the input file, go to the *vertex_cover.c* file and change the first parameter of the *fopen()* function in main() function in line number **190**.

Input file format -
```
N M
u1 v1
u2 v2
u3 v3
....
uM vM
```

*where N = number of vertices , M = number of edges and ui and vi represent the endpoints of the ith edge*

* Sample test cases (as mentioned in the assignment pdf on which testing was done):

1. inp_linear : Linear graph with 10 vertices and 9 edges.
2. inp_k33    : A complete bipartite graph i.e K 3,3. 

* Other sample test cases provided :
1. inp_k4 : A complete graph of 4 vertices.
2. inp_k23 : A complete bipartite graph i.e K 2,3. 

