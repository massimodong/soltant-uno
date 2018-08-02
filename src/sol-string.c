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

#define is_uppercase(c) ('A' <= (c) && (c) <= 'Z')

/*
 * convert all uppercase letters to lower case
 * param:
 *   str -- string to convert
 */
void string_to_lowercase(char *str){
	if(str == NULL) return;
	for(int i=0;str[i]!='\0';++i){
		if(is_uppercase(str[i])){
			str[i]=str[i]-'A'+'a';
		}
	}
}

/*
 * trim leading/trailing whitespaces
 * param:
 *   str -- string to trim
 */
void string_trim_whitespaces(char *str){
	if(str == NULL) return;
	int sp=0, tp = strlen(str), len;
	for(;isspace(str[sp]);++sp);
	for(;isspace(str[tp-1]);--tp);
	len = tp - sp;
	for(int i=0;i<len;++i) str[i] = str[i+sp];
	str[len] = '\0';
}