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

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <netinet/in.h>

#define BUFFER_MAX_SIZE 2048
#define CLIENT_BUFFER_MAX_SIZE 512
#define WebSocket_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

#define COMMAND_SIZE 512
#define COMMAND_NAME_SIZE 4
#define COMMAND_PAR_SIZE COMMAND_SIZE - COMMAND_NAME_SIZE

extern char *PROGRAM_NAME;
extern int SERVER_PORT;

#define INT2VOIDP(i) (void*)(uintptr_t)(i)
#define VOIDP2INT(p) (int)(uintptr_t)(p)

#define MAX_GAMES 255

/*
 * below are shared datatypes
 */
#define byte unsigned char

struct Trie{
	 struct Trie *go[1<<8];
	 void *value;
};

struct HttpHeader{
	 char *request_uri;
	 char *method;
	 struct Trie *fields;
};

struct WebSocket_Client{
	int fd;
};

struct Player{
	struct WebSocket_Client wb_client;
	char *username;
	int *deck, prepared, yelled_uno;
	size_t deck_size, deck_cnt;
};

enum {
	UNO_STATUS_WAITING = 1,
	UNO_STATUS_NORMAL,
	UNO_STATUS_DRAW1,
	UNO_STATUS_DRAW2,
	UNO_STATUS_DRAW4,
	UNO_STATUS_DRAW4_CHALLENGE,
};
struct Game{
	struct Player *players;
	int *deck, status, direction, cur_player;
	size_t players_size, players_cnt, deck_size, deck_cnt;
};

/*
 * below are shared functions
 */

// main
void string_to_lowercase(char *str);

// versions
void print_version();
void print_help();

// Trie
struct Trie *newTrie();
void delTrie(struct Trie *tr);
void trie_insert(struct Trie *, const char *, void *);
void *trie_query(struct Trie *, const char *);

// http
struct HttpHeader *newHttpHeader(char *);
void delHttpHeader(struct HttpHeader *);

// websocket
void websocket_work();

//sol-string
void string_to_lowercase(char *str);
void string_trim_whitespaces(char *str);

//game
void init_free_gids();
void start_player(int, int);
int new_game();

//websocket-communicate
void websocket_receive_frame(int, byte *, int *, byte *, int *);

//uno
void uno_init(struct Game *);
void uno_game_proceed(struct Game *, int, uint32_t, byte *);

//randoms
void swap(void *, void *, size_t);
unsigned int randu();
void rand_shuffle(void *, void *, size_t);