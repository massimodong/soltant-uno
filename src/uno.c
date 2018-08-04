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

int game_new_player(struct Game *game, int fd){
	if(game->players_size == game->players_cnt){
		game->players_size += 1;
		game->players_size *= 2;
		game->players = realloc(game->players, sizeof(struct Player) * game->players_size);
	}
	//todo: initialize player
	int player_id = game->players_cnt++;
	return player_id;
}

/*
 * a new player enters,
 * we will associate it a player id, and send it back to the thread.
 * params:
 *   [0 - 3]: int, player websocket file descriptor
 *   [4 - 8]: int *, file descriptor to write player id
 */
void new_player(struct Game *game, int player_id, byte *par){
	int fd = ntohl(*(uint32_t *)par),
		pfd = ntohl(*(uint32_t *)(par + 4));

	uint32_t new_player_id = game_new_player(game, fd);
	write(pfd, &new_player_id, 4);
	close(pfd);
	return;
}

/*
 * set the player as `prepared` and set `username`
 * do nothing if already prepared
 * start the game if there are more than one player and this is the last player not prepared
 * no undo
 */
void player_prepare(struct Game *game, int player_id, byte *par){
}

/*
 * yell uno!
 * set player as `yelled_uno`
 */
void player_yell_uno(struct Game *game, int player_id, byte *par){
	fprintf(stderr, "player %d yells uno!\n", player_id);
}

/*
 * player plays a card, following the rules of the game
 */
void player_play_card(struct Game *game, int player_id, byte *par){
}

/*
 * if some player times out, others can report it to the server
 * the server decides what the timed-out player do
 */
void player_report_timeout(struct Game *game, int player_id, byte *par){
}

const void (*commands[])(struct Game *, int, byte *) = {
	new_player,                            // 0
	player_yell_uno,                       // 1
	player_prepare,                        // 2
	player_play_card,                      // 3
	player_report_timeout                  // 4
};

/*
 * proceed the command by user
 * if command invalid, do nothing.
 * params:
 *   game -- current game status
 *   command -- command to proceed
 *   par -- parameters
 */
void uno_game_proceed(struct Game *game, int player_id, uint32_t command, byte *par){
	if(command >= sizeof(commands)/sizeof(void *)){
		return;
	}
	
	commands[command](game, player_id, par);
}