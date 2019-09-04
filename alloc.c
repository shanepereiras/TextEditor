/*
 *
 * BertOS - Memory allocator
 * src/alloc.c
 *
 * Copyright (C) 2019 Bastiaan Teeuwen <bastiaan@mkcl.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

#include "include/alloc.h"
#include "include/sys/print.h"

static struct block *head;

/* A pointer to the end of the kernel, all memory past this point is free */
extern uintptr_t kern_end;


static void *block_alloc(size_t n)
{
	//Printline for debugging
	printf("Creating a new block\n");
	//New block pointer
	struct block *bptr;

	//Check if head is empty
	if (!head) {
		//set the address for the first block equal to the end address of the kernel.
		bptr = (void*)&kern_end;
		//set the new block size
		bptr->size = n;
		//Set the new block writable address behind the struct itself.
		bptr->addr = ((void *)bptr + sizeof(struct block));
		//Next is still null
		bptr->next = NULL;
		//The block is now in use
		bptr->used = true;
		//Head is now the new block
		head = bptr;		
		//Printline for debugging
		printf("The end of the kernel is %p\n", kern_end);
		//Return the new writable address 
		return head->addr;
		
	}
	//If the head is not empty
	else {
		//Set the new block equal to the head
			bptr = head;
			//Loop through the list untill the last block is found
			while (bptr->next != NULL) {
				//if there is a next linked block, set the current block equal to it
				bptr = bptr->next;
			}
			//Set the address for the new block equal to the writable address after the last block
			bptr->next = (void *)(bptr->addr + bptr->size);
			//Set the size
			bptr->next->size = n;
			//Set the new block writable address behind the struct itself.
			bptr->next->addr = ((void *)bptr->next + sizeof(struct block));
			bptr->next->used = true;
			bptr->next->next = NULL;
			//Return the new block's writable address
			return bptr->next->addr;
		}
	return NULL;
}

static void block_free(struct block *bptr)
{
	//Set the block used variable back to false
	bptr->used = false;
}

static void *block_get(size_t n)
{
	//Current block that the loop is on.
	struct block *currentBlock = head;
	//Loop throuhg the linked list
	while (currentBlock->next != NULL)
	{
		//Check if the size of the current block is equal or bigger than the requested size and check if the it is already in use
		if (currentBlock->size >= n && currentBlock->used == false)
		{
			//Printline for debugging			
			printf("Using an old free block\n");
			//It its not in use set used to true
			currentBlock->used = true;
			//return the blocks writable address
			return currentBlock->addr;
		}
		else {
			//If it is not free or the block data capacity is too small go to the next block
			currentBlock = currentBlock->next;
		}
	}
	return NULL;
}

void *malloc(size_t n)
{
	void *availableAddr = NULL;
	//If there is something in the list.
	if (head != NULL) {
		//Check if an old block can be used
		availableAddr = block_get(n);
	}
	//if an old usable block is not found
	if (availableAddr == NULL) {
		//Make a new block if the size is not 0
		if (n != 0) return block_alloc(n);
	}
	else {
		//else if an old reusable block is found return it's writable address
		return availableAddr;
	}
	return NULL;
}

void free(void *ptr)
{ 
	//If the pointer is not null call block_free with a pointer to the block's address the data belongs to.
	if(ptr != NULL) block_free((void*)ptr - sizeof(struct block));
	//Printline for debugging
	printf("Freeing a block\n");
}

void *realloc(void *ptr, size_t n)
{
	
}

void *calloc(size_t n, size_t m)
{
	/* TODO */
}


