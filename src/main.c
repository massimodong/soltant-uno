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
#include <getopt.h>

char *PROGRAM_NAME;
int SERVER_PORT = 11111;

static const struct option longopts[] = {
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'v' },
	{ "port", required_argument, NULL, 'p' },
	{ NULL, 0, NULL, 0 },
};


int main(int argc, char *argv[])
{
	PROGRAM_NAME = argv[0];
	int optc, lose = 0;
	while((optc = getopt_long(argc, argv, "g:hntv", longopts, NULL)) != -1){
		switch(optc){
			case 'v':
				print_version();
				exit(EXIT_SUCCESS);
			case 'h':
				print_help();
				exit(EXIT_SUCCESS);
			case 'p':
				SERVER_PORT = strtol(optarg, NULL, 0);
				break;
			default:
				lose = 1;
				break;
		}
	}

	if(lose || optind < argc){
		if(optind < argc){
			fprintf(stderr,"%s: extra operand: %s\n",
			        PROGRAM_NAME, argv[optind]);
		}else{
			fprintf(stderr,"Try `%s --help` for more information.\n",
			        PROGRAM_NAME);
		}
		exit(EXIT_FAILURE);
	}

	websocket_work();

	return (0);
}