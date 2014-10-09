/**
 * @file i2c_hd44780.c
 * Implementazione dell'header i2c_hd44780.h
 *
 * @author Mirco Bertelli
 * @date 07/10/2014
 * @copyright GNU Public License v3
 *
 * i2c_hd44780.c
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



#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include "i2c_hd44780.h"

#define CMD_WRITE_E1 0x04 /* nibble inferiore per scrittura comando (enable = 1) */
#define CMD_WRITE_E0 0x00 /* nibble inferiore per scrittura comando (enable = 0) */

#define DATA_WRITE_E1 0x05 /* nibble inferiore per scrittura dati (enable = 1) */
#define DATA_WRITE_E0 0x01 /* nibble inferiore per scrittura dati (enable = 0) */

/*----------------- Funzioni Ausiliarie -------------------*/

/**
 * Construisce e invia un comando al display.
 *
 * @param d Il display.
 * @param hNibble1 prima parte del comando da inviare al display.
 * @param hNibble2 seconda parte del comando da mandare al display.
 * @retval <0 in caso di errore (setta errno)
 */
static int makeAndSendCommand(i2cDisplay_t d, int hNibble1, int hNibble2);

/**
 * Restituisce la posizione attuale del cursore.
 *
 * @param d Il display
 */
static int getCursorAddr(i2cDisplay_t d);

/**
 * Setta una nuova posizione del cursore nella struttura che rappresente il display.
 *
 * @param d Il display
 * @param nuova posizione del cursore.
 * @return il nuovo indirizzo del cursore normalizzato.
 */
static int setCursorAddr(i2cDisplay_t d, int addr);

/**
 * Ritorna il numero di linee che il display utilizza.
 *
 * @param d Il display.
 * @return Il numero di linee
 */
static int getLinesNum(i2cDisplay_t d);

/**
 * Normalizza un indirzzo per il cursore a seconda del numero di linee utilizzate
 * dal display.
 *
 * @param dispLines linee utilizzate
 * @param addr indirizzo da normalizzare
 *
 * @return un indirizzo normalizzato:
 *  - se si lavora con una riga l'indirizzo è normalizzato mantenedono sempre
 *    entro l'intervallo 0x00-I_LINE_MAX_CHAR;
 *  - se si lavora con 2 righe, ogni indirizzo compreso tra II_LINE_END_1L_ADDR
 *    e II_LINE_BEGIN_2L_ADDR vie trasformato in II_LINE_BEGIN_2L_ADDR, mentre
 *    ogni indirizzo maggiore di II_LINE_END_2L_ADDR viene trasformato in
 *    II_LINE_BEGIN_1L_ADDR.
 */
static int normalizeCursorAddr(int dispLines, int addr);

/**
 * Incrementa o decrementa di un numero arbitrario un indirizzo DDRAM con le
 * regole che dipendono se il display utilizza 1 o 2 righe.
 * 
 * @param nLines numero di righe utilizzare dal display
 * @param DDRAMaddr indirizzo da incrementare o decrementare
 * @param offset incremento (o decremento se è negativo)
 * 
 * @return l'indirizzo spostato:
 *  - se si usa una linea sola il nuovo indirizzo sarà semplicemente
 *    incrementato (o decrementato) tutto in modulo I_LINE_MAX_CHAR (che è
 *    la capacità della riga);
 *  - se si usano 2 righe il comportamento è quello che ci si aspetta, ovvero
 *    andare oltre la fine (prima dell'inizio) di una riga ci porta all'inizio
 *    (alla fine) dell'altra.
 */
static int shiftDDRAMaddr(int nLines, int DDRAMaddr, int offset);

/**
 * Salva lo stato del display su file.
 * Il nome del file usato è quello memorizzato nella variabile globale DSFileName.
 * 
 * @param d il display.
 */
static void saveDisplayState(i2cDisplay_t d);

/* ------------ Variabili globali --------------------------------------------*/
/**
 * Memorizza il nome del file che ricorda lo stato del display
 */
static const char* DSFileName;
/*------------- Implementazione libreria -------------------------------------*/

i2cDisplay_t getDisplay(int addr, int lines, rom_t romType, const char *displayFileName){
	FILE * fDisplay;
	size_t n;
	i2cDisplay_t display;
	int i2cBus;
	errno = 0;
	
	if (access(displayFileName, F_OK) <= 0) {
		/* il file di stato esiste */
		fDisplay = fopen(displayFileName, "rb");
		if (fDisplay){
			display = malloc(sizeof(struct_i2cDispaly_t));
			if (display){
				n = fread(display, 1, sizeof(struct_i2cDispaly_t), fDisplay);
				fclose(fDisplay);
				if (n != 1) {
					errno = EIO;
					free(display);
					display = NULL;
				}else{
					if (display->addr != addr) {
						errno = EINVAL;
						free(display);
						display = NULL;
					}else{
						DSFileName = displayFileName;
					}
				}
			}else{
				errno = ENOMEM;
				fclose(fDisplay);
				display = NULL;
			}
		}else{
			/* errno settato da fopen() */
			display = NULL;
		}
	}else{
		/* il file di stato NON esiste */
		fDisplay = fopen(displayFileName, "wb");
		if (fDisplay) {
			display = malloc(sizeof(struct_i2cDispaly_t));
			if (display) {
				if ( addr >= 0x03 && addr <= 0x77 ){
					if(lines > 0 && lines < 3){
						if( (i2cBus = wiringPiI2CSetup(addr)) >= 0 ){
							/* set nibble comunication */
							wiringPiI2CWrite(i2cBus, 0x2C);
							wiringPiI2CWrite(i2cBus, 0x28);
							
							/* set 1 o 2 lines and 5x8 charset */
							wiringPiI2CWrite(i2cBus, 0x2C);
							wiringPiI2CWrite(i2cBus, 0x28);
							if(lines == 1){
								wiringPiI2CWrite(i2cBus, 0x0C);
								wiringPiI2CWrite(i2cBus, 0x08);
							}else{
								wiringPiI2CWrite(i2cBus, 0x8C);
								wiringPiI2CWrite(i2cBus, 0x88);
							}
							
							/* entry mode: shift R and no display shifting */
							wiringPiI2CWrite(i2cBus, 0x0C);
							wiringPiI2CWrite(i2cBus, 0x08);
							wiringPiI2CWrite(i2cBus, 0x6C);
							wiringPiI2CWrite(i2cBus, 0x68);
							
							/* display ON and all cursors OFF */
							wiringPiI2CWrite(i2cBus, 0x0C);
							wiringPiI2CWrite(i2cBus, 0x08);
							wiringPiI2CWrite(i2cBus, 0xCC);
							wiringPiI2CWrite(i2cBus, 0xC8);
							
							/* clear display */
							wiringPiI2CWrite(i2cBus, 0x0C);
							wiringPiI2CWrite(i2cBus, 0x08);
							wiringPiI2CWrite(i2cBus, 0x1C);
							wiringPiI2CWrite(i2cBus, 0x18);
							
							/* backlight ON */
							wiringPiI2CWrite(i2cBus, 0x08);
							
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
							display->i2cBus         = i2cBus;
							
							if (fwrite(display, 1, sizeof(struct_i2cDispaly_t), fDisplay) != 1){
								free(display);
								fclose(fDisplay);
								remove(displayFileName);
								/* errno settata da fwrite() */
								display = NULL;
							}else{
								DSFileName = displayFileName;
							}
						}else{
							free(display);
							fclose(fDisplay);
							remove(displayFileName);
							/* errno settata da wiringPiI2CSetup() */
							display = NULL;
						}
					}else{
						free(display);
						fclose(fDisplay);
						remove(displayFileName);
						errno = EINVAL;
						display = NULL;
					}
				}else{
					free(display);
					fclose(fDisplay);
					remove(displayFileName);
					errno = EADDRNOTAVAIL;
					display = NULL;
				}
			}else{
				errno = ENOMEM;
				fclose(fDisplay);
				remove(displayFileName);
				display = NULL;
			}
		}else{
			/* errno settato da fopen() */
			display = NULL;
		}
	}
	return display;
}

int backlightSwitch(i2cDisplay_t d, switchStatus_t onOff){
	int retval;
	errno = 0;
	retval = 0;
	
	if(d){
		if(getBacklightStatus(d) != onOff){
			switch (onOff){
				case ON: {
					retval = wiringPiI2CWrite(d->i2cBus, 0x08);
				}break;
				case OFF: {
					retval = wiringPiI2CWrite(d->i2cBus, 0x00);
				}break;
			}
			if(retval >= 0)
				d->blStatus = onOff;
		}
		saveDisplayState(d);
		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}

switchStatus_t getBacklightStatus(i2cDisplay_t d){
	errno = 0;
	if(d){
		return d->blStatus;
	}else{
		errno = EINVAL;
		return OFF;
	}
}

int diplaySwitch(i2cDisplay_t d, switchStatus_t onOff){
	switchStatus_t ds, cs, bs;
	int hnibble1, hnibble2;
	int retval;
	
	errno = 0;
	retval = 0;
	
	if(d){
		ds = getDisplayStatus(d);
		cs = getCursorStatus(d);
		bs = getBlinkStatus(d);
		
		hnibble1 = 0x0;
		hnibble2 = 1 << 3 | onOff << 2 | cs << 1 | bs;
		
		if(ds != onOff)
			if ( (retval = makeAndSendCommand(d, hnibble1, hnibble2)) >= 0){
				d->displayStatus = onOff;
				saveDisplayState(d);
			}

		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}

switchStatus_t getDisplayStatus(i2cDisplay_t d){
	errno = 0;
	if(d){
		return d->displayStatus;
	}else{
		errno = EINVAL;
		return OFF;
	}
}

int cursorSwitch(i2cDisplay_t d, switchStatus_t onOff){
	switchStatus_t ds, cs, bs;
	int hnibble1, hnibble2;
	int retval;
	
	errno = 0;
	retval = 0;
	
	if(d){
		ds = getDisplayStatus(d);
		cs = getCursorStatus(d);
		bs = getBlinkStatus(d);
		
		hnibble1 = 0x0;
		hnibble2 = 1 << 3 | ds << 2 | onOff << 1 | bs;
		
		if(cs != onOff)
			if ( (retval = makeAndSendCommand(d, hnibble1, hnibble2)) >= 0){
				d->cursorStatus = onOff;
				saveDisplayState(d);
			}

		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}

switchStatus_t getCursorStatus(i2cDisplay_t d){
	errno = 0;
	if(d){
		return d->cursorStatus;
	}else{
		errno = EINVAL;
		return OFF;
	}
}

int blinkSwitch(i2cDisplay_t d, switchStatus_t onOff){
	switchStatus_t ds, cs, bs;
	int hnibble1, hnibble2;
	int retval;
	
	errno = 0;
	retval = 0;
	
	if(d){
		ds = getDisplayStatus(d);
		cs = getCursorStatus(d);
		bs = getBlinkStatus(d);

		hnibble1 = 0x0;
		hnibble2 = 1 << 3 | ds << 2 | cs << 1 | onOff;

		if(bs != onOff)
			if ( (retval = makeAndSendCommand(d, hnibble1, hnibble2)) >= 0){
				d->blinkStatus = onOff;
				saveDisplayState(d);
			}

		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}

switchStatus_t getBlinkStatus(i2cDisplay_t d){
	errno = 0;
	if(d){
		return d->blinkStatus;
	}else{
		errno = EINVAL;
		return OFF;
	}
}

int setEntryMode(i2cDisplay_t d, shiftDirection_t leftRight, switchStatus_t onOff){
	switchStatus_t ds;
	shiftDirection_t cd;
	int hnibble1, hnibble2;
	int retval;
	
	errno = 0;
	retval = 0;
	
	if(d){
		ds = getDisplayShift(d);
		cd = getCursorDir(d);

		hnibble1 = 0x0;
		hnibble2 = 1 << 2 | leftRight << 1 | onOff;
		if(cd != leftRight || ds != onOff)
			if ( (retval = makeAndSendCommand(d, hnibble1, hnibble2)) >= 0){
				d->cursorDirShift = onOff;
				d->displayShift = leftRight;
				saveDisplayState(d);
			}
			
		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}

shiftDirection_t getCursorDir(i2cDisplay_t d){
	errno = 0;
	if(d){
		return d->cursorDirShift;
	}else{
		errno = EINVAL;
	}
	return LEFT;
}

switchStatus_t getDisplayShift(i2cDisplay_t d){
	errno = 0;
	if(d){
		return d->displayShift;
	}else{
		errno = EINVAL;
	}
	return OFF;
}

int shiftCursor(i2cDisplay_t d, shiftDirection_t leftRight){
	int hnibble1, hnibble2, retval;
	
	retval = 0;
	errno = 0;
	hnibble1 = 0x1;
	hnibble2 = leftRight << 2;
	
	if(d){
		if ( (retval = makeAndSendCommand(d, hnibble1, hnibble2)) >= 0){
			if (leftRight == LEFT)
				setCursorAddr(d, shiftDDRAMaddr(getLinesNum(d), getCursorAddr(d),-1));
			else
				setCursorAddr(d, shiftDDRAMaddr(getLinesNum(d), getCursorAddr(d),1));
			
			saveDisplayState(d);
		}
		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}

int shiftDisplay(i2cDisplay_t d, shiftDirection_t leftRight){
	int hnibble1, hnibble2, retval;
	
	errno = 0;
	hnibble1 = 0x1;
	hnibble2 = 1 << 3 | leftRight << 2;
	
	if(d){
		if ( (retval = makeAndSendCommand(d, hnibble1, hnibble2)) >= 0){
			saveDisplayState(d);
		}
		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}

int clear(i2cDisplay_t d){
	int hnibble1, hnibble2, retval;
	
	retval = 0;
	errno = 0;
	hnibble1 = 0x0;
	hnibble2 = 0x1;
	
	if (d){
		if ( (retval = makeAndSendCommand(d, hnibble1, hnibble2)) >= 0){
			setCursorAddr(d, 0);
			saveDisplayState(d);
		}

		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}

int cursorSeek(i2cDisplay_t d, cursorRef_t ref, int offset){
	int absAddr, retval, hnibble1, hnibble2, nLines;
	
	retval = 0;
	errno = 0;
	
	if(d){
		nLines = getLinesNum(d);
		switch(ref){
			case L1_START :{
				if (nLines == 1 )
					absAddr = shiftDDRAMaddr(nLines, I_LINE_BEGIN_ADDR, offset);
				else
					absAddr = shiftDDRAMaddr(nLines, II_LINE_BEGIN_1L_ADDR, offset);
			}break;
			case L1_END :{
				if (nLines == 1 )
					absAddr = shiftDDRAMaddr(nLines, I_LINE_END_ADDR, offset);
				else
					absAddr = shiftDDRAMaddr(nLines, II_LINE_END_1L_ADDR, offset);
			}break;
			case L2_START :{
				if (nLines == 1)
					absAddr = shiftDDRAMaddr(nLines, I_LINE_BEGIN_ADDR, offset);
				else
					absAddr = shiftDDRAMaddr(nLines, II_LINE_BEGIN_2L_ADDR, offset);
			}break;
			case L2_END :{
				if (nLines == 1)
					absAddr = shiftDDRAMaddr(nLines, I_LINE_END_ADDR, offset);
				else
					absAddr = shiftDDRAMaddr(nLines, II_LINE_END_2L_ADDR, offset);
			}break;
			case CURRENT :{
				absAddr = shiftDDRAMaddr(nLines, getCursorAddr(d), offset);
			}break;
		}
		hnibble1 = absAddr >> 4 | 1 << 3;
		hnibble2 = absAddr & 0xf;
		
		if ( (retval = makeAndSendCommand(d, hnibble1, hnibble2)) <= 0){
			setCursorAddr(d, absAddr);
			saveDisplayState(d);
		}
		return retval;
	}else{
		saveDisplayState(d);
		errno = EINVAL;
		return -1;
	}
}




/*------------------ Implementazione funzioni ausiliarie ---------------------*/

static int makeAndSendCommand(i2cDisplay_t d, int hNibble1, int hNibble2){
	int byte[4], i, max, retval;
	int bl;
	
	bl = getBacklightStatus(d);
	i = 0;
	max = 4;
	byte[0] = (hNibble1 << 4) | (CMD_WRITE_E1 | bl << 3);
	byte[1] = (hNibble1 << 4) | (CMD_WRITE_E0 | bl << 3);
	byte[2] = (hNibble2 << 4) | (CMD_WRITE_E1 | bl << 3);
	byte[3] = (hNibble2 << 4) | (CMD_WRITE_E0 | bl << 3);
	retval = 0;
	/*
	for(i=0; i<max; i++){
		printf("send byte[%d] = 0x%02X\n", i, byte[i]);
	}
	*/
	i=0;
	while( (retval = wiringPiI2CWrite(d->i2cBus, byte[i])) >= 0 ){
		i++;
		if( i == max ) break;
	}
	
	return retval;
}

static int getCursorAddr(i2cDisplay_t d){
	return d->cursorAddr;
}

static int setCursorAddr(i2cDisplay_t d, int addr){
	int l, normAddr;
	
	l = getLinesNum(d);
	normAddr = normalizeCursorAddr(l, addr);
	d->cursorAddr = normAddr;
	return normAddr;
}

static int shiftDDRAMaddr(int nLines, int DDRAMaddr, int offset){
	int newAddr;
	
	if(nLines == 1){
		newAddr = (DDRAMaddr + offset) % I_LINE_MAX_CHAR;
		if (newAddr < 0 )
			newAddr += I_LINE_MAX_CHAR;
	}else{
		offset = offset % (2*II_LINE_MAX_CHAR);
		
		if (II_LINE_BEGIN_1L_ADDR <= DDRAMaddr && DDRAMaddr <= II_LINE_END_1L_ADDR){
			/* siamo nella prima riga */
			if (DDRAMaddr + offset > II_LINE_END_1L_ADDR)
				newAddr = II_LINE_BEGIN_2L_ADDR-1 + ((DDRAMaddr + offset) - II_LINE_END_1L_ADDR);
			else if (DDRAMaddr + offset < II_LINE_BEGIN_1L_ADDR)
				newAddr = II_LINE_END_2L_ADDR+1 - (II_LINE_BEGIN_1L_ADDR - (DDRAMaddr + offset));
			else
				newAddr = DDRAMaddr + offset;
		}else{
			/* siamo nella seconda riga */
			if (DDRAMaddr + offset > II_LINE_END_2L_ADDR)
				newAddr = II_LINE_BEGIN_1L_ADDR-1 + ((DDRAMaddr + offset) - II_LINE_END_2L_ADDR);
			else if (DDRAMaddr + offset < II_LINE_BEGIN_2L_ADDR)
				newAddr = II_LINE_END_1L_ADDR+1 - (II_LINE_BEGIN_2L_ADDR - (DDRAMaddr + offset));
			else
				newAddr = DDRAMaddr + offset;
		}
	}
	return newAddr;
}

static int normalizeCursorAddr(int dispLines, int addr){
	int normAddr;
	
	if (dispLines == 1){
		normAddr = addr % I_LINE_MAX_CHAR;
		if (normAddr < 0)
			normAddr += I_LINE_MAX_CHAR;
	}else{
		normAddr = addr % 0xFF;
		if (normAddr < 0)
			normAddr += 0xFF;
		if (II_LINE_END_1L_ADDR < normAddr && normAddr < II_LINE_BEGIN_2L_ADDR)
			normAddr = II_LINE_BEGIN_2L_ADDR;
		else if (II_LINE_END_2L_ADDR < normAddr)
			normAddr = II_LINE_BEGIN_1L_ADDR;
	}
	return normAddr;
}

static int getLinesNum(i2cDisplay_t d){
	return d->lines;
}

static void saveDisplayState(i2cDisplay_t d){
	FILE * fout;
	
	fout = fopen(DSFileName, "wb");
	fwrite(d, 1, sizeof(struct_i2cDispaly_t), fout);
	fclose(fout);
}
