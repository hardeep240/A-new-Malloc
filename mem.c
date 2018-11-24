#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <sys/mman.h>

typedef struct head_of_free_chunk
{
    long long int start;
    long long int size;
    struct head_of_free_chunk * next_head;
} free_head;

void Add_Free_Chunk(free_head *, long long int , long long int );
long long int Find_Chunk(free_head * free_list_head,long long int size);
void Remove_Chunk(free_head * free_list_head,long long int addr,long long int size);

typedef struct head_of_allocate_chunk
{
    long long int start;
    long long int size;
    struct head_of_allocate_chunk * next_head;
} alloc_head;

void Add_New_Chunk(alloc_head *alloc_list_head, long long int size_of_alloc_chunk, long long int addr);
void Remove_Alloc_Chunk(free_head *free_list_head,alloc_head * alloc_list_head, long long int addr);
free_head * Mem_Init(int sizeOfRegion);
void *Mem_Alloc(int size);
int Mem_Free(void *ptr);
int Mem_IsValid(void *ptr);
int Mem_GetSize(void *ptr);


//Add_Free_Chunk adds the released chunk to the free list
void Add_Free_Chunk(free_head * free_list_head, long long int size_of_free_chunk, long long int addr)
{ 
    free_head * curr = free_list_head;
    while(curr->next_head!=NULL ){
        curr=curr->next_head;
    }
    free_head * head = (free_head *)(curr + sizeof(free_head));
    head->start = addr;
    head->size = size_of_free_chunk;
    head->next_head = NULL;
    curr->next_head=head;
}

//Find Chunk finds the chunk of asked memory size inside the free list
long long int Find_Chunk(free_head * free_list_head,long long int size)
{
    free_head * curr = free_list_head;
    while(curr!=NULL)
    {
        if(curr->size >= size)
        {
            return curr->start;
        }
        curr = curr->next_head;
    }
    return -1;
}

// Remove_Chunk removes the found chunk of memory from the free list
void Remove_Chunk(free_head * free_list_head,long long int addr,long long int size)
{
    free_head * curr; 
    free_head * prev; 
    prev = free_list_head;
    curr = free_list_head;   
    while(prev->next_head != NULL)
    {
        if(prev->next_head->start == addr)
        {
            curr = prev->next_head;
            break;
        }
        prev = prev->next_head;
    }
   if(curr->size == size)
   {
       prev->next_head = curr->next_head;
   }
   else
   {
       curr->start = addr + size;
       curr->size = curr->size - size;
   }
}

//Add_New_Chunk add the allocated chunk to the alloc list
void Add_New_Chunk(alloc_head * alloc_list_head, long long int size_of_alloc_chunk, long long int addr)
{
    alloc_head * curr = alloc_list_head;
    alloc_head * prev = alloc_list_head;
    while(curr->next_head!= NULL)
    {       
        curr= curr->next_head;
    }
    if(curr == alloc_list_head)
    {
        if(curr->size==0)
        {
            curr->size = size_of_alloc_chunk;
        }
        else
        {
            alloc_head * node = (alloc_head *)(curr + sizeof(alloc_head)); 
            node->start = addr;
            node->size = size_of_alloc_chunk;
            node->next_head = curr->next_head;
            curr->next_head = node;
        }
    }
    else
    {
        alloc_head * node = (alloc_head *)(curr + sizeof(alloc_head)); 
        node->next_head = curr->next_head;
        curr->next_head = node;
        node->start = addr;
        node->size = size_of_alloc_chunk;
    }
}

// Remove_Alloc_Chunk removes the entry of released(freed) memory chunk from the alloc list
void Remove_Alloc_Chunk(free_head *free_list_head,alloc_head * alloc_list_head, long long int addr)
{
    alloc_head * cur = alloc_list_head;
    alloc_head * prev= alloc_list_head;
    if(prev->start==addr){
        cur = prev->next_head;
        Add_Free_Chunk(free_list_head, prev->size, prev->start);
        alloc_list_head = cur;
    }
    else{
        while(prev->next_head!=NULL)
        {
            if(prev->next_head->start == addr)
            {
                cur = prev;
                prev = prev->next_head;
                break;
            }
            prev = prev->next_head;
        }
        Add_Free_Chunk(free_list_head, prev->size,prev->start );
        cur->next_head = prev->next_head;
    }
    
}

free_head * head_free;
alloc_head * head_alloc;
int flag = 0;

free_head * Mem_Init(int sizeOfRegion)
{
    size_t pagesize = getpagesize();
    free_head * ret = (free_head *)mmap(0,sizeOfRegion,PROT_WRITE | PROT_READ,MAP_PRIVATE | MAP_ANON, -1, 0);
    char * x = (char *)ret;
    if(x == MAP_FAILED)
    {
        perror("Could not mmap");
    }
    long long int free_list= 1<<20;
    long long int work_list = 1<<25;
    long long int z = (long long int)(&(*head_free));

    //Initializing the free list
    head_free->start = z + (free_list * 2);
    head_free->size = work_list - 2 * free_list;
    head_free->next_head = NULL;
    
    //Initializing the Alloc list
    head_alloc = (alloc_head *)(head_free + free_list/sizeof(free_head));
    head_alloc->start = z + (free_list * 2);
    head_alloc->size = 0;
    head_alloc->next_head = NULL;
    
    long long int start = z + (free_list * 2);
    return ret;
}

// Mem_Alloc allocates the asked size of memory and ensures that the chunk allocated is removed from the free list
void *Mem_Alloc(int size)
{
	if(flag ==0)
	{
		head_free = Mem_Init(1<<25);
		flag = 1;
	}
    long long int addr = Find_Chunk(head_free,size);   
    void * ptr = addr;
    Remove_Chunk(head_free,addr,size);
    
    Add_New_Chunk(head_alloc,size,addr);
    return ptr;
}

//Mem_IsValid checks if the given address is within the range of any of the allocated chunks, if yes it returns 1 otherwise -1
int Mem_IsValid(void *ptr)
{
    long long int addr = (long long int)(&(*ptr));
    alloc_head * curr = head_alloc;
    while(curr != NULL)
    {
        if((curr->start < addr) && (curr->start + curr->size -1 >= addr))
        {
            return 1;
        }
        curr = curr->next_head;
    }
    printf("Pointer is not present in any allocated chunk of memory\n");
    return 0;
}


//Mem_GetSize finds the full size of the allocated chunk given any address within the range of it.
int Mem_GetSize(void *ptr) 
{
    long long int addr = (long long int)(&(*ptr));
    alloc_head * curr = head_alloc;
    while(curr != NULL)
    {
        if((curr->start < addr) && (curr->start + curr->size -1 >= addr))
        {
            return curr->size;
        }
    }
    printf("Pointer is not present in any allocated chunk of memory\n");
    return 0;
}

// Mem_Free frees the allocated chunk and add it to free list so that it can be allocated later
int Mem_Free(void *ptr)
{
    if(ptr == NULL)
    {
        return -1;
    }
    if(Mem_IsValid(ptr) == 0)
    {
        printf("The address is not a valid allocated address. Sorry, Try again!\n");
        return -1;
    }
    long long int addr = (long long int)(&(*ptr));
    alloc_head * curr = head_alloc;
    while(curr->next_head != NULL)
    {
        if((curr->start < addr) && (curr->start + curr->size -1 >= addr))
        {
            Remove_Alloc_Chunk(head_free,head_alloc,curr->start);
            return 0;
        }
        curr = curr->next_head;
    }
    return -1;
}

