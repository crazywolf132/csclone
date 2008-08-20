/*
 * CloneServer.cpp
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#include "CloneServer.h"

Server::Server(const char* host, uint16_t port)
{
	mSocket = socket(PF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host);
}

Server::~Server()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	closesocket(mSocket);
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	close(mSocket);
#endif
}
