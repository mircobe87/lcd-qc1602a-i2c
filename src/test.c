/*
 * test.c
 * This file is part of lcd-qc1602a-i2c
 *
 * Copyright (C) 2014 - Mirco Bertelli
 *
 * lcd-qc1602a-i2c is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * lcd-qc1602a-i2c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lcd-qc1602a-i2c. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "i2c_hd44780.h"
#include "a00_cgrom.h"

i2cDisplay_t mkFakeDisplay(int addr, int lines, rom_t romType){
	i2cDisplay_t display;
	display = malloc(sizeof(struct_i2cDispaly_t));
	
	display->addr           = addr;
	display->lines          = lines;
	display->romType        = romType;
	display->cursorAddr     = 0x00;
	display->blStatus       = ON;
	display->displayStatus  = ON;
	display->cursorStatus   = OFF;
	display->blinkStatus    = OFF;
	display->cursorDirShift = RIGHT;
	display->displayShift   = OFF;
	display->i2cBus         = 0;
	
	return display;
}

void printDisplay(i2cDisplay_t d){
	int i, max, c;
	printf("\n  display->addr           = 0x%02X\n",d->addr);
	printf("  display->lines          = %d\n",d->lines);
	printf("  display->romType        = %d\n",d->romType);
	printf("  display->cursorAddr     = 0x%02X\n",d->cursorAddr);
	printf("  display->blStatus       = 0x%02X\n",d->blStatus);
	printf("  display->displayStatus  = 0x%02X\n",d->displayStatus);
	printf("  display->cursorStatus   = 0x%02X\n",d->cursorStatus);
	printf("  display->blinkStatus    = 0x%02X\n",d->blinkStatus);
	printf("  display->cursorDirShift = 0x%02X\n",d->cursorDirShift);
	printf("  display->displayShift   = 0x%02X\n",d->displayShift);
	printf("  display->i2cBus         = %d\n",d->i2cBus);
	printf("  display->linesData      = [");
	for(i=0, max=80; i<max; i++){
		c = (d->linesData)[i];
		if(c == '\0')
			printf("0x00,");
		else
			printf("%c,", c);
	}
	printf("\b]\n\n");
}

int main(int argc, char *argv[]){
	i2cDisplay_t d;
	int i;

	d = getDisplay(0x27, 2, A00, "display.status");
	if (!d){
		perror("Nessun display creato");
	}
	printDisplay(d);
	printf("-----------------------------------------------\n");
	
	clear(d);
	
	cursorSwitch(d, ON);
	printf("cursor ON");
	printDisplay(d);
	sleep(1);
	
	cursorSwitch(d, OFF);
	printf("cursor OFF");
	printDisplay(d);
	sleep(1);
	
	displaySwitch(d, ON);
	printf("display ON");
	printDisplay(d);
	sleep(1);
	
	displaySwitch(d, OFF);
	printf("display OFF");
	printDisplay(d);
	sleep(1);
	
	blinkSwitch(d, ON);
	printf("blink ON");
	printDisplay(d);
	sleep(1);
	
	blinkSwitch(d, OFF);
	printf("blink OFF");
	printDisplay(d);
	sleep(1);
	
	displaySwitch(d, ON);
	cursorSwitch(d, ON);
	printf("display & cursor ON");
	printDisplay(d);
	
	lcdPutChar(d, 'A');
	printf("putChar A");
	printDisplay(d);
	sleep(1);
	
	lcdPutChar(d, '\n');
	printf("putChar \\n");
	printDisplay(d);
	sleep(1);
	
	lcdPutChar(d, 'B');
	printf("putChar B");
	printDisplay(d);
	sleep(1);
	
	lcdPutChar(d, '\n');
	printf("putChar \\n");
	printDisplay(d);
	sleep(1);
	
	clear(d);

	
	lcdPrintf(d, "Test Display\nI2C addr: 0x%02X", 0x27);
	printDisplay(d);
	
	sleep(10);
	cursorSwitch(d, OFF);
	
	sleep(1);
	blinkSwitch(d, ON);
	sleep(5);
	blinkSwitch(d, OFF);
	cursorSwitch(d, ON);
	sleep(1);
	cursorSwitch(d, OFF);
	blinkSwitch(d, ON);
	sleep(1);
	cursorSeek(d, L1_START, 0);
	sleep(1);
	cursorSeek(d, L1_END, -24);
	sleep(1);
	cursorSeek(d, L2_START, 0);
	sleep(1);
	cursorSeek(d, L2_END, -24);
	sleep(1);
	clear(d);
	blinkSwitch(d, OFF);
	cursorSwitch(d, ON);
	sleep(1);
	backlightSwitch(d, OFF);
	usleep(100000);
	
	backlightSwitch(d, ON);
	for(i=0; i<80; i++){
		shiftCursor(d, RIGHT);
		usleep(100000);
	}
	backlightSwitch(d, OFF);
	usleep(100000);
	backlightSwitch(d, ON);
	for(i=0; i<80; i++){
		shiftCursor(d, LEFT);
		usleep(100000);
	}
	backlightSwitch(d, OFF);
	usleep(100000);
	backlightSwitch(d, ON);
	usleep(100000);
	backlightSwitch(d, OFF);
	sleep(1);
	backlightSwitch(d, ON);
	
	clear(d);
	cursorSwitch(d, OFF);
	
	lcdPrintf(d,"Linea1");
	sleep(1);
	lcdPrintf(d,"\nLinea2");
	sleep(1);
	lcdPrintf(d,"\nLinea3");
	sleep(1);
	clear(d);
	
	lcdPrintf(d, "      The%c\n      %cEnd", A00_ARROW_R, A00_ARROW_L);
	printDisplay(d);
	
	return EXIT_SUCCESS;
}
