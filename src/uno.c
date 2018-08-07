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
#include "cards.h"

/*
 * add a new player to a game, return new player id (index)
 * param:
 *   game -- game struct
 */
int game_new_player(struct Game *game){
	if(game->players_size == game->players_cnt){
		game->players_size += 1;
		game->players_size *= 2;
		game->players = realloc(game->players, sizeof(struct Player) * game->players_size);
	}
	int player_id = game->players_cnt++;
	memset(game->players + player_id, 0, sizeof(struct Player));
	return player_id;
}

/*
 * determines if all the players are prepared
 * param:
 *   game -- game struct
 * return:
 *   0 or 1
 */
int game_all_players_prepared(struct Game *game){
	for(int i=0;i<game->players_cnt;++i){
		if(game->players[i].prepared == 0) return 0;
	}
	return 1;
}

/*
 * add a set of `CARDS_NUM` cards to deck
 * and then shuffle the ADDED CARDS
 * param:
 *   game -- game struct
 */
void uno_add_deck(struct Game *game){
	game->deck_size += CARDS_NUM;
	game->deck = realloc(game->deck, sizeof(int) * game->deck_size);

	for(int i=game->deck_cnt-1;i>=0;--i) game->deck[i+CARDS_NUM] = game->deck[i];

	for(int i=0;i<CARDS_NUM;++i) game->deck[i]=i;
	rand_shuffle(game->deck, game->deck + CARDS_NUM, sizeof(int));

	game->deck_cnt += CARDS_NUM;
}

/*
 * start a game.
 */
void start_game(struct Game *game){
	game->status = UNO_STATUS_NORMAL;
	game->direction = game->cur_player = 0;
	uno_add_deck(game);
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

	uint32_t new_player_id = game_new_player(game);
	write(pfd, &new_player_id, 4);
	close(pfd);

	game->players[new_player_id].wb_client.fd = fd;

#ifndef NDEBUG
	fprintf(stderr, "new Player with id: %d\n", new_player_id);
#endif
	return;
}

/*
 * set the player as `prepared` and set `username`
 * do nothing if already prepared
 * start the game if there are more than one player and this is the last player not prepared
 * no undo
 * params:
 *   [0 - 3]: int, player username length, 20 maximum
 *   [4 - x]: string, player username
 */
void player_prepare(struct Game *game, int player_id, byte *par){
	int length;
	char *username;
	length = ntohl(*(int32_t *)par);
	username = (char *)(par + 4);

	if(game->status != UNO_STATUS_WAITING){
		return;
	}
	if(game->players[player_id].prepared){
		return;
	}

	game->players[player_id].prepared = 1;
	game->players[player_id].username = malloc(sizeof(char) * (length + 1));
	memcpy(game->players[player_id].username, username, sizeof(char) * length);
	game->players[player_id].username[length] = '\0';

	if(game->players_cnt >= 2 && game_all_players_prepared(game)){
		start_game(game);
	}
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
 * player decides to draw one card from deck instead of playing a card
 */
void player_draw1(struct Game *game, int player_id, byte *par){
}

/*
 * player can't/won't play a card after drawing one card from deck
 */
void player_skip_turn(struct Game *game, int player_id, byte *par){
}

/*
 * player reports one user not yelling uno
 */
void player_report_uno(struct Game *game, int player_id, byte *par){
}

/*
 * if some player times out, others can report it to the server
 * the server decides what the timed-out player do
 */
void player_report_timeout(struct Game *game, int player_id, byte *par){
}

/*
 * challenge the previous player's draw 4 card
 */
void player_challenge_draw4(struct Game *game, int player_id, byte *par){
}

/*
 * in case of challenge of a CARD_WILD_DRAW4,
 * deny to show the hand and draw 4 cards instead
 */
void player_challenge_draw4_deny(struct Game *game, int player_id, byte *par){
}

/*
 * in case of challenge of a CARD_WILD_DRAW4,
 * show the hand,
 * if challenge success, draw 4 cards instead,
 * otherwise, opponent draws 6 cards.
 */
void player_challenge_draw4_accept(struct Game *game, int player_id, byte *par){
}

const void (*commands[])(struct Game *, int, byte *) = {
	new_player,                            // 0
	player_yell_uno,                       // 1
	player_prepare,                        // 2
	player_play_card,                      // 3
	player_draw1,                          // 4
	player_skip_turn,                      // 5
	player_report_uno,                     // 6
	player_report_timeout,                 // 7
	player_challenge_draw4,                // 8
	player_challenge_draw4_deny,           // 9
	player_challenge_draw4_accept,         // 10
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

/*
 * init a uno game
 * params:
 *   game -- Uno game struct
 */
void uno_init(struct Game *game){
	memset(game, 0, sizeof(struct Game));
}