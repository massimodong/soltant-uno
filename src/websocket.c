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
 *   b64_val -- a pointer to the base64 encoded string
 */
void base64_encode(byte *value, int len, char * b64_val){
	FILE *stream = fmemopen(b64_val, len * 2, "w");
	BIO *b64 = BIO_new(BIO_f_base64()), *bio = BIO_new_fp(stream, BIO_NOCLOSE);
	BIO_push(b64, bio);
	BIO_write(b64, value, len);
 	BIO_flush(b64);
 	BIO_free_all(b64);
	fclose(stream);
}

/*
 * produce `Sec-WebSocket-Accept` value
 * param:
 *   key -- Sec-Websocket-key value
 *   acv -- `Sec-WebSocket-Accept`,
 *          Must have length at least SHA_DIGEST_LENGTH * 2
 */
void get_websocket_accept_value(char *key, char *acv){
	char *token = malloc(sizeof(char)*
	                     (strlen(key) + strlen(WebSocket_GUID) + 1));
	strcpy(token, key);

	string_trim_whitespaces(token);
	strcat(token, WebSocket_GUID);

	byte *sh = malloc(sizeof(byte) * SHA_DIGEST_LENGTH);
	SHA1((byte *)token, strlen(token), sh);
	free(token);

	base64_encode(sh, SHA_DIGEST_LENGTH, acv);
	free(sh);
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
 * connect to game server
 * and complete handshake
 * param:
 *   fd -- client socket file descriptor
 *   username -- pointer to store username
 * return:
 *   gid -- game id, -1 if error
 */
int handshake(int fd, char *username){
	char *buff = malloc(sizeof(char) * BUFFER_MAX_SIZE);
	int len, game_id = -1;
	len = read(fd, buff, BUFFER_MAX_SIZE);
	buff[len] = '\0';

	struct HttpHeader *hh = newHttpHeader(buff);

	char *socket_key = trie_query(hh->fields, "sec-websocket-key");
	if(socket_key != NULL){
		fprintf(stderr, "WebSocket!!\n");
	}else{
		fprintf(stderr, "Http!!\n");
		return -1;
	}

	sscanf(hh->request_uri, "/game/%d", &game_id);

#ifndef NDEBUG
	fprintf(stderr, "new username: %s\n", (char *)trie_query(hh->get_params, "username"));
#endif
	if(!game_register(game_id, trie_query(hh->get_params, "username"), fd)) return -1;
	strcpy(username, trie_query(hh->get_params, "username"));

	char *accept_value = malloc(sizeof(char) * SHA_DIGEST_LENGTH * 2);
	get_websocket_accept_value(socket_key, accept_value);

	dprintf(fd, "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n", accept_value);
	
	delHttpHeader(hh);
	free(buff);
	free(accept_value);

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
	char username[USERNAME_MAX_SIZE + 1];
	int gid = handshake(fd, username);
	if(gid > 0) start_player(gid, username, fd);
	close(fd);
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