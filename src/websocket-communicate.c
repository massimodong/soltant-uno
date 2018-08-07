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
#include <arpa/inet.h>

/*
 * receive one frame data from websocket file descriptor,
 * BLOCKS if no frame incoming or frame not complete.
 * maxmimum frame length is BUFFER_MAX_SIZE
 * frame figure:
 *    0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
			 
 * params:
 *   fd -- wesocket file descriptor
 *   buff -- a buffer which contains previously received data
 *   buff_len_ptr -- pointer to valid length of buff
 *   frame -- return value, frame data received
 *   frame_len_ptr -- return value, length of received frame data, not set if NULL
 */
void websocket_receive_frame(int fd, byte *buff, int *buff_len_ptr, byte *frame, int *frame_len_ptr){
	int buff_len = *buff_len_ptr;
	
	int frame_len = 0, fin, opcode, mask, payload_len, offset;
	byte masking_key[4];

	do{
		offset = 0;

		while(buff_len < 2){ //valid length too short that `Payload len` is not included
			buff_len += read(fd, buff + buff_len, BUFFER_MAX_SIZE - buff_len);
		}
		fin = (*buff) >> 7;
		opcode = (*buff) & 0x0f;
		mask = (*(buff + 1)) >> 7;
		payload_len = (*(buff + 1)) & 0x7f;

		offset += 2;

#ifndef NDEBUG
		fprintf(stderr, "<new frame>\nfin : %d\nopcode: %d\nmask: %d\npayload len:%d\n", fin, opcode, mask, payload_len);
#endif

		if(!mask){
			//todo: close connection, because according to the protocol, client's mask must be 1
		}

		if(payload_len == 126){
			while(buff_len < offset + 2){
				buff_len += read(fd, buff + buff_len, BUFFER_MAX_SIZE - buff_len);
			}
			uint16_t length = 0;
			memcpy(&length, buff + offset, 2);
			payload_len = ntohs(length);
			offset += 2;
		}else if(payload_len == 127){
			//todo: close connection, because frame too large
			/* if accept such length
			while(buff_len < offset + 8){
				buff_len += read(fd, buff + buff_len, BUFFER_MAX_SIZE - buff_len);
			}
			uint64_t length = 0;
			memcpy(&length, buff + offset, 8);
			payload_len = be64toh(length);
			offset += 8;
			*/
		}
#ifndef NDEBUG
		fprintf(stderr, "new payload len: %d\n", payload_len);
#endif
		if(payload_len > BUFFER_MAX_SIZE - 14){ //14 bytes for frame information
			//todo: close connection, because frame too large
		}

		if(payload_len + frame_len > BUFFER_MAX_SIZE){
			//todo: close connection, because frame too large
		}
	
		memcpy(masking_key, buff + offset, 4);
		offset += 4;

		while(buff_len < offset + payload_len){
			buff_len += read(fd, buff + buff_len, BUFFER_MAX_SIZE - buff_len);
		}

		memcpy(frame + frame_len, buff + offset, payload_len);
	
		for(int i=0;i<payload_len;++i){
			frame[frame_len + i] ^= masking_key[i % 4];
		}

		frame_len += payload_len;
		offset += payload_len;

		//memcpy(buff, buff + offset, buff_len - offset);
		for(int i=offset;i<buff_len;++i) buff[i-offset] = buff[i];
		buff_len -= offset;
	}while(!fin);

	*buff_len_ptr = buff_len;
	if(frame_len_ptr) *frame_len_ptr= frame_len;
}

/*
 * send binary data as a frame over websocket protocol
 * params:
 *   fd -- websocket file descriptor
 *   data -- binary data to send
 *   len -- binary data length
 */
void websocket_send_binary(int fd, byte *data, uint64_t len){
	byte *buff = malloc(sizeof(byte) * (len + 20));

	*buff = 0x82; // 10000010
	int offset = 0;
	if(len < 126){
		*(buff + 1) = len;
		offset = 2;
	}else if(len <= USHRT_MAX){
		*(buff + 1) = 126;
		*(uint16_t *)(buff + 2) = htons(len);
		offset = 4;
	}else{
		*(buff + 1) = 127;
		*(uint64_t *)(buff + 2) = htobe64(len);
		offset = 10;
	}

	memcpy(buff + offset, data, len);

	write(fd, buff, offset + len);

	free(buff);
}