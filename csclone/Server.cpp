/*
 * Server.cpp
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#include "Server.h"

Server::Server(const Ogre::String& name)
	: mName(name), mMaxPlayers(20), mNumPlayers(0)
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
	buffer[0] = nmtServerInfo;
	//host name
	strcpy(&(buffer[1]), mName.c_str());
	pos = mName.length() + 2;
	//map name
	Ogre::String mapName = mWorldMgr.getMapName();
	strcpy(&(buffer[pos]), mapName.c_str());
	pos += mapName.length()+1;
	//num players
	memcpy(&(buffer[pos]), &mMaxPlayers, sizeof(Ogre::ushort));
	pos += sizeof(Ogre::ushort);
	memcpy(&(buffer[pos]), &mNumPlayers, sizeof(Ogre::ushort));
	pos += sizeof(Ogre::ushort);
	//send
	mSocket.sendTo(buffer, pos, 0, (struct sockaddr*)addr, len);
}

void Server::sendLoadMap(sockaddr_in* addr, int len)
{
	char buffer[128];
	buffer[0] = nmtLoadMap;
	strcpy(&(buffer[1]), mWorldMgr.getMapName().c_str());
	mSocket.sendTo(buffer, mWorldMgr.getMapName().length()+2, 0, (struct sockaddr*)addr, len);
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
	memset(&addr, 0, sizeof(struct sockaddr_in));
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
		case nmtConnect:
			{
			ClientInfo client;
			Ogre::UTFString playerName = &(buffer[1]);
			client.address = addr;
			mClients.insert(ClientInfoMap::value_type(playerName, client));
			sendLoadMap(&addr, sizeof(struct sockaddr_in));
			}
			break;
		default:
			break;
		}
	}
}

bool Server::loadMap(const Ogre::String& mapName)
{
	return mWorldMgr.loadMap(mapName);
}
