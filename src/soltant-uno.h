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
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_MAX_SIZE 2048
#define WebSocket_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

extern char *PROGRAM_NAME;
extern int SERVER_PORT;

/*
 * below are shared datatypes
 */
struct Trie{
	 struct Trie *go[1<<8];
	 void *value;
};

struct HttpHeader{
	 char *request_uri;
	 char *method;
	 struct Trie *fields;
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