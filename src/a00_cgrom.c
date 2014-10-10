/**
 * @file a00_cgrom.c
 * Implementazione di a00_cgrom.h
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

#include "a00_cgrom.h"

int getA00char(int c){
	int code;
	
	if (' ' <= c && c <= '[')
		code = c;
	else if (']' <= c && c <= '}')
		code = c;
	else if (c == A00_YEN)
		code = 0x5C;
	else if (c == A00_ARROW_R)
		code = 0x7E;
	else if (c == A00_ARROW_L)
		code = 0x7F;
	else if (c == A00_DEGREE)
		code = 0xDF;
	else if (c == A00_ALPHA)
		code = 0xE0;
	else if (c == A00_2DOT_A)
		code = 0xE1;
	else if (c == A00_BETA)
		code = 0xE2;
	else if (c == A00_EPSILON)
		code = 0xE3;
	else if (c == A00_MU)
		code = 0xE4;
	else if (c == A00_SIGMA)
		code = 0xE5;
	else if (c == A00_RHO)
		code = 0xE6;
	else if (c == A00_LONG_G)
		code = 0xE7;
	else if (c == A00_SQRT)
		code = 0xE8;
	else if (c == A00_RECIPROCAL)
		code = 0xE9;
	else if (c == A00_IMAGINARY)
		code = 0xEA;
	else if (c == A00_SUP_X)
		code = 0xEB;
	else if (c == A00_CENT)
		code = 0xEC;
	else if (c == A00_LIRA)
		code = 0xED;
	else if (c == A00_N_TILDE)
		code = 0xEE;
	else if (c == A00_2DOT_O)
		code = 0xEF;
	else if (c == A00_LONG_P)
		code = 0xF0;
	else if (c == A00_LONG_Q)
		code = 0xF1;
	else if (c == A00_THETA)
		code = 0xF2;
	else if (c == A00_INFINITY)
		code = 0xF3;
	else if (c == A00_OMEGA)
		code = 0xF4;
	else if (c == A00_2DOT_U)
		code = 0xF5;
	else if (c == A00_SSIGMA)
		code = 0xF6;
	else if (c == A00_PI)
		code = 0xF7;
	else if (c == A00_NOT_X)
		code = 0xF8;
	else if (c == A00_LONG_Y)
		code = 0xF9;
	else if (c == A00_DIV)
		code = 0xFD;
	else if (c == A00_BLOCK)
		code = 0xFF;
	else
		code = ' ';
	
	return code;
}
