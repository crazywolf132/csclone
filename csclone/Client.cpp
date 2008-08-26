/*
 * Client.cpp
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#include "Server.h"
#include "Client.h"

Client::Client(WorldManagerClient* worldMgr)
	: mWorldMgr(worldMgr), mConnected(false), mName("sa-bu")
{
	mSocket.enableBlocking(false);
	mSocket.enableBoadcast(true);
}

Client::~Client()
{
	delete mWorldMgr;
}

void Client::sendFindServer()
{
	char buffer[128];
	buffer[0] = (char)nmtFindServer;
	strcpy(&(buffer[1]), "Servers CS Clone, where are you?");
	mSocket.sendTo(buffer, sizeof("Servers CS Clone, where are you?")+2, 0, "127.0.0.1"/*"192.168.0.255"*/, DEFAULT_PORT);
	mServers.clear();
}

void Client::sendConnect(struct sockaddr_in* address)
{
	char buffer[128];
	buffer[0] = (char)nmtConnect;
	strcpy(&(buffer[1]), mName.c_str());
	mSocket.sendTo(buffer, mName.length()+2, 0, (struct sockaddr*)address, sizeof(struct sockaddr_in));
}

bool Client::loadMap(const Ogre::String& mapName)
{
	return mWorldMgr->loadMap(mapName);
}

void Client::recvServerInfo(char* msg, sockaddr_in* addr)
{
	Ogre::UTFString serverName;
	ServerInfo info;

	serverName = msg;
	msg += serverName.length()+1;
	info.mapName = msg;
	msg += info.mapName.length()+1;
	info.maxPlayers = *((Ogre::ushort*)msg);
	msg += 2;
	info.numPlayers = *((Ogre::ushort*)msg);
	info.address = *addr;
	mServers.insert(ServerInfoMap::value_type(serverName, info));
}

void Client::checkRecv()
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
		case nmtServerInfo:
			recvServerInfo(&(buffer[1]), &addr);
			break;
		case nmtLoadMap:
			mConnected = true;
			mServerAddress = addr;
			mWorldMgr->loadMap(&(buffer[1]));
			break;
		default:
			break;
		}
	}
}

void Client::connectToServer(const char* host, unsigned short port)
{
	static struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host);

	sendConnect(&addr);
}

void Client::connectToServer(const Ogre::UTFString& serverName)
{
	sendConnect(&mServers.find(serverName)->second.address);
}
