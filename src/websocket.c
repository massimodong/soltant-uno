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
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

/*
 * base64 encode input binary string
 * params:
 *   value -- binary string to encode
 *   len -- string length
 * return:
 *   a static pointer to the base64 encoded string
 */
char *base64_encode(unsigned char *value, unsigned int len){
	static char *base64_value = NULL;
	base64_value = realloc(base64_value, sizeof(char) * len * 2);
	FILE *stream = fmemopen(base64_value, len * 2, "w");
	BIO *b64 = BIO_new(BIO_f_base64()), *bio = BIO_new_fp(stream, BIO_NOCLOSE);
	BIO_push(b64, bio);
	BIO_write(b64, value, SHA_DIGEST_LENGTH);
 	BIO_flush(b64);
 	BIO_free_all(b64);
	fclose(stream);
	return base64_value;
}

/*
 * produce `Sec-WebSocket-Accept` value
 * param:
 *   key -- Sec-Websocket-key value
 * return:
 *   valid `Sec-WebSocket-Accept`
 */
char *get_websocket_accept_value(char *key){
	char *token = malloc(sizeof(char)*
	                     (strlen(key) + strlen(WebSocket_GUID) + 1));
	strcpy(token, key);

	string_trim_whitespaces(token);
	strcat(token, WebSocket_GUID);

	unsigned char *sh = SHA1((unsigned char *)token, strlen(token), NULL);
	free(token);

	return base64_encode(sh, SHA_DIGEST_LENGTH);
}

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
	char *buff = malloc(sizeof(char) * BUFFER_MAX_SIZE);
	int len, game_id = -1;
	len = read(fd, buff, BUFFER_MAX_SIZE);
	buff[len] = '\0';
	
#ifndef NDEBUG
	fprintf(stderr, "read len %d\n", len);
	fprintf(stderr, "%s", buff);
#endif

	struct HttpHeader *hh = newHttpHeader(buff);

	char *socket_key = trie_query(hh->fields, "sec-websocket-key");
	if(socket_key != NULL){
		fprintf(stderr, "WebSocket!!\n");
	}else{
		fprintf(stderr, "Http!!\n");
		return -1;
	}

	sscanf(hh->request_uri, "/game/%d", &game_id);

	char *accept_value = get_websocket_accept_value(socket_key);

	dprintf(fd, "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n", accept_value);
	
	delHttpHeader(hh);
	free(buff);

#ifndef NDEBUG
	fprintf(stderr, "Handshake complete with game id : %d\n", game_id);
#endif
	
	return game_id;
}

/*
 * help a client connect to game over websocket protocol
 * param:
 *   fd -- client socket file descriptor
 */
void *resolve_client(void *fdp){
	int fd=VOIDP2INT(fdp);
	int game_id = handshake(fd);
	start_player(game_id, fd);
	return NULL;
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
		pthread_t ptid;
		pthread_create(&ptid, NULL, resolve_client, INT2VOIDP(client_socket));
	}
	close(web_socket);
}