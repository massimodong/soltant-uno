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

/************ communication part ***************/
/*
 * broadcast messages
 */
void trie_dfs_send_message(struct Trie *tr, void *parp){
	struct Par{
		byte *buffer;
		size_t len;
	};
	fprintf(stderr, "fd: %d\n", ((struct Player*)tr->value)->wb_client.fd);
	websocket_send_binary(((struct Player*)tr->value)->wb_client.fd,
	                      ((struct Par *)parp)->buffer,
	                      ((struct Par *)parp)->len);
}

void broadcast(struct Game *game, byte *msg, size_t len){
	struct{
		byte *buffer;
		size_t len;
	}par = {msg, len};
	trie_enumerate(game->players, trie_dfs_send_message, &par);
}

/*
 * broadcast that a new player added
 */
void broadcast_new_player(struct Game *game, const char *username){
	byte buff[30];
	*(uint32_t *)buff = htonl(0);
	*(uint32_t *)(buff + 4) = htonl(strlen(username));
	strcpy((char *)(buff + 8), username);
	broadcast(game, buff, 8 + strlen(username));
}

/************* end of communication part *********/

/*
 * add a new player to a game, return new player id (index)
 * param:
 *   game -- game struct
 */
int game_new_player(struct Game *game, const char *username){
	struct Player *player = trie_query(game->players, username);
	if(player != NULL) return 0;

	broadcast_new_player(game, username);

	player = malloc(sizeof(struct Player));
	trie_insert(game->players, username, player);
	return 1;
}

/*
 * determines if all the players are prepared
 * param:
 *   game -- game struct
 * return:
 *   0 or 1
 */
int game_all_players_prepared(struct Game *game){
	/*
	for(int i=0;i<game->players_cnt;++i){
		if(game->players[i].prepared == 0) return 0;
	}
	*/
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

/*********** commands *******************/

/*
 * a new player enters,
 * we attempt to register it to the game
 * and tell the client the result
 * params:
 *   [0 - 3]: int, player websocket file descriptor
 *   [4 - 8]: int *, file descriptor to write result, an integer 0 or 1 is to be written
 *   [9 - ?]: string, username
 */
void new_player(struct Game *game, const char *username, byte *par){
	int fd = ntohl(*(uint32_t *)par),
		pfd = ntohl(*(uint32_t *)(par + 4));

	int result = game_new_player(game, username);
	write(pfd, &result, 4);
	close(pfd);

	if(!result) return;

	struct Player *player = trie_query(game->players, username);
	memset(player, 0, sizeof(struct Player));
	player->wb_client.fd = fd;
	player->username = malloc(sizeof(char) * (USERNAME_MAX_SIZE + 1));
	strcpy(player->username, username);

#ifndef NDEBUG
	fprintf(stderr, "new Player: %s\n", username);
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
void player_prepare(struct Game *game, const char *username, byte *par){
	/*
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
*/
}

/*
 * yell uno!
 * set player as `yelled_uno`
 */
void player_yell_uno(struct Game *game, const char *username, byte *par){
	//fprintf(stderr, "player %d yells uno!\n", player_id);
}

/*
 * player plays a card, following the rules of the game
 */
void player_play_card(struct Game *game, const char *username, byte *par){
}

/*
 * player decides to draw one card from deck instead of playing a card
 */
void player_draw1(struct Game *game, const char *username, byte *par){
}

/*
 * player can't/won't play a card after drawing one card from deck
 */
void player_skip_turn(struct Game *game, const char *username, byte *par){
}

/*
 * player reports one user not yelling uno
 */
void player_report_uno(struct Game *game, const char *username, byte *par){
}

/*
 * if some player times out, others can report it to the server
 * the server decides what the timed-out player do
 */
void player_report_timeout(struct Game *game, const char *username, byte *par){
}

/*
 * challenge the previous player's draw 4 card
 */
void player_challenge_draw4(struct Game *game, const char *username, byte *par){
}

/*
 * in case of challenge of a CARD_WILD_DRAW4,
 * deny to show the hand and draw 4 cards instead
 */
void player_challenge_draw4_deny(struct Game *game, const char *username, byte *par){
}

/*
 * in case of challenge of a CARD_WILD_DRAW4,
 * show the hand,
 * if challenge success, draw 4 cards instead,
 * otherwise, opponent draws 6 cards.
 */
void player_challenge_draw4_accept(struct Game *game, const char *username, byte *par){
}

/***************end of commands *********************/

const void (*commands[])(struct Game *, const char*, byte *) = {
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
void uno_game_proceed(struct Game *game, const char *username, uint32_t command, byte *par){
	if(command >= sizeof(commands)/sizeof(void *)){
		fprintf(stderr, "invalid command!\n");
		return;
	}
	
	commands[command](game, username, par);
}

/*
 * init a uno game
 * params:
 *   game -- Uno game struct
 */
void uno_init(struct Game *game){
	memset(game, 0, sizeof(struct Game));
	game->players = newTrie();
}