/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// Get allocated flag
#define GET_FLAG(p) ((*p) & 1)

// Set allocated flag
#define SET_FLAG(p)                 ((*p) = ((*p) | 1))

// set size and allocated flag
#define SET_SIZE_AND_FLAG(p,size)   ((*p) = size | 1)

// Get block size
#define GET_SIZE(p) (size_t)((*p) & ~1)


// top and bottom pointers of heap
void *heap_start;
void *heap_end;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    heap_start = NULL;
    heap_end = NULL;
    return 0;
}


void *find_fit(size_t size) {
    size_t *header = (size_t *)heap_start;
    if (heap_start == NULL) {
        return NULL;
    }
    while (header < (size_t *)heap_end) {
        if (GET_FLAG(header) == 0 && GET_SIZE(header) >= size) {
            return header;
        }
        header = (size_t *)((char *)header + GET_SIZE(header));
    }
    return NULL;
}


void split_heap(size_t *header, size_t *footer, size_t size) {
    size_t allsize = GET_SIZE(header);
    size_t size2 = allsize - size;
    if (size2 <= 2 * SIZE_T_SIZE) {
        SET_FLAG(header);
        SET_FLAG(footer);
    } else {
        size_t *header2 = (size_t *)((char *)footer + SIZE_T_SIZE);
        size_t *footer2 = (size_t *)((char *)header2 + size2 - SIZE_T_SIZE);
    
        SET_SIZE_AND_FLAG(header2, size2);
        SET_SIZE_AND_FLAG(footer2, size2);
    }
}


/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t newsize = ALIGN(size + 2 * SIZE_T_SIZE);
    size_t *header = find_fit(newsize);
    size_t *footer;
    if (header == NULL) {
	    if ((header = mem_sbrk(newsize)) == (void *)-1)
            return NULL;
        footer = (size_t *)((char *)header + newsize - SIZE_T_SIZE);
        SET_SIZE_AND_FLAG(header, newsize);
        SET_SIZE_AND_FLAG(footer, newsize);
        heap_end = header;
        if (heap_start == NULL )
            heap_start = header;
    } else {
        footer = (size_t *)((char *)header + GET_SIZE(header) - SIZE_T_SIZE);
        // split_heap(header, footer, newsize);
        SET_FLAG(header);
        SET_FLAG(footer);
    }
    return (void *)((char *)header + SIZE_T_SIZE);
}


void top_coalesce(size_t *header) {
        
}

void bottom_coalesce(size_t *header) {}

void top_and_bottom_coalesce(size_t *header) {

}

void coalesce(size_t *p) {
    size_t *top_header = (size_t *)((char *)p + GET_SIZE(p));
    size_t *bottom_footer = (size_t *)((char *)p - SIZE_T_SIZE);
    if (p == heap_start && p != heap_end) {
        if (GET_FLAG(top_header) == 0) {
            top_coalesce(p);
        }
    } else if (p != heap_start && p == heap_end) {
        if (GET_FLAG(bottom_footer) == 0) {
            bottom_coalesce(p);
        }
    } else if (p != heap_start && p != heap_end) {
        if (GET_FLAG(top_header) == 0 && GET_FLAG(bottom_footer) == 0) {
            top_coalesce(p);
            bottom_coalesce(p);
        } else if (GET_FLAG(top_header) == 0) {
            top_coalesce(p);
        } else if (GET_FLAG(bottom_footer) == 0) {
            bottom_coalesce(p);
        }
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t *header = (size_t *)((char *)ptr - SIZE_T_SIZE);
    size_t *footer = (size_t *)((char *)header + GET_SIZE(header) - SIZE_T_SIZE);
    *header = *header & -2;
    *footer = *footer & -2;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














