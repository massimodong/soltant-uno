/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2018 Massimo <dongmassimo@gmail.com>
 * 
 * soltant-uno is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * soltant-uno is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "soltant-uno.h"

/*
 * swap two elements
 * params:
 *   a -- address a
 *   b -- address b
 *   size -- size of datatype
 */
void swap(void *a, void *b, size_t size){
	if(a == b) return;
	static void *buff = NULL;
	buff = realloc(buff, size);
	memcpy(buff, a, size);
	memcpy(a, b, size);
	memcpy(b, buff, size);
}

/*
 * returns a random unsigned integer
 */
unsigned int randu(){
	static FILE *rd = NULL;
	unsigned int ret;
	if(rd == NULL){
		rd = fopen("/dev/urandom", "r");
	}
	fread(&ret, sizeof(unsigned int), 1, rd);
	return ret;
}

/*
 * shuffles an array
 * uses the randomized selection sort algorithm
 * params:
 *   begin -- begin address of array
 *   end -- end address of array
 *   size -- size of datatype
 */
void rand_shuffle(void *begin, void *end, size_t size){
	if((end - begin) % size != 0){
		perror("wrong size to shuffle!\n");
		exit(EXIT_FAILURE);
	}
	int cnt = (end-begin) / size;
	for(int i=0;i<cnt-1;++i){
		int p = (randu() % (cnt - i)) + i;
		swap(begin + i*size, begin + p*size, size);
	}
}