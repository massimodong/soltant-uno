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
#include <sys/socket.h>
#include <netinet/in.h>

/*
 * below are intended to be configurable
 */
#define SERVER_PORT 1111

char Buff[BUFFER_MAX_SIZE];

/*
 * tackles handshake failure
 * param:
 *   fd -- client socket file descriptor
 */
void handshake_failure(int fd){
	close(fd);
}

/*
 * complete handshake and get game id
 * param:
 *   fd -- client socket file descriptor
 * return:
 *   game_id data in handshake header, -1 if in error
 */
int handshake(int fd){
	int len, game_id = -1;
	len = read(fd, Buff, BUFFER_MAX_SIZE);
	Buff[len] = '\0';
	
#ifndef NDEBUG
	fprintf(stderr, "read len %d\n", len);
	fprintf(stderr, "%s", Buff);
#endif

	struct HttpHeader *hh = newHttpHeader(Buff);

	if(trie_query(hh->fields, "sec-websocket-key") != NULL){
		fprintf(stderr, "WebSocket!!\n");
	}else{
		fprintf(stderr, "Http!!\n");
		return -1;
	}

	sscanf(hh->request_uri, "/game/%d", &game_id);

	fprintf(stderr, "game id: %d", game_id);

	//todo
	
	delHttpHeader(hh);
	
	return game_id;
}

/*
 * help a client connect to game over websocket protocol
 * param:
 *   fd -- client socket file descriptor
 */
void resolve_client(int fd){
	handshake(fd);
	//todo
	close(fd);
}

/*
 * start the websocket server.
 * listen to clients and guide them to game
 */
void websocket_work(){
	int web_socket, client_socket;
	while ((web_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Failed to create socket, retrying.");
		sleep(5);
	}

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERVER_PORT);
	while (bind(web_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind failed, retrying.");
		sleep(5);
	}

	while(listen(web_socket, 3) < 0){
		perror("Listen failed, retrying.");
		sleep(5);
	}

	while(1){
		client_socket = accept(web_socket, (struct sockaddr *)&addr, &addrlen);
		resolve_client(client_socket);
		break;
	}
	close(web_socket);
}