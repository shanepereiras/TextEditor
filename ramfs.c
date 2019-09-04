/*
 *
 * BertOS - File System assignment
 * src/ramfs.c
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

#include <ramfs.h>
#include <alloc.h>

static struct file *head = NULL;

/* Seek for files by 'name' */
struct file *ramfs_seek(const char *name)
{
	/* TODO */
	struct file *currentFile = head;
	if (strcmp(name, currentFile->name) == 0)
	{
		return currentFile;
	}
	while (currentFile->next)
	{
		currentFile = currentFile->next;
		if (strcmp(name, currentFile->name) == 0)
		{
			return currentFile;
		}
		
	}
	return NULL;
}

/* Return the head of the file list */
struct file *ramfs_readdir(void)
{
	return head;
}

/*
 * Read from a file into 'data' (which is automatically (re)allocated)
 * Return:
 *  # of bytes read
 * -1 when out of memory
 */
ssize_t ramfs_read(struct file *fp, char **data)
{	
	*data = &fp->data;
	return strlen(fp->data) + 1;
}

/*
 * Write 'n' bytes to a file from 'data'
 * Return:
 *  # of bytes written
 * -1 when out of memory
 */
ssize_t ramfs_write(struct file *fp, const char *data, size_t n)
{
	void *dataAddress = malloc(sizeof(data));
	if (dataAddress) {
		fp->data = dataAddress;
		strcpy(fp->data, data);
		fp->size = sizeof(struct file) + strlen(data)+1;
		return strlen(data) + 1;
	}
	else
	{
		return -1;
	}
}
/*
 * Initialize a file object
 * Return:
 * 	File object 
 */
int ramfs_create(const char *name)
{
	if (!name || sizeof(name) > NAME_MAX) {
		return -3;
	}
	else if (name == NULL) {
		return -2;
	}
	else {
		struct file *newFile = malloc(sizeof(struct file));
		if (!newFile) {
			return -1;
		}
		else {
			if (!head) {
				strcpy(newFile->name, name);
				newFile->size = sizeof(struct file);
				head = newFile;
			}
			else {
				//struct file *prevFile;
				struct file *currentFile = head;
				while (currentFile->next)
				{
					//prevFile = currentFile;
					currentFile = currentFile->next;
				}
				strcpy(newFile->name, name);
				currentFile->next = newFile;
				newFile->prev = currentFile;
				newFile->size = sizeof(struct file);
			}
			return 0;
		}
	}
}


/* Remove a file and it's data */
void ramfs_remove(struct file *fp)
{
	fp->next->prev = fp->prev;
	fp->prev->next = fp->next;
	free(fp);
	free(fp->data);
}
