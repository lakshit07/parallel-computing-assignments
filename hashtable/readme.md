## Concurrent Hash Table

* The problem involves implementing a concurrent open-addressed hashtable with
*quadratic probing* that supports operations to initialize the hash table having capacity **C**, add an element,
delete an element, find an element and rehash the hash table
* Rehashing is invoked
internally if the load factor exceeds a given threshold **L** and it must be run in the
background to ensure that other operations are locked out for the smallest time
possible   

The problem has been done using **Pthreads**(*POSIX Threads*)

________________

* **Compile**

```bash
gcc -w -fopenmp -pthread hash_table_parallel.c
```
*[Make sure the **query** file containing the input queries is present in the same
directory before running. The total number of queries in the **query** file provided in
the test_cases_2 are 10^5 but user can work with any number of queries by changing
the **NUM_QUERIES** parameter in the code]*

* **Run**
```bash
./a.out
```
_____________________

* NOTE

1. On execution, enter the value of **C** and **L** as prompted. Do ensure that value of **L** is between
0 and 1  
2. The result on the terminal displays the time taken by the code. The output file
containing the result of queries executed by the threads will be created with name
**query_out_parallel**. The actual queries (*in the order executed by the threads*) will be
created in the file **query_actual**
3. The file used for testing is **query**, which was generated randomly is available in
folder **test_cases_2**. The format of the queries is -  
	*  **0 key** - *find* element with given key
	* **1 key value** - *add* element with given key and value
	* **2 key** - *delete* element with given key

________________



