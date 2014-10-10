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

#define A00_YEN '¥'
#define A00_ARROW_R '→'
#define A00_ARROW_L '←'
#define A00_DEGREE '°'
#define A00_ALPHA 'α'
#define A00_2DOT_A 'ä'
#define A00_BETA 'β'
#define A00_EPSILON 'ε'
#define A00_MU 'μ'
#define A00_SIGMA 'σ'
#define A00_RHO 'ρ'
#define A00_LONG_G 'ǥ'
#define A00_SQRT '√'
#define A00_RECIPROCAL '¯'
#define A00_IMAGINARY 'ⅉ'
#define A00_SUP_X '˟'
#define A00_CENT '¢'
#define A00_LIRA '£'
#define A00_N_TILDE 'ñ'
#define A00_2DOT_O 'ö'
#define A00_LONG_P 'Ƿ'
#define A00_LONG_Q 'Ǫ'
#define A00_THETA 'θ'
#define A00_INFINITY '∞'
#define A00_OMEGA 'Ω'
#define A00_2DOT_U 'ü'
#define A00_SSIGMA 'Σ'
#define A00_PI 'π'
#define A00_NOT_X '×'
#define A00_LONG_Y 'Ұ'
#define A00_DIV '÷'
#define A00_BLOCK '█'

/**
 * Ritorna l'indirizzo di un carattere
 * 
 * @param c il carattere
 * @return l'indirizzo del carattere.
 */
int getA00char(int c);

#endif
