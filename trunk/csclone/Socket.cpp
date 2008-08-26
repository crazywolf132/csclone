/*
 * Socket.cpp
 *
 *  Created on: 21.08.2008
 *      Author: sa-bu
 */

#include "Socket.h"

void Socket::initialise()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	WSADATA wsaData;
	int error = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (error != 0)
		checkError();
#endif
}

void Socket::uninitialise()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	int error = WSACleanup();
	if (error == SOCKET_ERROR)
		checkError();
#endif
}

#include <OgreStringConverter.h>

void Socket::checkError()
{
	std::string errmsg = "Socket Error: ";
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	char msg[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		LANG_USER_DEFAULT, msg, 1024, NULL);
	errmsg += msg;
/*
	switch(GetLastError()) {
	case WSANOTINITIALISED:
		errmsg += "";
		break;
	default:
		errmsg += "Error code: ";
		errmsg += Ogre::StringConverter::toString(errno);
	}
*/
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	switch(errno) {
	case EPROTONOSUPPORT:
		errmsg += "Protocol not supported";
		break;
    case EAFNOSUPPORT:
		errmsg += "Address family not supported by protocol";
        break;
	case EADDRNOTAVAIL:
		errmsg += "Cannot assign requested address";
		break;
	default:
		errmsg += "Error code: ";
		errmsg += Ogre::StringConverter::toString(errno);
	}
#endif
	errmsg += "\n";
	printf(errmsg.c_str());
}

Socket::Socket() : mSocket(0)
{
	mSocket = socket(PF_INET, SOCK_DGRAM, 0);
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (mSocket == INVALID_SOCKET)
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	if (mSocket == -1)
#endif
		checkError();
}

Socket::~Socket()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	closesocket(mSocket);
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	close(mSocket);
#endif
}

void Socket::enableBoadcast(bool enable)
{
	int val = enable;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof(int)) == SOCKET_ERROR)
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	if (setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof(int)) == -1)
#endif
		checkError();
}

void Socket::enableBlocking(bool enable)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	unsigned long flag = !enable;
	if (ioctlsocket(mSocket, FIONBIO, &flag) == SOCKET_ERROR)
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	if (fcntl(mSocket, F_SETFL, O_NONBLOCK) == -1)
#endif
		checkError();
}

void Socket::bind(const char* host, unsigned short port)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = host ? inet_addr(host) : INADDR_ANY;

	int error = ::bind(mSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (error == SOCKET_ERROR)
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	if (error == -1)
#endif
		checkError();
}

bool Socket::sendTo(const void* buffer, int length, int flags, const struct sockaddr* to, int tolen)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (sendto(mSocket, (const char*)buffer, length, flags, to, tolen) != SOCKET_ERROR)
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	if (sendto(mSocket, buffer, length, flags, to, tolen) != -1)
#endif
		return true;
	checkError();
	return false;
}

bool Socket::sendTo(const void* buffer, int length, int flags, const char* host, unsigned short port)
{
	static struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host);

	return sendTo(buffer, length, flags, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
}

int Socket::recvFrom(void* buf, int flags, struct sockaddr* from, int* fromlen)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	int ret = recvfrom(mSocket, (char*)buf, 128, flags, from, fromlen);
	if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	int ret = recvfrom(mSocket, (char*)buf, 128, flags, from, (socklen_t*)fromlen);
	if (ret == -1 && errno != EAGAIN)
#endif
		checkError();
	return ret;
}
