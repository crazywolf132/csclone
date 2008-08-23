/*
 * Socket.h
 *
 *  Created on: 21.08.2008
 *      Author: sa-bu
 */

#ifndef __CLONE_SOCKET_H__
#define __CLONE_SOCKET_H__

#include <OgrePlatform.h>
#include <OgreString.h>

#include <string.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <winsock2.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#endif

#define DEFAULT_PORT 27015

enum NetMessageType {
	nmtNone,
	//server send
	nmtPing,
	nmtFindClient,
	nmtChatMessage,
	nmtRadioMessage,
	nmtMapInfo,
	//client send
	nmtPong,
	nmtFindServer,
	nmtChatMessageToAll,
	nmtChatMessageToForce,
};

class Socket
{
protected:
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	SOCKET mSocket;
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	int mSocket;
#endif

public:
	Socket();
	~Socket();

	void enableBoadcast(bool enable);
	void enableBlocking(bool enable);

	void bind(const char* host, unsigned short port);
	bool sendTo(const void* buffer, int length, int flags, const struct sockaddr* to, int tolen);
	bool sendTo(const void* buffer, int length, int flags, const char* host, unsigned short port);
	int recvFrom(void* buf, int flags, struct sockaddr* from, int* fromlen);

	void sendBegin(const char* host, unsigned short port, NetMessageType type);
	void sendNext(void* buffer, int length);
	void sendEnd();

	static void checkError();
	static void initialise();
	static void uninitialise();
};

#endif
