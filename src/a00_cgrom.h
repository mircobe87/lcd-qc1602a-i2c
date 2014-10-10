/**
 * @file a00_cgrom.h
 * Definisce le funzioni per convertire un carattere UTF-8 in un codice carattere
 * per il display con CGROM A00.
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

#ifndef __A00_CGROM_H__
#define __A00_CGROM_H__

#define A00_YEN        0x5A /* '¥' */
#define A00_ARROW_R    0x7E /* '→' */
#define A00_ARROW_L    0x7F /* '←' */
#define A00_DEGREE     0xDF /* '°' */
#define A00_ALPHA      0xE0 /* 'α' */
#define A00_2DOT_A     0xE1 /* 'ä' */
#define A00_BETA       0xE2 /* 'β' */
#define A00_EPSILON    0xE3 /* 'ε' */
#define A00_MU         0xE4 /* 'μ' */
#define A00_SIGMA      0xE5 /* 'σ' */
#define A00_RHO        0xE6 /* 'ρ' */
#define A00_LONG_G     0xE7 /* 'ǥ' */
#define A00_SQRT       0xE8 /* '√' */
#define A00_RECIPROCAL 0xE9 /* '¯' */
#define A00_IMAGINARY  0xEA /* 'ⅉ' */
#define A00_SUP_X      0xEB /* '˟' */
#define A00_CENT       0xEC /* '¢' */
#define A00_LIRA       0xEE /* '£' */
#define A00_N_TILDE    0xEF /* 'ñ' */
#define A00_2DOT_O     0xF0 /* 'ö' */
#define A00_LONG_P     0xF1 /* 'Ƿ' */
#define A00_LONG_Q     0xF2 /* 'Ǫ' */
#define A00_THETA      0xF3 /* 'θ' */
#define A00_INFINITY   0xF4 /* '∞' */
#define A00_OMEGA      0xF5 /* 'Ω' */
#define A00_2DOT_U     0xF6 /* 'ü' */
#define A00_SSIGMA     0xF7 /* 'Σ' */
#define A00_PI         0xF8 /* 'π' */
#define A00_NOT_X      0xF9 /* '×' */
#define A00_LONG_Y     0xFA /* 'Ұ' */
#define A00_DIV        0xFD /* '÷' */
#define A00_BLOCK      0xFF /* '█' */

/**
 * Ritorna l'indirizzo di un carattere
 * 
 * @param c il carattere
 * @return l'indirizzo del carattere.
 */
int getA00char(int c);

#endif
