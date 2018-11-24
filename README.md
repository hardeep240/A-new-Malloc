# A-new-Malloc
The aim of this project is to create a new improved version of malloc wherein while freeing, the user does not need to always pass the pointer to the start of a chunk of object memory in order to free it. They can pass a pointer to any position of the chunk - our function detects which object it belongs to, and accordingly frees the entire chunk.
