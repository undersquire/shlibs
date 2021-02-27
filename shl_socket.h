/**
 * BSD 3-Clause License
 * 
 * Copyright (c) 2021, undersquire
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SHL_SOCKET_H
#define SHL_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

enum shl_socket_type {
	SHL_SOCKET_TCP
};

typedef int shl_socket_t;

struct shl_socket_info {
	char address[64];
	unsigned short port;
};

/* Open a socket using specified backend */
shl_socket_t shl_socket_open(int type);

/* Connect a socket to the specified remote host */
int shl_socket_connect(shl_socket_t socket, char *address, unsigned short port);

/* Bind a socket to the specified port and start listening */
int shl_socket_listen(shl_socket_t socket, unsigned short port);

/* Accept a socket and store info into specified client_info structure */
shl_socket_t shl_socket_accept(shl_socket_t socket, struct shl_socket_info *info);

/* Send data through connected socket */
int shl_socket_send(shl_socket_t socket, char *buf, int size);

/* Receive data from connected socket */
int shl_socket_recv(shl_socket_t socket, char *buf, int size);

/* Close a socket */
void shl_socket_close(shl_socket_t socket);

#ifdef SHL_SOCKET_IMPL

#ifdef _WIN32

#include <windows.h>

shl_socket_t shl_socket_open(int type)
{
	shl_socket_t s = -1;

	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
		return s;

	switch (type) {
		case SHL_SOCKET_TCP:
			s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			break;
	}

	return s;
}

int shl_socket_connect(shl_socket_t socket, char *address, unsigned short port)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;

	addr.sin_port = port;
	inet_pton(AF_INET, address, &addr.sin_addr);

	return connect(socket, (struct sockaddr *)&addr, sizeof(addr));
}

int shl_socket_listen(shl_socket_t socket, unsigned short port)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;

	addr.sin_port = port;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(socket, (struct sockaddr *)&addr, sizeof(addr)) != 0)
		return -1;

	return listen(socket, SOMAXCONN);
}

shl_socket_t shl_socket_accept(shl_socket_t socket, struct shl_socket_info *info)
{
	shl_socket_t client = -1;

	struct sockaddr_in addr;
	int addr_size = sizeof(addr);

	client = accept(socket, (struct sockaddr *)&addr, &addr_size);

	if (client == -1)
		return client;

	if (info != NULL) {
		info->port = addr.sin_port;
		inet_ntop(AF_INET, &addr.sin_addr, info->address, INET_ADDRSTRLEN);
	}

	return client;
}

int shl_socket_send(shl_socket_t socket, char *buf, int size)
{
	return send(socket, buf, size, 0);
}

int shl_socket_recv(shl_socket_t socket, char *buf, int size)
{
	return recv(socket, buf, size, 0);
}

void shl_socket_close(shl_socket_t socket)
{
	closesocket(socket);
}

#else

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

shl_socket_t shl_socket_open(int type)
{
	shl_socket_t socket = -1;

	switch (type) {
		case SHL_SOCKET_TCP:
			socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			break;
	}

	return socket;
}

int shl_socket_connect(shl_socket_t socket, char *address, unsigned short port)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;

	addr.sin_port = port;
	inet_pton(AF_INET, address, &(addr.sin_addr));

	return connect(socket, (struct sockaddr *)&addr, sizeof(addr));
}

int shl_socket_listen(shl_socket_t socket, unsigned short port)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;

	addr.sin_port = port;
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket, (struct sockaddr *)&addr, sizeof(addr)) != 0)
		return -1;

	return listen(socket, SOMAXCONN);
}

shl_socket_t shl_socket_accept(shl_socket_t socket, struct shl_socket_info *info)
{
	shl_socket_t client = -1;

	struct sockaddr_in addr;
	int addr_size = sizeof(addr);

	client = accept(socket, (struct sockaddr *)&addr, &addr_size);

	if (client == -1)
		return -1;

	if (info != NULL) {
		info->port = addr.sin_port;
		inet_ntop(AF_INET, &(addr.sin_addr), info->address, INET_ADDRSTRLEN);
	}

	return client;
}

int shl_socket_send(shl_socket_t socket, char *buf, int size)
{
	return send(socket, buf, size, 0);
}

int shl_socket_recv(shl_socket_t socket, char *buf, int size)
{
	return recv(socket, buf, size, 0);
}

void shl_socket_close(shl_socket_t socket)
{
	close(socket);
}

#endif /* UNIX */

#endif /* IMPL */

#ifdef __cplusplus
}
#endif

#endif /* SHL_SOCKET_H */
