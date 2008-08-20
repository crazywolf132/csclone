/*
 * CloneServer.h
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#ifndef __CLONE_SERVER_H__
#define __CLONE_SERVER_H__

#include <OgrePlatform.h>

#include <string.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <winsock2.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

class Server
{
protected:
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	SOCKET mSocket;
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	int mSocket;
#endif

public:
	Server(const char* host, uint16_t port);
	~Server();
};

#endif
