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

const char version[] = "0.1";

void print_version(){
	printf("soltant-uno %s\n",version);
	printf("Copyright (C) 2018 Free Software Foundation, Inc.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n");
}

void print_help(){
	printf("Usage: %s [OPTION]...\n", PROGRAM_NAME);
	printf("An Uno game server over WebSocket protocol written in C\n");
	puts("");
	puts("\
	-h, --help        display this help and exit\n\
	-v, --version     display version information and exit\n");

	puts("");
	puts("\
	-d, --debug           debug mode\n");

	puts("");
	puts("Report bugs to: dongmassimo@gmail.com\n\
pkg home page: <>\n\
General help using GNU software: <http://www.gnu.org/gethelp/>\n");
}
