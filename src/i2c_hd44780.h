/**
 * @file i2c_hd44780.h
 * Una serie di primitive per la gestione del display LCD QC1602A via bus I2C.
 *
 * @author Mirco Bertelli
 * @date 07/10/2014
 * @copyright GNU Public License v3.
 *
 * i2c_hd44780.h
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



#ifndef __I2C_HD44780_H__
#define __I2C_HD44780_H__

#define I_LINE_MAX_CHAR 80 /**< Definisce il numero massimo di caratteri disponibili in modalità "1 linea" */
#define II_LINE_MAX_CHAR 40 /**< Definisce il numero massimo di caratteri disponibili in modalità "2 linee" */

#define I_LINE_BEGIN_ADDR 0x00 /**< Modalità "1 linea": indirizzo del primo carattere */
#define I_LINE_END_ADDR   0x4F /**< Modalità "1 linea": indirizzo dell'ultimo carattere */

#define II_LINE_BEGIN_1L_ADDR 0x00 /**< Modalità "2 linee": indirizzo del primo carattere della prima riga */
#define II_LINE_END_1L_ADDR   0x27 /**< Modalità "2 linee": indirizzo dell'ultimo carattere della prima riga */
#define II_LINE_BEGIN_2L_ADDR 0x40 /**< Modalità "2 linee": indirizzo del primo carattere della seconda riga */
#define II_LINE_END_2L_ADDR   0x67 /**< Modalità "2 linee": indirizzo dell'ultimo carattere della seconda riga */

/**
 * Il tipo di rom presente nel controller del display.
 *
 * - A00: la versione giapponese
 * - A02: la versione europea
 */
typedef enum {A00, A02} rom_t;

/**
 * Definisce i valori per i 2 tipo di ram presenti.
 */
typedef enum {DDRAM, CGRAM} ram_t;

/**
 * Definisce il tipo shiftDirection e i suoi valori possibili.
 */
typedef enum {LEFT, RIGHT} shiftDirection_t;

/**
 * Definisce il tipo switchStatus e i suoi valori possibili.
 */
typedef enum {OFF, ON} switchStatus_t;

/**
 * Definisce il tipo cursorRef_t e i suoi valori che indicano un riferimento
 * a partire dal quale si vuol spostare il cursore.
 *
 * - L1_START: prima posizione della prima riga;
 * - L1_END: ultima posizione della prima riga;
 * - L2_START: prima posizione della seconda riga;
 * - L2_END: ultima posizione della seconda riga;
 * - CURRENT: posizione corrente del cursore.
 */
typedef enum {L1_START, L1_END, L2_START, L2_END, CURRENT} cursorRef_t;

/**
 * La struttura che descrive il display
 */
typedef struct {
	int addr;                        /**< Indirizzo I2C del display */
	int lines;                       /**< Numero di linee utilizzare sul dispositivo */
	rom_t romType;                   /**< Tipo di ROM del dispositivo */
	int cursorAddr;                  /**< Posizione corrente del cursore */
	switchStatus_t blStatus;         /**< Stato corrente della retroilluminazione: acceso o spento */
	switchStatus_t displayStatus;    /**< Stato corrente del display: acceso o spento */
	switchStatus_t cursorStatus;     /**< Stato corrente del cursore: acceso o spento */
	switchStatus_t blinkStatus;      /**< Stato corrente del cursore lampeggiante: acceso o spento */
	shiftDirection_t cursorDirShift; /**< Indica la direzione di scorrimento del cursore */
	switchStatus_t displayShift;     /**< Indica se anche tutto il contenuto del display viene spostato */
	int i2cBus;                      /**< Descrittore del bus I2C */
} struct_i2cDispaly_t;

/**
 * Definisce il tipo display.
 */
typedef struct_i2cDispaly_t *i2cDisplay_t;

/**
 * Inizializza un nuovo display.
 *
 * @param addr Indirizzo I2C del display.
 * @param lines Numero di linee da usare sul display.
 * @param romType Tipo della ROM presente sul display.
 *
 * @return l'istanza di un nuovo display. Il nuovo display sarà attivo con la
 *         retroilluminazione accesa, il cursore sarà posizionato all'inizio ma
 *         non visibile (ne statico ne lampeggiante) e durante la scrittura si
 *         sposterà a destra metre il contenuto del display rimarrà fermo.
 */
i2cDisplay_t mkDisplay(int addr, int lines, rom_t romType);

/**
 * Controlla la retroilluminazione.
 *
 * @param d Il display da controllare
 * @param onOff OFF spegne la luce, ON la accende.
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int backlightSwitch(i2cDisplay_t d, switchStatus_t onOff);

/**
 * Ritorna lo stato della retroilluminazione del display.
 *
 * @param d Il display
 * @retval ON se la retroilluminazione è attiva
 * @retval OFF altrimenti (in caso di errore errno != 0)
 */
switchStatus_t getBacklightStatus(i2cDisplay_t d);

/**
 * Attiva o disattiva il display.
 *
 * Con il display disattivo non viene mostrato niente sopra l'LCD ma tutte le
 * scritture hanno effetto e saranno visibili alla prossima attivazione.
 *
 * @param d Il display
 * @param onOff OFF disattiva, ON attiva il display
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int diplaySwitch(i2cDisplay_t d, switchStatus_t onOff);

/**
 * Ritorna lo stato del display.
 *
 * @param d Il display
 * @retval ON se il display è attivo
 * @retval OFF altrimenti (in caso di errore errno != 0)
 */
switchStatus_t getDisplayStatus(i2cDisplay_t d);

/**
 * Attiva o disattiva il cursore sul display.
 *
 * Il cursore è un trattino orizzontale che si accende sotto ad un carattere
 * sul display. Con il cursore attivo si può vedere dove avverrà la prossima
 * scrittura ma nulla vieta di scrivere sul display senza mostrare il cursore.
 *
 * @param d Il display
 * @param onOff OFF disattiva, ON attiva il cursore
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int cursorSwitch(i2cDisplay_t d, switchStatus_t onOff);

/**
 * Ritorna lo stato del cursore del display.
 *
 * @param d Il display
 * @retval ON se il cursore è attivo
 * @retval OFF altrimenti (in caso di errore errno != 0)
 */
switchStatus_t getCursorStatus(i2cDisplay_t d);

/**
 * Attiva o disattiva il cursore lampeggiante sul display.
 *
 * Il cursore lampeggiante è un blocco che copre ad intermittenza un intero
 * carattere sul display. Con il cursore lampeggiante attivo si può vedere dove
 * avverrà la prossima scrittura ma nulla vieta di scrivere sul display senza
 * mostrarlo.
 *
 * @param d Il display
 * @param onOff OFF disattiva, ON attiva il cursore lampeggiante.
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int blinkSwitch(i2cDisplay_t d, switchStatus_t onOff);

/**
 * Ritorna lo stato del cursore lampeggiante del display.
 *
 * @param d Il display
 * @retval ON se il cursore lampeggiante è attivo
 * @retval OFF altrimenti (in caso di errore errno != 0)
 */
switchStatus_t getBlinkStatus(i2cDisplay_t d);

/**
 * Imposta il comportamento del display dopo ogni scrittura.
 *
 * Imposta in quale direzione far scorrere il cursore e se spostare anche tutto
 * il contenuto del display in quella direzione.
 *
 * @param d Il display
 * @param leftRight Direzione dello shift, LEFT o RIGHT.
 * @param onOff Se spostare anche il contenuto del display, ON o OFF.
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int setEntryMode(i2cDisplay_t d, shiftDirection_t leftRight, switchStatus_t onOff);

/**
 * Ritorna la direzione nella quale il display sposta il cursore durante la scrittura.
 *
 * @param d Il display.
 * @retval RIGHT se lo sposta a destra
 * @retval LEFT se lo sposta a sinistra o errore (setta errno)
 */
shiftDirection_t getCursorDir(i2cDisplay_t d);

/**
 * Ritorna se il contenuto del display viene spostato o meno durante la scrittura.
 * 
 * @param d Il display.
 * @retval ON se viene spostato
 * @retval OFF se non viene spostato o c'è errore (setta errno)
 */
switchStatus_t getDisplayShift(i2cDisplay_t d);

/**
 * Muove il cursore di una posizione.
 *
 * Sposta il cursore a destra o a sinistra di una posizione.
 *
 * @param d Il display.
 * @param leftRight la direzione di spostamento, LEFT o RIGHT
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int shiftCursor(i2cDisplay_t d, shiftDirection_t leftRight);

/**
 * Muove il contenuto del display di una posizione.
 *
 * Sposta tutto il contenuto a destra o a sinistra di una posizione.
 *
 * @param d Il display.
 * @param leftRight la direzione di spostamento, LEFT o RIGHT
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int shiftDisplay(i2cDisplay_t d, shiftDirection_t leftRight);

/**
 * Elimina il contenuto del display e riposta il cursore nella posizione iniziale.
 *
 * @param d Il display.
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int clear(i2cDisplay_t d);

/**
 * Sposta il cursore in maniera "random".
 *
 * Dato un punto di riferimento, il cursore viene mosso di un certo numeri di caratteri.
 *
 * @param d Il display.
 * @param ref Il punto di riferimento.
 * @param offset spostamento.
 * @retval 0 se tutto è andato bene;
 * @retval -1 in caso di errore (setta errno)
 */
int cursorSeek(i2cDisplay_t d, cursorRef_t ref, int offset);
#endif /* __I2C_HD44780_H__ */

