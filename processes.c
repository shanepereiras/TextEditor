/*
 *
 * BertOS - Process assignment
 * src/processes.c
 *
 * Copyright (C) 2019 Robin Kruit <0936014@hr.nl>
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



#include <sys/print.h>

#include <processes.h>

const char state_starting = 's';
const char state_ready = 'r';
const char state_running = 'e';
const char state_wait = 'w';

unsigned int process_id = 1;
struct process *head;
/*
 * This function should fill a new instance of the process struct
 */
int add_process(char *name, int (*func)(void))
{
	struct process *newProcess;
	if (!head) {
		
		strcpy(newProcess->name, name);
		newProcess->id = process_id;
		strcpy(newProcess->state, 'r');
		newProcess->func = func;
		head = newProcess;
		return 0;
	}
	//If the head is not empty
	else {
		newProcess = head;
		while (newProcess->nextProcess) {
			newProcess = newProcess->nextProcess;
		}
		newProcess->nextProcess->id = process_id;
		process_id++;
		strcpy(newProcess->nextProcess->name, name);
		strcpy(newProcess->nextProcess->state, state_ready);
		newProcess->nextProcess->func = func;
		return 0;
	}
	return 1;
}

/*
 * This function should pause a process, causing the scheduler to skip it
 */
int suspend_process(int id)
{
	struct process *currentProcess = head;
	while (currentProcess->nextProcess) {
		if (strcmp(currentProcess->state, state_wait) != 0 && currentProcess->id == id) {
			strcpy(currentProcess->state, state_wait);
		}
		currentProcess = currentProcess->nextProcess;
	}
}

/*
 * This function should resume a paused process
 */
int resume_process(int id)
{
	struct process *currentProcess = head;
	while (currentProcess->nextProcess) {
		if (strcmp(currentProcess->state, state_wait) == 0 && currentProcess->id == id) {
			strcpy(currentProcess->state, state_ready);
		}
		currentProcess = currentProcess->nextProcess;
	}
}

/*
 * This function should remove a process from the table and pack the table
 */
int kill_process(int id)
{
	struct process *currentProcess = head;
	struct process *prevProcess;
	while (currentProcess->nextProcess) {
		if (currentProcess->id == id) {
			if (prevProcess) {
				prevProcess->nextProcess = currentProcess->nextProcess;
				currentProcess->id = 0;
				currentProcess->state = 0;
				currentProcess->nextProcess = 0;
				currentProcess->func = 0;
				currentProcess = 0;
			}
		}
		prevProcess = currentProcess;
		currentProcess = currentProcess->nextProcess;
	}
}

/*
 * This function should check each process’ state and act appropiately.
 * Processes can be killed here, after which the process table should be packed.
 */
void do_round(void)
{
	struct process *currentProcess = head;
	while (currentProcess->nextProcess) {
		if (strcmp(currentProcess->state, state_ready) == 0) {
			strcpy(currentProcess->state, state_running);
		}
		else if (strcmp(currentProcess->state, state_starting) == 0) {
			strcpy(currentProcess->state, state_ready);
		}
		else if (strcmp(currentProcess->state, state_running) == 0) {
			currentProcess->func();
		}
		else if (strcmp(currentProcess->state, state_wait) == 0) {
			//Wait for dependent event to occur
		}


		currentProcess = currentProcess->nextProcess;
	}
}
