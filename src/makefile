##
# makefile
# This file is part of lcd-qc1602a-i2c
#
# Copyright (C) 2014 - Mirco Bertelli
#
# lcd-qc1602a-i2c is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# lcd-qc1602a-i2c is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with lcd-qc1602a-i2c. If not, see <http://www.gnu.org/licenses/>.
##

.PHONY : runtest clean docu

CC = gcc
CFALGS = -Wall -pedantic

test : test.o i2c_hd44780.o
	$(CC) $(CFALGS) $^ -o $@ -lwiringPi

test.o : test.c i2c_hd44780.h
	$(CC) $(CFALGS) -c $< -o $@

i2c_hd44780.o: i2c_hd44780.c i2c_hd44780.h
	$(CC) $(CFALGS) -c $< -o $@

runtest :
	./test

clean:
	-rm *.o *~

docu:
	doxygen

