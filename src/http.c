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

/*
 * This is a data structure to store a http header for further use.
 * The entries are stored as a Trie.
 */

/*
 * set request_uri and get parameters
 * params:
 *   hh -- a HttpHeader
 *   ri -- request_uri
 * return:
 *  1 on success, -1 on unsuccess
 */
int set_request_uri(struct HttpHeader *hh, char *ri){
	char *path, *token, *path_saveptr, *token_saveptr;

	path = strtok_r(ri, "?", &path_saveptr);
	if(path == NULL) return -1;
	hh->request_uri = malloc(sizeof(char) * (strlen(path) + 1));
	strcpy(hh->request_uri, path);

	hh->get_params = newTrie();
	
	path = strtok_r(NULL, "?", &path_saveptr);
	token = strtok_r(path, "&", &token_saveptr);
	while(token != NULL){
		char *name = token, *value = token;
		while(*value != '='){
			if(*value == '\0') goto loop_end;
			++value;
		}
		*value = '\0';
		++value;

		char *_mem = malloc(sizeof(char)*(strlen(value)+1));
		strcpy(_mem, value);
		trie_insert(hh->get_params, name, _mem);
loop_end:
		token = strtok_r(NULL, "&", &token_saveptr);
	}
	return 1;
}

/*
 * examine request line (first line of a http header),
 * and set request_uri (which will be something as `/path/subpath`),
 * and method (`post` or `get`) for a HttpHeader
 * and get parameters
 * params:
 *   hh -- a HttpHeader
 *   rl -- request line (first line of a http header)
 * return:
 *   1 on success, -1 on unsuccess
 */
int set_request_line(struct HttpHeader *hh, char *rl){
	char *token, *saveptr;

	token = strtok_r(rl, " ", &saveptr);
	if(token == NULL) return -1;
	string_to_lowercase(token);
	if(strcmp(token, "get")!=0 && strcmp(token, "post")!=0){
		return -1;
	}
	hh->method = malloc(sizeof(char)*5);
	strcpy(hh->method, token);

	token = strtok_r(NULL, " ", &saveptr);
	if(token == NULL){
		free(hh->method);
		return -1;
	}
	/*
	hh->request_uri = malloc(sizeof(char)*(strlen(token)+1));
	strcpy(hh->request_uri, token);
	*/
	set_request_uri(hh, token);

	token = strtok_r(NULL, " ", &saveptr);
	if(token != NULL) string_to_lowercase(token);
	if(token == NULL || strcmp(token, "http/1.1") != 0){
		free(hh->method);
		free(hh->request_uri);
		return -1;
	}

	return 1;
}

/*
 * create a new HttpHeader using the http header string
 * param:
 *   str -- http header string
 * return:
 *   hh -- new HttpHeader
 */
struct HttpHeader *newHttpHeader(char *str){
	char *request_line, *field, *header_saveptr;
	request_line = strtok_r(str, "\r\n", &header_saveptr);

	if(request_line == NULL){
		return NULL;
	}

	struct HttpHeader *nhh = malloc(sizeof(struct HttpHeader));
	if(set_request_line(nhh, request_line) < 0){
		free(nhh);
		return NULL;
	}
	

	nhh->fields = newTrie();

	field = strtok_r(NULL, "\r\n", &header_saveptr);
	while(field != NULL){
		char *name = field, *value = field;
		while(*value != ':'){
			if(*value == '\0') goto loop_end;
			++value;
		}
		*value = '\0';
		++value;

		char *_mem = malloc(sizeof(char)*(strlen(value)+1));
		strcpy(_mem, value);
		string_to_lowercase(name);
		trie_insert(nhh->fields, name, _mem);
		
loop_end:
		field = strtok_r(NULL, "\r\n", &header_saveptr);
	}

	return nhh;
}

/*
 * destroy a HttpHeader
 * param:
 *   hh -- HttpHeader to destroy
 */
void delHttpHeader(struct HttpHeader *hh){
	free(hh->request_uri);
	free(hh->method);
	delTrie(hh->fields);
	free(hh);
}