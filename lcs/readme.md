## LCS of Documents

* The problem involves computing the pair-wise length of LCS *(longest
common-sub-sequence)* of **N** documents by considering each line as a sequence  
* Top **K** pair of documents with longest LCS lengths are selected and pairwise LCS of
these documents computed   
* Finally intersection of the content in the **K** documents is
calculated  

The problem has been done using **OpenMP** (*Open Multi-Processing*)

________________

* **Compile**

```bash
gcc -fopenmp lcs_documents.c
```
*[Make sure ‘test1’, ‘test2’, ‘test3’ , ... files are present in the **same** directory as the source file **lcs_documents.c** before running the above command. The maximum number of documents is 100 but user can work with any number of documents by changing the **MAX_N** parameter in the code. For testing some text files are given in **test_cases_1** folder]*

* **Run**
```bash
./a.out
```
_____________________

* NOTE

1. Then enter the value of ‘N’ and ‘K’ as prompted. Do ensure that value of K is less
than (N*(N+1))/2

2.  The result on the terminal displays the time taken by the code. The output file of the
intersection of documents will be created with name **out_lcs**

3. Make sure that the documents are named as text1 , text2, text3, ... , textn. The
documents used for testing are provided in ‘test_cases_1’ folder
____________

