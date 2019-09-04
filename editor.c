/*
 *
 * BertOS - Editor assignment
 * src/editor.c
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

#include <editor.h>
#include <ramfs.h>
#include <alloc.h>
#include "include/sys/vga.h"
#include "include/sys/print.h"
#include "include/sys/ps2.h"

//EDITOR
int max_x = 80;
int max_y = 25;
char text[2000];
char fileData[2000];
char status;
bool editorIsRunning = true;
int fileOpened;

//FILE INFO
char* currentEditedFileName;
struct file *currentEditedFile;



//KEYBOARD HANDLING
int enterKey = 10;
int backspace = 8;
int n = 0;
int x = 0;
int y = 0;
int offset = 2; //line to start typing at
int arrowLeft = 27;
int D = 68;
int C = 67;
int A = 65;
int O = 111;
int S = 83;
int P = 80;
char ctrl = '\001';
bool arrowAction = false;
bool ctrlAction = false;
bool enteringFileName = false;

// This funnction clears the screen and prints the new values
void updateScreen() {
	// Clear the whole screen to rewrite everything
	vga_clear();
	// Commented printf for debugging reasons
	// printf("status:%s,%d text:%s,%d", status, strlen(status), text, strlen(text));
	// Print the current status
	printf("%s", status);
	// Print the typed text
	printf("%s", text);

	//Handle the cursor in another function
	handleCursor(n);	
}

// This value places the cursor according to the en value (en = position in text array)
void handleCursor(int en){
	// Calculate the x value of the cursor (column) en is the position in the text array
	// The screen is 80 characters wide. en % 80 will always be between 0-79
	x = en % 80;
	// Calculate th y value of the cursor (row) en / 80 is cut of since its an integer.
	// This way it will return the row i.e. en=132 -> 132/80 = 1... 
	// the function will return the row 0-based. using an offset makes it start on the right row
	y = (en / 80) + offset;

	// Init cursor object
	struct vga_cursor cur = vga_curget();
	
	// Set the cursor fields
	cur.x = x;
	cur.y = y;

	// Print a row with information
	printf("\nCharacters:%i, Column:%i, Row:%i", strlen(text), x, y-1);

	// Set the cursor
	vga_curset(cur, false);
}

// This function handles all the keyboard input
char handleKeyboard(char key) {
	// If the incoming key is not any special character
	if (key != backspace && key != arrowLeft && !arrowAction && !ctrlAction && key != enterKey) {
		// If the cursor is not at the end of the text, place a character between the characters
		// and shift all characters to the right
		if (n < strlen(text)) {
			for (int i = strlen(text); i > n; --i) {
				text[i] = text[i-1];
			}
		}
		text[n] = key;
		n++;
		handleCursor(n);
		
	}
	else {
		if (key == enterKey) {
			if (n < strlen(text)) {
				for (int i = strlen(text); i > n; --i) {
					text[i] = text[i-1];
				}
			}
			text[n] = key;
			int addForNextLine = ((y-1) * 80) - n;
			n = n + addForNextLine;
			handleCursor(n);
		}
		if (key == backspace) {
			//backspace
			for (int i = n - 1; i < strlen(text); i++) {
				text[i] = text[i + 1];
			}
			n--;
			if(n < 0){n=0;}
			handleCursor(n);
			
		}
		if (key == arrowLeft) {
			arrowAction = true;
		}
		if (arrowAction) {
			if (key == D) {
				//left
				n--;
				if(n < 0){n=0;}
				handleCursor(n);
				arrowAction = false;
			}
			if (key == C) {
				//right
				n++;
				if(n > strlen(text)){n = strlen(text);}
				handleCursor(n);
				arrowAction = false;
			}
			if (key == A) {
				arrowAction = false;
			}
			if (key == O) {
				arrowAction = false;
				editorIsRunning = false;
				vga_clear();
				strcpy(status, "");
				// main();
			}
			if(key == P){
				arrowAction = false;
				for (int i = n; i < strlen(text); i++) {
					text[i] = text[i + 1];
				}
				// n--;
				if(n < 0){n=0;}
				handleCursor(n);
			}
			
		}
	}
	updateScreen();
	return key;
}

struct file *saveFile(char *fileName)
{
	currentEditedFile = ramfs_seek(fileName);
	if (currentEditedFile)
	{
		ramfs_write(currentEditedFile, fileData, (strlen(fileData) + 1));
		printf("%s", currentEditedFile->data);
	}
	else {
		ramfs_create(fileName);
		currentEditedFile = ramfs_seek(fileName);
		if (currentEditedFile) {
			free(currentEditedFile->data);
			ramfs_write(currentEditedFile, fileData, (strlen(fileData) + 1));
			// printf("%s \n", currentEditedFile->data);
			// printf("\nNew file added with name: %s", fileName);
			return currentEditedFile;
		}
		return NULL;
	}
}

int openFile(char *name) {
	currentEditedFile = ramfs_seek(name);
	if (currentEditedFile) {
		printf("File opened with name; %s", currentEditedFile->name);
		if (currentEditedFile->data) {
			strcpy(text, currentEditedFile->data);
			n = strlen(text);
			strcpy(status, "=======TEXT EDITOR====== \n\n");
			updateScreen();
			return 1;
		}
	}else{
		return 0;
	}
	
}

void editor(char *openName)
{
	//INITIALIZING
	vga_clear();
	strcpy(status, "=======TEXT EDITOR====== \n\n");
	editorIsRunning = true;

	if(strlen(openName)>0){
		int resp = openFile(openName);
		if(resp < 1){
			printf("File not opnened, openName len:%i", strlen(openName));
			editorIsRunning = false;
		}else{
			editorIsRunning = true;
		}
	}

	
	// openFile();

	//START THE MAIN THREAD OF THE PROGRAM
	while (editorIsRunning) {
		// strcpy(status, "=======TEXT EDITOR====== \n\n");
		updateScreen();
		char typed = handleKeyboard(ps2_getch());
		if (typed == 65) { // arrow up
			memset(typed, 0, sizeof(typed));
			strcpy(status, "Please enter a name for your file\n\n");
			vga_clear();
			printf("Please enter a name for your file\n\n");
			enteringFileName = true;
			strcpy(fileData, text);
			memset(text, 0, sizeof(text));
			strcpy(text, "");
			n = 0;
			while(enteringFileName){
				typed = handleKeyboard(ps2_getch());
				if (typed == enterKey) {
					memset(status, 0, sizeof(status));
					enteringFileName = false;
					text[strlen(text)-1] = 0;
					struct file *new = saveFile(text);
					if(new){
						// struct file *seekedFile = ramfs_seek(text);
						// strcpy(status, new->name);
						memset(text, 0, sizeof(text));
						// strcpy(text, "");
						// updateScreen();
						n = 0;
						editorIsRunning = false;
						vga_clear();
					}else{
						memset(text, 0, sizeof(text));
						n = 0;
						vga_clear();
						strcpy(status, "Error creating new file");
					}
					
					break;
				}
			}
		}		
	}
		
	
}


