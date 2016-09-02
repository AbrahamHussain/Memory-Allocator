/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include "myalloc.h"


/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;


static unsigned char *freeptr;
static unsigned char *local_var; // pointer used to find the most optimal block

/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 */
void init_myalloc() {

    /*
     * Allocate the entire memory pool, from which our simple allocator will
     * serve allocation requests.
     */
    mem = (unsigned char *) malloc(MEMORY_SIZE); 
    if (mem == 0) {
        fprintf(stderr,
                "init_myalloc: could not get %d bytes from the system\n",
                MEMORY_SIZE);
        abort();
    } 

    freeptr = mem;
    
    // This is going to write the amount of memory to the header 
    // (first four bytes)
    *((int *) freeptr) = MEMORY_SIZE - 8; 
    // This is going to write the amount of memory to the the footer
    // (last four bytes )
    *((int *) (freeptr + MEMORY_SIZE - 4)) = MEMORY_SIZE - 8;
}


/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 */
unsigned char *myalloc(int size) {
    freeptr = mem;
    local_var = NULL; 
    int counter = 0;
    
    // we are going to loop through all the memory block in order 
    // to find the best fit
    while ((unsigned int) freeptr < (unsigned int) (mem + MEMORY_SIZE)) 
    {
        // Tests if the block is a big enough size  
        if ((*((int *) freeptr)) >= (size + 8)) 
        {
            // if there is no potential block we will set it to the 
            // first valid block
            if (local_var == NULL) 
            {
                local_var = freeptr; 
            } 
            else if ((*((int *) local_var)) > (*((int *) freeptr))) 
            {
                local_var = freeptr;
            } 
        }
        // This is going to increment your freeptr
        counter++;
        freeptr = freeptr + (abs(*((int *) freeptr))) + 8; 
    }
    // If loc is still Null then this means that there is no more memory
    // to allocate 
    if(local_var == NULL || local_var > (mem + MEMORY_SIZE) || 
      local_var < mem) {
        fprintf(stderr, "myalloc: cannot service request of size %d with"
                " %d bytes allocated\n", size, (freeptr - mem));
        return (unsigned char *) 0;
    }
    // now we are going to allocate the memory by using the local variable 
    // to change the
    else {
        *((int *) (local_var + size + 8)) =  
        abs(*((int *) local_var)) - size - 8; 

        *(int *)((*(int *)local_var) + local_var + 4) = 
        (*((int *) local_var)) - size - 8;
        // this sets the header to a negative value in order to negative 
        // to indicate that it is now not free 
        *(int *) local_var = -1* size; 
        // this sets the footer a similar way to the header 
        *((int *) (local_var + size + 4)) = -1*size; 
        // this does it for the header for the part of the block that is
        // excessive 
        //*((int *) (local_var + size + 8)) =  *((int *) local_var) - size - 8; 
        // this does it for the footer 
        //*((int *) local_var + size) = (*((int *) local_var)) - size - 8;
    } 

    return local_var + 4; 
}


/*!
 * Free a previously allocated pointer.  oldptr should be an address returned by
 * myalloc().
 */
void myfree(unsigned char *oldptr) {
   // the following two lines is going to change the the header and the footer
   // to a positive number to indicate that it is free
   *(int *) (oldptr - 4) = abs(*(int *) (oldptr - 4));
   *(int *) (oldptr  + *(int *) (oldptr - 4)) = abs(*(int *) (oldptr - 4));

   // checks the left side to coalecse 
   if ((oldptr - 8 > mem) && *((int *)(oldptr - 8)) > 0) {

       *(int*)((oldptr - 4) - *(int *)(oldptr - 8) - 8) = 
       *(int *)(oldptr - 4) + 8 + *(int *)(oldptr - 8);

       *(int *)(oldptr + *(int *)(oldptr - 4)) = 
       *(int *)(oldptr - 4) + 8 + *(int *)(oldptr - 8);
       // if this is the case then we must move the pointer for the header value in case
       oldptr = (oldptr - 4) - *(int *)(oldptr - 8) - 4;
   }

   // checks the right side to coalesce 
   local_var = oldptr  + abs(*(int *)(oldptr - 4)) + 4;
    if ((local_var < mem + MEMORY_SIZE) && (*(int*)(local_var) > 0)) 
      {
          *(int *)(local_var + 4 + *(int *)local_var) = 
          *(int*)(oldptr - 4) + *(int *)(local_var) + 8;
          *(int *)(oldptr - 4) = *(int*)(oldptr - 4) + *(int *)(local_var) + 8;
      }   







   
}