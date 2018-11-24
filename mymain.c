#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
void main()
{
//Testing by making calls to Mem_Alloc and Mem_Free
    int * alloc1 =  (int *)Mem_Alloc(10 * sizeof(int));
    int * alloc2 = (int *)Mem_Alloc(15 * sizeof(int));
    int * alloc3 = (int *)Mem_Alloc(25* sizeof(int));
    int * alloc4 = (int *)Mem_Alloc(10 * sizeof(int));
    int * alloc5 = (int *)Mem_Alloc(20 * sizeof(int));

    int check1, check2, check3;
    check1 = Mem_Free(alloc1 + 15);
    check2 = Mem_Free(alloc1 + 5);
    int * alloc6 = (int *)Mem_Alloc(20 * sizeof(int));
    int * alloc7 = (int *)Mem_Alloc(20 * sizeof(int));
    check3 = Mem_Free(alloc5 + 19);
	printf("ddfnknkdf check1 %d\n", check1);
// Printing the final free list and allocated list
   /** alloc_head *temp;
    temp = head_alloc;
    while(temp!=NULL){
        printf("Alloc list: start address of chunk: %lld with Size: %lld \n", temp->start, temp->size);
        temp=temp->next_head;
    }
    free_head *temp1;
    temp1 = head_free;
    while(temp1!=NULL){
        printf("FREE list: Start address of free chunk: %lld with size %lld \n", temp1->start, temp1->size);
        temp1=temp1->next_head;
    }**/

   // munmap(head_free,1<<25);
}
