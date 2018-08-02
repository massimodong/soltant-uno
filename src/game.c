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

pid_t game_pids[MAX_GAMES + 1];
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
 *   gid -- game id
 */
void run_game(int gid){
	struct Game *game = malloc(sizeof(struct Game));
	memset(game, 0, sizeof(struct Game));
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
	pid_t pid = game_pids[gid] = fork();
	if(pid){
		return gid;
	}else{
		run_game(gid);
		exit(EXIT_SUCCESS);
	}
}

/*
 * send new client file descriptor to game
 * if game doesn't exists, do nothing
 * params:
 *   game_id -- game id
 *   fd -- client socket file descriptor
 */
void game_send_client_fd(int game_id, int fd){
}