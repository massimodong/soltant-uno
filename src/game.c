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

int game_pipes[MAX_GAMES + 1];
int free_gids[MAX_GAMES], free_gids_top;

/*
 * init the `gree_gids` stack with all available game ids
 */
void init_free_gids(){
	free_gids_top = 0;
	for(int i=1;i<=MAX_GAMES;++i) free_gids[free_gids_top++] = i;
}

/*
 * run a game
 * param:
 *   fdp -- commands file descriptor cast to void*
 */
void *run_game(void *fdp){
	int fd = VOIDP2INT(fdp);
	unsigned char *command_buff = malloc(sizeof(unsigned char) * COMMAND_SIZE);
	struct Game *game = malloc(sizeof(struct Game));
	memset(game, 0, sizeof(struct Game));

	while(1){
		read(fd, command_buff, sizeof(char) * COMMAND_SIZE);
		int64_t command_name = *(int64_t *)command_buff;
		unsigned char *command_par = command_buff + COMMAND_NAME_SIZE;

#ifndef NDEBUG
		fprintf(stderr, "command: %d\nparams:\n", (int)command_name);
		for(int i=0;i<COMMAND_PAR_SIZE;++i){
			fprintf(stderr, "%d: %d", i, (int)command_par[i]);
			if(isprint(command_par[i])){
				fprintf(stderr, " -- <%c>\n", command_par[i]);
			}else{
				fprintf(stderr, "\n");
			}
		}
#endif
	}

	free(command_buff);
	free(game);
}

/*
 * create a new game
 * return:
 *   return new game id if success, 0 if fails.
 */
int new_game(){
	if(!free_gids_top) return 0;
	int gid = free_gids[--free_gids_top];

	int pfd[2];
	pipe(pfd);

	game_pipes[gid] = pfd[1];
	pthread_t ptid;
	pthread_create(&ptid, NULL, run_game, INT2VOIDP(pfd[0]));

	return gid;
}

/*
 * send new client file descriptor to game
 * if game doesn't exists, do nothing
 * params:
 *   game_id -- game id
 *   fd -- client socket file descriptor
 */
void game_send_client_fd(int game_id, int fd){
	unsigned char *buff = malloc(sizeof(char) * COMMAND_SIZE);
	*(int64_t *)buff = 2334333;
	strcpy((char *)(buff) + COMMAND_NAME_SIZE, "Test Data");
	write(game_pipes[game_id], buff, COMMAND_SIZE);
}