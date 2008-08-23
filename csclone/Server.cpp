/*
 * Server.cpp
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#include "Server.h"

Server::Server(const Ogre::String& name)
	: mName(name)
{
	mSocket.enableBlocking(false);
	mSocket.enableBoadcast(true);
	mSocket.bind(0, DEFAULT_PORT);
}

Server::~Server()
{
}

void Server::sendServerInfo(sockaddr_in* addr, int len)
{
	int pos;
	char buffer[128];
	buffer[0] = nmtMapInfo;
	//host name
	strcpy(&(buffer[1]), mName.c_str());
	//map name
	Ogre::String mapName = mWorldManagerServer.getMapName();
	pos = mName.length() + 2;
	strcpy(&(buffer[pos]), mapName.c_str());
	mSocket.sendTo(buffer, strlen(buffer)+1, 0, (struct sockaddr*)addr, len);
}

void Server::sendChatMessageToAll(const Ogre::String& text)
{
	char buffer[128];
	buffer[0] = nmtChatMessage;
	strcpy(&(buffer[1]), text.c_str());
	mSocket.sendTo((void*)buffer, text.length()+2, 0, "192.168.0.255", DEFAULT_PORT);
}

void Server::checkRecv()
{
	char buffer[128];
	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);

	while (true) {
		int ret = mSocket.recvFrom(buffer, 0, (struct sockaddr*)&addr, &addr_len);
		if (ret < 1)
			return;
		NetMessageType type = (NetMessageType)buffer[0];
		switch (type) {
		case nmtFindServer:
			if (strcmp(&(buffer[1]), "Servers CS Clone, where are you?") == 0) {
				sendServerInfo(&addr, addr_len);
			}
			break;
		default:
			break;
		}
	}
}
