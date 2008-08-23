/*
 * Client.cpp
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#include "Server.h"
#include "Client.h"

Client::Client(WorldManagerClient* worldMgr)
	: mWorldMgr(worldMgr)
{
	mSocket.enableBoadcast(true);
}

Client::~Client()
{
}

void Client::sendFindServer()
{
	char buffer[128];
	buffer[0] = (char)nmtFindServer;
	strcpy(&(buffer[1]), "Servers CS Clone, where are you?");
	mSocket.sendTo(buffer, sizeof("Servers CS Clone, where are you?")+2, 0, "127.0.0.1"/*"192.168.0.255"*/, DEFAULT_PORT);
}

bool Client::loadMap(const Ogre::String& mapName)
{
	return mWorldMgr->loadMap(mapName);
}

void Client::checkRecv()
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
		case nmtMapInfo:
			break;
		default:
			break;
		}
	}
}
