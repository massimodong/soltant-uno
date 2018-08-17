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
	char username[USERNAME_MAX_SIZE + 1];
	byte *command_buff = malloc(sizeof(byte) * COMMAND_SIZE);
	struct Game *game = malloc(sizeof(struct Game));
	uno_init(game);

	while(1){
		read(fd, command_buff, sizeof(char) * COMMAND_SIZE);
		uint32_t command_name = ntohl(*(uint32_t *)command_buff);
		strcpy(username, (char *)(command_buff + COMMAND_SIZE - USERNAME_MAX_SIZE - 1));
		byte *command_par = command_buff + COMMAND_NAME_SIZE;

#ifndef NDEBUG
		fprintf(stderr, "new command %d\nplayer: %s\n", (int)command_name, username);
#endif

		uno_game_proceed(game, username, command_name, command_par);
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
 *   username -- username
 *   fd -- client socket file descriptor
 * return:
 *   1 or 0
 */
int game_register(int game_id, const char *username, int fd){
	byte *buff = malloc(sizeof(byte) * COMMAND_SIZE);
	int pfd[2];
	pipe(pfd);

	*(uint32_t *)buff = htonl(0); //new player
	*(uint32_t *)(buff + 4) = htonl(fd);
	*(uint32_t *)(buff + 8) = htonl(pfd[1]);
	strcpy((char *)(buff + COMMAND_SIZE - USERNAME_MAX_SIZE - 1), username);
	write(game_pipes[game_id], buff, COMMAND_SIZE);

	uint32_t result;
	read(pfd[0], &result, 4);
	close(pfd[0]);

	return (int)result;
}

/*
 * receive commands from client
 * concatenate username at the end of whole command as USERNAME_MAX_SIZE + 1 bytes
 * and send them to game
 * params:
 *   gfd -- game file descriptor
 *   username -- username
 *   fd -- client file descriptor
 */
void transmit_commands(int gfd, const char *username, int fd){
	byte *msg_buff = malloc(sizeof(byte) * BUFFER_MAX_SIZE),
		*command_buff = malloc(sizeof(byte) * (BUFFER_MAX_SIZE + 8));
	int msg_buff_len = 0, len, res=0;

	while(res == 0){
		res = websocket_receive_frame(fd, msg_buff, &msg_buff_len, command_buff, &len);
		if(res){
			*(uint32_t *)command_buff = htonl(2);
		}
		strcpy((char *)(command_buff + COMMAND_SIZE - USERNAME_MAX_SIZE - 1), username);
#ifndef NDEBUG
		fprintf(stderr, "received frame len %d\n", len);
#endif
		write(gfd, command_buff, COMMAND_SIZE); //atomic because COMMAND_SIZE is 512
	}
}

/*
 * start a player
 * registers the player to the game
 * and transmits commands
 * params:
 *   game_id -- game id
 *   username -- username
 *   fd -- player websocket client file descriptor
 */
void start_player(int gid, const char *username, int fd){
	transmit_commands(game_pipes[gid], username, fd);
}