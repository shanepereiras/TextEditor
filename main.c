/*
 *
 * BertOS - I/O assignment
 * src/main.c
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

#include "include/sys/vga.h"
#include "include/sys/print.h"
#include "include/sys/ps2.h"
#include "include/sys/string.h"
#include "include/ramfs.h"

void main(void)
{
	/**
	 * Code to execute assignment 1
	*/
	// char *addr1 = (char*)malloc(6 * sizeof(char));
	// char *addr2 = (char*)malloc(8 * sizeof(char));
	// char *addr3 = (char*)malloc(13 * sizeof(char));
	// char *addr4 = (char*)malloc(11 * sizeof(char));
	// char *addr5 = (char*)malloc(12 * sizeof(char));

	// strcpy(addr1,"Shane");
	// strcpy(addr2,"pereira");
	// strcpy(addr3,"ShanePereira");
	// strcpy(addr4,"0123456789");
	// strcpy(addr5,"98765432101");

	// printf("Address 1: %p, with value: %s \n", addr1, addr1);
	// printf("Address 2: %p, with value: %s \n", addr2, addr2);
	// printf("Address 3: %p, with value: %s \n", addr3, addr3);
	// printf("Address 4: %p, with value: %s \n", addr4, addr4);
	// printf("Address 5: %p, with value: %s \n", addr5, addr5);

	// free(addr2);

	// addr2 = (char*)malloc(9 * sizeof(char));
	// char *addr6 = (char*)malloc(8 * sizeof(char));
	// printf("Address 6: %p, with value: %s \n", addr6, addr6);

	/**
	 * Code to execute assignment 2
	*/

	struct vga_cursor currentCurs;

	bool running = true;
	bool pausePrinting = false;
	char key;
	char command[300];
	int cliIsRunning = 1;
	int enterKey = 10;
	int backspace = 8;
	int n = 0;
	int arrowLeft = 27;
	int D = 68;
	int C = 67;
	
	vga_clear();
	printf("Type help to show a list of commands\n");
	key = ps2_getch();

	while(running){
		while(key != enterKey){
			
			currentCurs = vga_curget();
			if(pausePrinting){
				if(key == D){
					key = "";
					currentCurs.x = currentCurs.x - 1;
					vga_curset(currentCurs,false);	
					pausePrinting = false;
				}
				if(key == C){
					key = "";
					currentCurs.x = currentCurs.x + 1;
					vga_curset(currentCurs,false);	
					pausePrinting = false;
				}
			}
			else if(key == backspace){
				//shift all characters to the right of the cursor 1 to the left
				printf("\b");
				// command[n] = NULL;
				// n--;

			}else if(key == arrowLeft){
				pausePrinting = true;
			}
			else if(key != NULL){
				command[n] = key;
				n++;
				vga_clear();
				printf("%s", command);
			}
			
			key = ps2_getch();
		}

		key = 0;

		if(command != NULL && cliIsRunning == 1){

				if(strcmp(command, "-clear") == 0){
					vga_clear();
				}else if(strcmp(command, "-quit") == 0){
					printf("\nCLI has quit\n");
					running = false;
				}else if (strcmp(command, "-editor") == 0) {
					printf("starting editor");
					// running = false;
					editor("");
				}else if(checkCommand(command, "-open")){
					const char s[2] = " ";
					char *token;

					token = strtok(command, s);
					token = strtok(NULL, s);
					// running = false;
					if(ramfs_seek(token)){
						editor(token);
					}else{
						printf("\nfile not found");
					}
					
				}else if(strcmp(command, "-help") == 0){
					vga_clear();
					printf("The following commands can be used:\nclear - Clears the whole interface\nquit - Quits the CLI and will stop listening for commands\necho [string] - Echo's the given string\n");
				}else if(checkCommand(command, "echo")){

					const char s[2] = " ";
					char *token;

					token = strtok(command, s);
					token = strtok(NULL, s);
					printf( " \n%s\n", token );

				}else if(checkCommand(command, "-touch")){
					const char s[2] = " ";
					char *token;

					token = strtok(command, s);
					token = strtok(NULL, s);
		
					int responseCode = ramfs_create(token);

					if(responseCode == 0){
						printf("\nNew file added with name: %s", token);
					}else{
						printf("\nResponsecode: %d", responseCode);
					}
					printf("\n");
				}else if(checkCommand(command, "-cat")){
					const char s[2] = " ";
					char *token;

					token = strtok(command, s);
					token = strtok(NULL, s);
					printf("\nSeeking for name: %s", token);
					struct file *found = ramfs_seek(token);
					if(found){
						printf("\nFile found...", found->name);
						printf("\nName: %s\n", found->name);
						printf("Data: %s", found->data);
					}else{
						printf("\nFile with name %s not found", token);
					}
					printf("\n");	
				}else if(checkCommand(command, "-write")){
					const char s[2] = " ";
					char *token;

					token = strtok(command, s);
					token = strtok(NULL, s);
					struct file *fileToWrite = ramfs_seek(token);
					if(fileToWrite){
						printf("\nFile found with name: %s, looked for: %s", fileToWrite->name, token);
						token = strtok(NULL, s);
						char *data = "Some Test Data ahsjdbashjdsbsadbsajhdbasjhsdbsahj";
						printf("\nSize of data: %d", strlen(data) + 1);
						ssize_t bytesWritten = ramfs_write(fileToWrite, data, (strlen(data) + 1));
						printf("\nBytes written: %d", bytesWritten);
						printf("\n %s", fileToWrite->data);
					}else{
						printf("\nFile with name: %s not found", token);
					}
				
				}else if (checkCommand(command, "-remove")) {
					const char s[2] = " ";
					char *token;

					token = strtok(command, s);
					token = strtok(NULL, s);
					struct file *fileToRemove = ramfs_seek(token);
					if(fileToRemove){
						ramfs_remove(fileToRemove);
					}else{
						printf("\nFile not found\n");
					}
					
				}else if(strcmp(command, "-ls") == 0){
					struct file *headFile = ramfs_readdir();
					if(headFile){
						printf("\nfile name: %s", headFile->name);
						while(headFile->next){
							headFile = headFile->next;
							printf("\nfile name: %s", headFile->name);
						}
					}
					printf("\n");
				}
				else if (checkCommand(command, "-write")) {
					const char s[2] = " ";
					char *token;

					token = strtok(command, s);
					token = strtok(NULL, s);
					struct file *fileToWrite = ramfs_seek(token);
				}
				else if (checkCommand(command, "-read")) {
					const char s[2] = " ";
					char *token;

					token = strtok(command, s);
					token = strtok(NULL, s);
					char* readDataTo;
					ramfs_read(ramfs_seek(token), readDataTo);
				}
				else{
					printf("\nCommand was not recognized.\n");
				}	
			memset(command, 0, sizeof(command));
			n=0;
		}
		else {

		}
	}	
}